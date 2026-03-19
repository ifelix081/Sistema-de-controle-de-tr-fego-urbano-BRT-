#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

struct BRT {
    int linha;
    int numero_onibus;
    char placa[10];
    char garagem[50];
    char motorista[50];
    char turno[10];
};

void cadastro(){
    FILE *relatoriobrt = fopen("relatoriobrt.txt", "a");
    struct BRT b;

    if (relatoriobrt == NULL){
        printf("Erro na criação do arquivo!\n");
        return;
    }

    printf("---Cadastro BRT---\n");

    printf("Linha: ");
    scanf("%d", &b.linha);

    printf("Número do onibus: ");
    scanf("%d", &b.numero_onibus);

    printf("Placa: ");
    scanf("%s", b.placa);

    printf("Qual garagem: ");
    scanf("%s", b.garagem);

    printf("Nome do Motorista: ");
    scanf("%s", b.motorista);

    printf("Turno: ");
    scanf("%s", b.turno);

    fprintf(relatoriobrt, "%d %d %s %s %s %s\n",
            b.linha, b.numero_onibus, b.placa,
            b.garagem, b.motorista, b.turno);

    fclose(relatoriobrt);

    printf("Cadastro realizado com sucesso!\n");
}

int main(void){
    cadastro();
    return 0;
}