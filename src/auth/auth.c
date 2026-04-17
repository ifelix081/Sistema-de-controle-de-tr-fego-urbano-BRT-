#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <openssl/sha.h>

#define FILE_PATH "credenciais.txt"

// ================= SHA256 =================
void sha256_string(const char *str, char output[65]) {
    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256((unsigned char*)str, strlen(str), hash);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }

    output[64] = 0;
}

// ================= SALT =================
void gerarSalt(char *salt, int tamanho) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    for (int i = 0; i < tamanho - 1; i++) {
        int key = rand() % (sizeof(charset) - 1);
        salt[i] = charset[key];
    }

    salt[tamanho - 1] = '\0';
}

// ================= REGISTRAR =================
int registrarUsuario() {
    char usuario[20], senha[20];
    char salt[16];
    char combinado[100];
    char hash[65];

    FILE *f = fopen(FILE_PATH, "a");
    if (!f) {
        printf("Erro ao abrir arquivo\n");
        return 0;
    }

    printf("Novo usuario: ");
    scanf("%19s", usuario);

    printf("Nova senha: ");
    scanf("%19s", senha);

    gerarSalt(salt, 16);

    sprintf(combinado, "%s%s", senha, salt);

    sha256_string(combinado, hash);

    fprintf(f, "%s:%s:%s\n", usuario, salt, hash);

    fclose(f);

    printf("Usuario registrado com sucesso!\n");
    return 1;
}

// ================= LOGIN =================
int loginUsuario() {
    char usr[20], sen[20];
    char fileUsr[20], fileSalt[20], fileHash[65];

    char combinado[100];
    char inputHash[65];

    FILE *f = fopen(FILE_PATH, "r");
    if (!f) {
        printf("Arquivo de credenciais nao encontrado\n");
        return 0;
    }

    printf("Usuario: ");
    scanf("%19s", usr);

    printf("Senha: ");
    scanf("%19s", sen);

    while (fscanf(f, "%19[^:]:%19[^:]:%64s\n", fileUsr, fileSalt, fileHash) == 3) {

        if (strcmp(usr, fileUsr) == 0) {

            sprintf(combinado, "%s%s", sen, fileSalt);

            sha256_string(combinado, inputHash);

            if (strcmp(inputHash, fileHash) == 0) {
                fclose(f);
                printf("Login realizado com sucesso!\n");
                return 1;
            }
        }
    }

    fclose(f);
    printf("Usuario ou senha incorretos!\n");
    return 0;
}