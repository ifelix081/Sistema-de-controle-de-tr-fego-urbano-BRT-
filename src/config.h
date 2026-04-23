#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

/**
 * @file config.h
 * @brief Gerenciamento robusto de caminhos de configuração
 * 
 * Estratégias para encontrar arquivos de configuração:
 * 1. Relativo ao executável (mais confiável para aplicações instaladas)
 * 2. Variável de ambiente CONFIG_PATH
 * 3. Diretório home do usuário
 * 4. Diretório padrão do projeto
 */

typedef struct {
    char admin_path[256];
    char operadores_path[256];
} ConfigPaths;

/**
 * Diagnostica e mostra onde o programa está procurando pelos arquivos
 */
static void diagnostico_caminhos(void)
{
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd))) {
        fprintf(stderr, "📍 Diretório de execução: %s\n", cwd);
    }
    
    char exe_path[256] = {0};
#ifdef __linux__
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len > 0) {
        exe_path[len] = '\0';
        fprintf(stderr, "📍 Executável em: %s\n", exe_path);
    }
#elif defined(_WIN32)
    GetModuleFileNameA(NULL, exe_path, sizeof(exe_path));
    fprintf(stderr, "📍 Executável em: %s\n", exe_path);
#endif
}

/**
 * Verifica se arquivo existe
 */
static int arquivo_existe(const char *path)
{
    FILE *f = fopen(path, "r");
    if (f) {
        fclose(f);
        return 1;
    }
    return 0;
}

/**
 * Localiza arquivo usando múltiplas estratégias
 * Retorna 1 se encontrado, 0 caso contrário
 */
static int localizar_arquivo(const char *nome_arquivo, char *resultado, size_t tam)
{
    char caminho[256];
    
    // Estratégia 1: Relativo ao CWD (mais comum em desenvolvimento)
    snprintf(caminho, sizeof(caminho), "src/config/%s", nome_arquivo);
    if (arquivo_existe(caminho)) {
        snprintf(resultado, tam, "%s", caminho);
        fprintf(stderr, "[OK] Encontrado: %s\n", caminho);
        return 1;
    }
    
    // Estratégia 2: Relativo ao executable (para builds)
    char exe_path[256] = {0};
#ifdef __linux__
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len > 0) {
        exe_path[len] = '\0';
        snprintf(caminho, sizeof(caminho), "%s/../config/%s", 
                dirname(exe_path), nome_arquivo);
        if (arquivo_existe(caminho)) {
            snprintf(resultado, tam, "%s", caminho);
            fprintf(stderr, "[OK] Encontrado relativo ao exe: %s\n", caminho);
            return 1;
        }
    }
#endif
    
    // Estratégia 3: Variável de ambiente
    const char *config_env = getenv("CONFIG_PATH");
    if (config_env) {
        snprintf(caminho, sizeof(caminho), "%s/%s", config_env, nome_arquivo);
        if (arquivo_existe(caminho)) {
            snprintf(resultado, tam, "%s", caminho);
            fprintf(stderr, "[OK] Encontrado via CONFIG_PATH: %s\n", caminho);
            return 1;
        }
    }
    
    // Estratégia 4: Diretório home
    const char *home = getenv("HOME");
    if (home) {
        snprintf(caminho, sizeof(caminho), "%s/.config/brt/%s", home, nome_arquivo);
        if (arquivo_existe(caminho)) {
            snprintf(resultado, tam, "%s", caminho);
            fprintf(stderr, "[OK] Encontrado no home: %s\n", caminho);
            return 1;
        }
    }
    
    // Falha: arquivo não encontrado
    fprintf(stderr, "❌ Arquivo não encontrado: %s\n", nome_arquivo);
    fprintf(stderr, "   Locais procurados:\n");
    fprintf(stderr, "   • src/config/%s\n", nome_arquivo);
    fprintf(stderr, "   • (relativo ao executável)/config/%s\n", nome_arquivo);
    if (config_env) {
        fprintf(stderr, "   • %s/%s\n", config_env, nome_arquivo);
    }
    if (home) {
        fprintf(stderr, "   • %s/.config/brt/%s\n", home, nome_arquivo);
    }
    
    return 0;
}

/**
 * Inicializa estrutura de caminhos de configuração
 * Retorna 1 se bem-sucedido, 0 caso contrário
 */
static int config_inicializar(ConfigPaths *cfg)
{
    int sucesso = 1;
    
    fprintf(stderr, "\n\033[36m[DEBUG]\033[0m Iniciando busca de arquivos de configuração...\n");
    
    if (!localizar_arquivo("admin.txt", cfg->admin_path, sizeof(cfg->admin_path))) {
        fprintf(stderr, "\033[31m[ERRO]\033[0m Não foi possível localizar admin.txt\n");
        sucesso = 0;
    }
    
    if (!localizar_arquivo("operadores.txt", cfg->operadores_path, sizeof(cfg->operadores_path))) {
        fprintf(stderr, "\033[31m[ERRO]\033[0m Não foi possível localizar operadores.txt\n");
        sucesso = 0;
    }
    
    if (sucesso) {
        fprintf(stderr, "\033[32m[OK]\033[0m Todos os arquivos de configuração encontrados\n\n");
    } else {
        diagnostico_caminhos();
        fprintf(stderr, "\n");
    }
    
    return sucesso;
}

#endif // CONFIG_H
