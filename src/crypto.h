#ifndef CRYPTO_H
#define CRYPTO_H

/**
 * @file crypto.h
 * @brief Sistema de criptografia AES-256 para senhas
 * 
 * Usa OpenSSL para criptografar/descriptografar senhas
 * Método: AES-256-CBC com derivação de chave PBKDF2
 * 
 * Compile com: gcc ... -lssl -lcrypto
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <openssl/evp.h>

#define CRYPTO_KEY_SIZE 32          // 256 bits para AES-256
#define CRYPTO_IV_SIZE 16           // 128 bits para IV
#define CRYPTO_SALT_SIZE 16         // 128 bits para salt
#define CRYPTO_ITERATIONS 100000    // PBKDF2 iterations (segurança)
#define CRYPTO_MAX_OUTPUT 256       // Tamanho máximo de saída

typedef struct {
    unsigned char salt[CRYPTO_SALT_SIZE];
    unsigned char iv[CRYPTO_IV_SIZE];
    unsigned char ciphertext[CRYPTO_MAX_OUTPUT];
    int ciphertext_len;
} CryptoData;

/**
 * Derivar chave a partir de senha usando PBKDF2
 * Mais seguro que SHA simples
 */
static int crypto_derive_key(
    const char *password,
    const unsigned char *salt,
    unsigned char *key,
    unsigned char *iv
) {
    if (!PKCS5_PBKDF2_HMAC(
        password,
        strlen(password),
        salt,
        CRYPTO_SALT_SIZE,
        CRYPTO_ITERATIONS,
        EVP_sha256(),
        CRYPTO_KEY_SIZE,
        key
    )) {
        fprintf(stderr, "[CRYPTO] Erro ao derivar chave\n");
        return 0;
    }
    
    // IV derivado também da chave (determinístico)
    memcpy(iv, key + (CRYPTO_KEY_SIZE - CRYPTO_IV_SIZE), CRYPTO_IV_SIZE);
    
    return 1;
}

/**
 * Criptografar dados com AES-256-CBC
 * 
 * @param plaintext    Texto a criptografar
 * @param password     Senha para derivar chave
 * @param output       Estrutura CryptoData preenchida
 * @return             1 se sucesso, 0 se erro
 */
static int crypto_encrypt(
    const char *plaintext,
    const char *password,
    CryptoData *output
) {
    EVP_CIPHER_CTX *ctx;
    unsigned char key[CRYPTO_KEY_SIZE];
    unsigned char iv[CRYPTO_IV_SIZE];
    int len = 0;
    int ciphertext_len = 0;

    if (!plaintext || !password || !output) {
        fprintf(stderr, "[CRYPTO] Parâmetros inválidos\n");
        return 0;
    }

    // Gerar salt aleatório
    if (!RAND_bytes(output->salt, CRYPTO_SALT_SIZE)) {
        fprintf(stderr, "[CRYPTO] Erro ao gerar salt aleatório\n");
        return 0;
    }

    // Derivar chave e IV do salt e senha
    if (!crypto_derive_key(password, output->salt, key, iv)) {
        return 0;
    }

    // Criar contexto de criptografia
    ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        fprintf(stderr, "[CRYPTO] Erro ao criar contexto EVP\n");
        return 0;
    }

    // Inicializar criptografia AES-256-CBC
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) {
        fprintf(stderr, "[CRYPTO] Erro ao inicializar criptografia\n");
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    // Criptografar dados
    if (EVP_EncryptUpdate(
        ctx,
        output->ciphertext,
        &ciphertext_len,
        (unsigned char *)plaintext,
        strlen(plaintext)
    ) != 1) {
        fprintf(stderr, "[CRYPTO] Erro durante criptografia\n");
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    // Finalizar
    if (EVP_EncryptFinal_ex(ctx, output->ciphertext + ciphertext_len, &len) != 1) {
        fprintf(stderr, "[CRYPTO] Erro ao finalizar criptografia\n");
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    ciphertext_len += len;
    output->ciphertext_len = ciphertext_len;

    EVP_CIPHER_CTX_free(ctx);

    return 1;
}

/**
 * Descriptografar dados criptografados
 * 
 * @param data         Estrutura CryptoData com dados encriptados
 * @param password     Senha para derivar chave
 * @param plaintext    Buffer para output
 * @param max_len      Tamanho máximo do buffer
 * @return             1 se sucesso, 0 se erro
 */
static int crypto_decrypt(
    const CryptoData *data,
    const char *password,
    char *plaintext,
    int max_len
) {
    EVP_CIPHER_CTX *ctx;
    unsigned char key[CRYPTO_KEY_SIZE];
    unsigned char iv[CRYPTO_IV_SIZE];
    int len = 0;
    int plaintext_len = 0;

    if (!data || !password || !plaintext) {
        fprintf(stderr, "[CRYPTO] Parâmetros inválidos\n");
        return 0;
    }

    // Derivar chave e IV usando o salt do arquivo
    if (!crypto_derive_key(password, data->salt, key, iv)) {
        return 0;
    }

    // Criar contexto de descriptografia
    ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        fprintf(stderr, "[CRYPTO] Erro ao criar contexto EVP\n");
        return 0;
    }

    // Inicializar descriptografia AES-256-CBC
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) {
        fprintf(stderr, "[CRYPTO] Erro ao inicializar descriptografia\n");
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    // Descriptografar
    if (EVP_DecryptUpdate(
        ctx,
        (unsigned char *)plaintext,
        &plaintext_len,
        data->ciphertext,
        data->ciphertext_len
    ) != 1) {
        fprintf(stderr, "[CRYPTO] Erro durante descriptografia (senha errada?)\n");
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    // Finalizar
    if (EVP_DecryptFinal_ex(ctx, (unsigned char *)plaintext + plaintext_len, &len) != 1) {
        fprintf(stderr, "[CRYPTO] Erro ao finalizar descriptografia\n");
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    plaintext_len += len;
    plaintext[plaintext_len] = '\0';  // Null-terminate

    EVP_CIPHER_CTX_free(ctx);

    return (plaintext_len < max_len) ? 1 : 0;
}

/**
 * Converter CryptoData para formato hexadecimal (para arquivo)
 * Formato: SALT_HEX|IV_HEX|CIPHERTEXT_HEX
 */
static int crypto_to_hex(const CryptoData *data, char *hex_output, int max_len)
{
    int pos = 0;

    if (max_len < (CRYPTO_SALT_SIZE * 2 + CRYPTO_IV_SIZE * 2 + 
                   CRYPTO_MAX_OUTPUT * 2 + 10)) {
        fprintf(stderr, "[CRYPTO] Buffer muito pequeno\n");
        return 0;
    }

    // Salt em hex
    for (int i = 0; i < CRYPTO_SALT_SIZE; i++) {
        pos += sprintf(hex_output + pos, "%02x", data->salt[i]);
    }
    pos += sprintf(hex_output + pos, "|");

    // IV em hex
    for (int i = 0; i < CRYPTO_IV_SIZE; i++) {
        pos += sprintf(hex_output + pos, "%02x", data->iv[i]);
    }
    pos += sprintf(hex_output + pos, "|");

    // Ciphertext em hex
    for (int i = 0; i < data->ciphertext_len; i++) {
        pos += sprintf(hex_output + pos, "%02x", data->ciphertext[i]);
    }

    return 1;
}

/**
 * Converter formato hexadecimal de volta para CryptoData
 */
static int crypto_from_hex(const char *hex_input, CryptoData *data)
{
    if (!hex_input || !data) {
        fprintf(stderr, "[CRYPTO] Parâmetros inválidos\n");
        return 0;
    }

    char *input_copy = strdup(hex_input);
    char *token = NULL;
    int idx = 0;

    token = strtok(input_copy, "|");
    
    // Parse salt
    if (!token || strlen(token) != CRYPTO_SALT_SIZE * 2) {
        fprintf(stderr, "[CRYPTO] Formato inválido (salt)\n");
        free(input_copy);
        return 0;
    }
    for (int i = 0; i < CRYPTO_SALT_SIZE; i++) {
        sscanf(token + i * 2, "%2hhx", &data->salt[i]);
    }

    // Parse IV
    token = strtok(NULL, "|");
    if (!token || strlen(token) != CRYPTO_IV_SIZE * 2) {
        fprintf(stderr, "[CRYPTO] Formato inválido (iv)\n");
        free(input_copy);
        return 0;
    }
    for (int i = 0; i < CRYPTO_IV_SIZE; i++) {
        sscanf(token + i * 2, "%2hhx", &data->iv[i]);
    }

    // Parse ciphertext
    token = strtok(NULL, "|");
    if (!token || strlen(token) > CRYPTO_MAX_OUTPUT * 2) {
        fprintf(stderr, "[CRYPTO] Formato inválido (ciphertext)\n");
        free(input_copy);
        return 0;
    }
    data->ciphertext_len = strlen(token) / 2;
    for (int i = 0; i < data->ciphertext_len; i++) {
        sscanf(token + i * 2, "%2hhx", &data->ciphertext[i]);
    }

    free(input_copy);
    return 1;
}

/**
 * Debug: mostrar estrutura CryptoData em formato legível
 */
static void crypto_debug(const CryptoData *data)
{
    fprintf(stderr, "[CRYPTO DEBUG]\n");
    fprintf(stderr, "  Salt (%d bytes): ", CRYPTO_SALT_SIZE);
    for (int i = 0; i < CRYPTO_SALT_SIZE; i++) {
        fprintf(stderr, "%02x", data->salt[i]);
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "  IV (%d bytes): ", CRYPTO_IV_SIZE);
    for (int i = 0; i < CRYPTO_IV_SIZE; i++) {
        fprintf(stderr, "%02x", data->iv[i]);
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "  Ciphertext (%d bytes): ", data->ciphertext_len);
    for (int i = 0; i < data->ciphertext_len && i < 16; i++) {
        fprintf(stderr, "%02x", data->ciphertext[i]);
    }
    if (data->ciphertext_len > 16) fprintf(stderr, "...");
    fprintf(stderr, "\n");
}

#endif // CRYPTO_H
