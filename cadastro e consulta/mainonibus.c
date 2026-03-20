#include <stdio.h> // Biblioteca para entrada e saída de dados
#include <stdlib.h> // Biblioteca para funções de alocação de memória e outros
#include <locale.h> // Biblioteca para configuração de localização e deixar acentos

struct BRT {
    int linha;
    int numero_onibus;
    char placa[8];
    char garagem[1];
    char motorista[50];
    char turno[1];
};
void cadastro(){
    // Abre o arquivo em modo de escrita
    // "a" = append (adiciona ao final do arquivo)
    //a primeira parte do fopen indica onde o arquivo será criado
    FILE *relatoriobrt = fopen("relatoriobrt.txt", "a");
    struct BRT b;

    // Verifica se o arquivo foi aberto corretamente
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

    //fprintf escreve no arquivo
    fprintf(relatoriobrt, "%d %d %s %s %s %s\n",
            b.linha, b.numero_onibus, b.placa,
            b.garagem, b.motorista, b.turno);

    // Fecha o arquivo
    fclose(relatoriobrt);
    printf("Cadastro realizado com sucesso!\n");
}
int main(void){
    cadastro();
    return 0;
}
