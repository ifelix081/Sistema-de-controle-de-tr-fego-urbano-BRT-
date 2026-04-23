#ifndef CONFIG_CRYPTO_H
#define CONFIG_CRYPTO_H

/**
 * @file config_crypto.h
 * @brief Integração de criptografia no sistema de configuração
 * 
 * Permite carregar senhas criptografadas de arquivos .enc
 * Requer: OpenSSL, crypto.h
 */

#include "crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Estrutura para armazenar credenciais
 */
typedef struct {
    char usuario[50];
    char senha[50];
    int criptografado;  // Flag para saber se veio de arquivo .enc
} Credencial;

/**
 * Carregar credencial de arquivo de texto simples (formato: usuario:senha)
 */
static int carregar_credencial_texto(
    const char *arquivo,
    Credencial *cred
) {
    FILE *f = fopen(arquivo, "r");
    if (!f) {
        return 0;
    }

    char linha[200];
    if (!fgets(linha, sizeof(linha), f)) {
        fclose(f);
        return 0;
    }
    fclose(f);

    // Parse: usuario:senha
    char *sep = strchr(linha, ':');
    if (!sep) {
        fprintf(stderr, "[CONFIG] Formato inválido: %s\n", arquivo);
        return 0;
    }

    int user_len = sep - linha;
    strncpy(cred->usuario, linha, user_len);
    cred->usuario[user_len] = '\0';

    strcpy(cred->senha, sep + 1);
    
    // Remove newline
    char *nl = strchr(cred->senha, '\n');
    if (nl) *nl = '\0';

    cred->criptografado = 0;
    return 1;
}

/**
 * Carregar credencial de arquivo criptografado (.enc)
 * 
 * @param arquivo_enc    Caminho do arquivo .enc
 * @param senha_master   Senha mestre para descriptografar
 * @param cred           Estrutura para preencer
 * @return               1 se sucesso, 0 se erro
 */
static int carregar_credencial_criptografada(
    const char *arquivo_enc,
    const char *senha_master,
    Credencial *cred
) {
    FILE *f = fopen(arquivo_enc, "r");
    if (!f) {
        fprintf(stderr, "[CONFIG] Arquivo criptografado não encontrado: %s\n", arquivo_enc);
        return 0;
    }

    // Ler conteúdo do arquivo
    char conteudo[4096] = {0};
    size_t read = fread(conteudo, 1, sizeof(conteudo) - 1, f);
    fclose(f);
    conteudo[read] = '\0';

    // Parse: HEADER|SALT_HEX|CIPHERTEXT_HEX
    char *part1 = strchr(conteudo, '|');
    if (!part1) {
        fprintf(stderr, "[CONFIG] Formato de arquivo criptografado inválido\n");
        return 0;
    }

    // Extrair componentes
    CryptoData crypto_data;
    if (!crypto_from_hex(conteudo, &crypto_data)) {
        fprintf(stderr, "[CONFIG] Erro ao parsear arquivo criptografado\n");
        return 0;
    }

    // Descriptografar
    char plaintext[256] = {0};
    if (!crypto_decrypt(&crypto_data, senha_master, plaintext, sizeof(plaintext))) {
        fprintf(stderr, "[CONFIG] Erro ao descriptografar (senha mestre incorreta?)\n");
        return 0;
    }

    // Parse: usuario:senha
    char *sep = strchr(plaintext, ':');
    if (!sep) {
        fprintf(stderr, "[CONFIG] Formato inválido em arquivo criptografado\n");
        return 0;
    }

    int user_len = sep - plaintext;
    strncpy(cred->usuario, plaintext, user_len);
    cred->usuario[user_len] = '\0';

    strcpy(cred->senha, sep + 1);
    cred->criptografado = 1;

    return 1;
}

/**
 * Carregar credencial com busca em cascata:
 * 1. Arquivo criptografado (.enc) - mais seguro
 * 2. Arquivo de texto - menos seguro
 * 3. Variáveis de ambiente
 */
static int carregar_credencial(
    const char *nome_base,          // ex: "admin.txt"
    const char *senha_master,       // para descriptografar .enc
    Credencial *cred
) {
    char arquivo_enc[256];
    char arquivo_txt[256];

    snprintf(arquivo_enc, sizeof(arquivo_enc), "src/config/%s.enc", nome_base);
    snprintf(arquivo_txt, sizeof(arquivo_txt), "src/config/%s", nome_base);

    fprintf(stderr, "[CONFIG] Buscando credenciais para: %s\n", nome_base);

    // Prioridade 1: Arquivo criptografado (.enc)
    if (carregar_credencial_criptografada(arquivo_enc, senha_master, cred)) {
        fprintf(stderr, "[CONFIG] ✅ Carregado de arquivo criptografado\n");
        fprintf(stderr, "[CONFIG]    Arquivo: %s\n", arquivo_enc);
        fprintf(stderr, "[CONFIG]    Usuário: %s\n", cred->usuario);
        return 1;
    }

    // Prioridade 2: Arquivo de texto simples
    if (carregar_credencial_texto(arquivo_txt, cred)) {
        fprintf(stderr, "[CONFIG] ⚠️  Carregado de arquivo de texto (MENOS SEGURO!)\n");
        fprintf(stderr, "[CONFIG]    Arquivo: %s\n", arquivo_txt);
        fprintf(stderr, "[CONFIG]    Usuário: %s\n", cred->usuario);
        fprintf(stderr, "[CONFIG]    💡 Considere criptografar com: python3 crypto_util.py encrypt src/config/%s\n", nome_base);
        return 1;
    }

    // Prioridade 3: Variáveis de ambiente
    char env_user[64];
    char env_pass[64];
    snprintf(env_user, sizeof(env_user), "BRT_%s_USER", nome_base);
    snprintf(env_pass, sizeof(env_pass), "BRT_%s_PASS", nome_base);

    const char *user_env = getenv(env_user);
    const char *pass_env = getenv(env_pass);

    if (user_env && pass_env) {
        strncpy(cred->usuario, user_env, sizeof(cred->usuario) - 1);
        strncpy(cred->senha, pass_env, sizeof(cred->senha) - 1);
        cred->criptografado = 0;
        fprintf(stderr, "[CONFIG] ✅ Carregado de variáveis de ambiente\n");
        fprintf(stderr, "[CONFIG]    %s e %s\n", env_user, env_pass);
        return 1;
    }

    fprintf(stderr, "[CONFIG] ❌ Nenhuma fonte de credencial encontrada!\n");
    fprintf(stderr, "[CONFIG]    Procurou por:\n");
    fprintf(stderr, "[CONFIG]      • %s (criptografado)\n", arquivo_enc);
    fprintf(stderr, "[CONFIG]      • %s (texto)\n", arquivo_txt);
    fprintf(stderr, "[CONFIG]      • %s (env)\n", env_user);
    fprintf(stderr, "[CONFIG]      • %s (env)\n\n", env_pass);

    return 0;
}

#endif // CONFIG_CRYPTO_H
