#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

int main (void){
//variaveis
    int linha,numero_onibus;
    char placa[10];
    char garagem[50];
    char motorista[50];
    char turno[10];
    char horario_saida[6];
    char horario_chegada[6];
    char status[20];
    char observacoes[100];
//recebendo dados cadastro

void cadastro(){
    FILE *relatoriobrt = fopen("relatoriobrt", "a");
    struct BRT b;
    if (relatoriobrt == NULL){
        printf("Erro na criação do arquivo!\n");
        return;
    }
    printf("---Cadastro BRT---");
    printf("Linha: ");
    scanf("%d", &b.linha);
    printf("Número do onibus: ");
    scanf("%d", &b.numero_onibus);




}






    return 0;
}