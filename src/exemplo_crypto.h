#ifndef EXEMPLO_CRYPTO_H
#define EXEMPLO_CRYPTO_H

/**
 * @file exemplo_crypto.h
 * @brief Exemplos de como usar criptografia em main.c
 * 
 * Mostra diferentes formas de carregar senhas criptografadas
 */

#include "config_crypto.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * EXEMPLO 1: Carregar de arquivo criptografado (.enc)
 * Usuário digita senha mestre quando programa inicia
 */
static int exemplo_carregar_com_prompt(void)
{
    Credencial admin;
    char senha_mestre[100] = {0};
    
    printf("\n═══════════════════════════════════════════════════\n");
    printf("EXEMPLO 1: Login com Senha Mestre (Prompt)\n");
    printf("═══════════════════════════════════════════════════\n\n");
    
    // Pedir senha ao usuário (mais seguro - não grava em histórico)
    printf("🔐 Digite a SENHA MESTRE para desbloquear configurações:\n");
    printf("   (Esta é uma senha DIFERENTE do admin.txt)\n");
    printf("\n   Senha: ");
    
    // IMPORTANTE: Em produção, desabilitar echo
    // system("stty -echo");
    fgets(senha_mestre, sizeof(senha_mestre), stdin);
    // system("stty echo");
    
    // Remove newline
    size_t len = strlen(senha_mestre);
    if (len > 0 && senha_mestre[len-1] == '\n') {
        senha_mestre[len-1] = '\0';
    }
    
    printf("\n   Verificando credenciais...\n\n");
    
    // Carregar (tenta .enc primeiro, depois .txt)
    if (!carregar_credencial("admin.txt", senha_mestre, &admin)) {
        printf("   ❌ Falha ao carregar credenciais!\n");
        printf("   Possíveis causas:\n");
        printf("   • Senha mestre incorreta\n");
        printf("   • Arquivo admin.txt.enc não encontrado\n");
        printf("   • Arquivo corrompido\n\n");
        return 0;
    }
    
    printf("   ✅ Credenciais carregadas com sucesso!\n");
    printf("   Usuário: %s\n", admin.usuario);
    printf("   Tipo: %s\n\n", 
           admin.criptografado ? "🔐 Criptografado" : "📄 Texto Puro");
    
    return 1;
}

/**
 * EXEMPLO 2: Carregar de variável de ambiente
 * Mais seguro para produção (CI/CD, Docker, etc)
 */
static int exemplo_carregar_de_env(void)
{
    Credencial admin;
    
    printf("\n═══════════════════════════════════════════════════\n");
    printf("EXEMPLO 2: Carregar de Variável de Ambiente\n");
    printf("═══════════════════════════════════════════════════\n\n");
    
    // Pegar senha do ambiente
    const char *senha_env = getenv("BRT_MASTER_PASSWORD");
    
    if (!senha_env) {
        printf("❌ Variável BRT_MASTER_PASSWORD não está definida\n");
        printf("   Defina com: export BRT_MASTER_PASSWORD='sua_senha'\n\n");
        return 0;
    }
    
    printf("✅ Variável BRT_MASTER_PASSWORD detectada\n");
    printf("   Carregando credenciais...\n\n");
    
    // Carregar
    if (!carregar_credencial("admin.txt", senha_env, &admin)) {
        printf("❌ Falha ao carregar credenciais (senha incorreta?)\n\n");
        return 0;
    }
    
    printf("✅ Credenciais carregadas!\n");
    printf("   Usuário: %s\n", admin.usuario);
    printf("   Tipo: %s\n\n", 
           admin.criptografado ? "🔐 Criptografado" : "📄 Texto Puro");
    
    return 1;
}

/**
 * EXEMPLO 3: Carregar com fallback (cascata)
 * Tenta múltiplos métodos em ordem de segurança
 */
static int exemplo_carregar_com_fallback(void)
{
    Credencial admin;
    char senha_mestre[100] = {0};
    
    printf("\n═══════════════════════════════════════════════════\n");
    printf("EXEMPLO 3: Carregar com Fallback (Cascata)\n");
    printf("═══════════════════════════════════════════════════\n\n");
    
    // Estratégia 1: Variável de ambiente (mais seguro)
    const char *senha_env = getenv("BRT_MASTER_PASSWORD");
    
    if (senha_env) {
        printf("✅ Usando BRT_MASTER_PASSWORD (variável de ambiente)\n");
        if (carregar_credencial("admin.txt", senha_env, &admin)) {
            printf("   Credenciais carregadas com sucesso!\n\n");
            return 1;
        }
    }
    
    // Estratégia 2: Arquivo .env seguro
    FILE *f = fopen("/etc/brt/.env.secure", "r");
    if (f) {
        printf("✅ Usando /etc/brt/.env.secure\n");
        fgets(senha_mestre, sizeof(senha_mestre), f);
        fclose(f);
        
        // Remove newline
        size_t len = strlen(senha_mestre);
        if (len > 0 && senha_mestre[len-1] == '\n') {
            senha_mestre[len-1] = '\0';
        }
        
        if (carregar_credencial("admin.txt", senha_mestre, &admin)) {
            printf("   Credenciais carregadas com sucesso!\n\n");
            return 1;
        }
    }
    
    // Estratégia 3: Prompt do usuário
    printf("⚠️  Nenhuma variável de ambiente encontrada\n");
    printf("   Solicitando senha do usuário...\n\n");
    
    printf("🔐 Digite a SENHA MESTRE:\n");
    printf("   Senha: ");
    fgets(senha_mestre, sizeof(senha_mestre), stdin);
    
    // Remove newline
    size_t len = strlen(senha_mestre);
    if (len > 0 && senha_mestre[len-1] == '\n') {
        senha_mestre[len-1] = '\0';
    }
    
    if (!carregar_credencial("admin.txt", senha_mestre, &admin)) {
        printf("\n❌ Falha ao carregar credenciais\n\n");
        return 0;
    }
    
    printf("   ✅ Credenciais carregadas!\n\n");
    return 1;
}

/**
 * EXEMPLO 4: Validar credencial contra arquivo descriptografado
 * Mostra como usar as credenciais carregadas
 */
static int exemplo_validar_login(void)
{
    Credencial admin;
    char senha_mestre[100];
    char usuario_entrada[50];
    char senha_entrada[50];
    
    printf("\n═══════════════════════════════════════════════════\n");
    printf("EXEMPLO 4: Validar Login com Credencial Carregada\n");
    printf("═══════════════════════════════════════════════════\n\n");
    
    // Passo 1: Carregar credenciais (atrás das cortinas)
    printf("Passo 1: Desbloqueando arquivo de credenciais...\n");
    printf("🔐 Senha mestre: ");
    fgets(senha_mestre, sizeof(senha_mestre), stdin);
    
    // Remove newline
    size_t len = strlen(senha_mestre);
    if (len > 0 && senha_mestre[len-1] == '\n') {
        senha_mestre[len-1] = '\0';
    }
    
    if (!carregar_credencial("admin.txt", senha_mestre, &admin)) {
        printf("   ❌ Falha ao desbloquear\n\n");
        return 0;
    }
    
    printf("   ✅ Arquivo desbloqueado com sucesso!\n\n");
    
    // Passo 2: Pedir login do usuário
    printf("Passo 2: Login do sistema...\n");
    printf("   Usuário: ");
    fgets(usuario_entrada, sizeof(usuario_entrada), stdin);
    
    // Remove newline
    len = strlen(usuario_entrada);
    if (len > 0 && usuario_entrada[len-1] == '\n') {
        usuario_entrada[len-1] = '\0';
    }
    
    printf("   Senha: ");
    fgets(senha_entrada, sizeof(senha_entrada), stdin);
    
    // Remove newline
    len = strlen(senha_entrada);
    if (len > 0 && senha_entrada[len-1] == '\n') {
        senha_entrada[len-1] = '\0';
    }
    
    // Passo 3: Validar contra credencial carregada
    printf("\n   Validando...\n");
    
    if (strcmp(usuario_entrada, admin.usuario) != 0) {
        printf("   ❌ Usuário incorreto!\n\n");
        return 0;
    }
    
    if (strcmp(senha_entrada, admin.senha) != 0) {
        printf("   ❌ Senha incorreta!\n\n");
        return 0;
    }
    
    printf("   ✅ Credenciais validadas!\n");
    printf("   ✅ Login bem-sucedido como: %s\n\n", admin.usuario);
    
    return 1;
}

/**
 * EXEMPLO 5: Carregar múltiplas credenciais
 * Demonstra como carregar múltiplos tipos (admin, operador, etc)
 */
static int exemplo_multiplas_credenciais(void)
{
    Credencial admin, operador;
    char senha_mestre[100];
    
    printf("\n═══════════════════════════════════════════════════\n");
    printf("EXEMPLO 5: Carregar Múltiplas Credenciais\n");
    printf("═══════════════════════════════════════════════════\n\n");
    
    printf("🔐 Digite a SENHA MESTRE:\n");
    printf("   Senha: ");
    fgets(senha_mestre, sizeof(senha_mestre), stdin);
    
    // Remove newline
    size_t len = strlen(senha_mestre);
    if (len > 0 && senha_mestre[len-1] == '\n') {
        senha_mestre[len-1] = '\0';
    }
    
    printf("\n   Carregando credenciais de ADMIN...\n");
    if (!carregar_credencial("admin.txt", senha_mestre, &admin)) {
        printf("   ❌ Falha ao carregar admin.txt\n\n");
        return 0;
    }
    printf("   ✅ Admin carregado: %s\n\n", admin.usuario);
    
    printf("   Carregando credenciais de OPERADOR...\n");
    if (!carregar_credencial("operadores.txt", senha_mestre, &operador)) {
        printf("   ❌ Falha ao carregar operadores.txt\n\n");
        return 0;
    }
    printf("   ✅ Operador carregado: %s\n\n", operador.usuario);
    
    // Resumo
    printf("═══════════════════════════════════════════════════\n");
    printf("RESUMO DAS CREDENCIAIS CARREGADAS:\n");
    printf("═══════════════════════════════════════════════════\n");
    printf("Admin:\n");
    printf("  Usuário: %s\n", admin.usuario);
    printf("  Criptografado: %s\n", admin.criptografado ? "Sim" : "Não");
    printf("\nOperador:\n");
    printf("  Usuário: %s\n", operador.usuario);
    printf("  Criptografado: %s\n\n", operador.criptografado ? "Sim" : "Não");
    
    return 1;
}

#endif // EXEMPLO_CRYPTO_H
