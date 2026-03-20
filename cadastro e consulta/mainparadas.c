#include <stdio.h> // Biblioteca para entrada e saída de dados
#include <stdlib.h> // Biblioteca para funções de alocação de memória e outros
#include <locale.h> // Biblioteca para configuração de localização e deixar acentos

struct BRT {
    int codigo;
    char endereco[100];
    float latitude;
    float longitude;
};
void paradas(){
    // Abre o arquivo em modo de escrita
    // "a" = append (adiciona ao final do arquivo)
    //a primeira parte do fopen indica onde o arquivo será criado
    FILE *relatorioparadas = fopen("relatorioparadas.txt", "a");
    struct BRT b;

    // Verifica se o arquivo foi aberto corretamente
    if (relatorioparadas == NULL){
        printf("Erro na criação do arquivo!\n");
        return;
    }
    printf("---Cadastro PARADAS---\n");
    printf("Código: ");
    scanf("%d", &b.codigo);
    printf("Endereço: ");
    // %99[^\n] lê até 99 caracteres ou até encontrar \n
    scanf(" %99[^\n]", b.endereco);
    printf("Latitude: ");
    scanf(" %f", &b.latitude);
    printf("Longitude: ");
    scanf(" %f", &b.longitude);
        
    //fprintf escreve no arquivo
    fprintf(relatorioparadas, "%d %s %f %f\n",
            b.codigo, b.endereco, b.latitude, b.longitude);

    // Fecha o arquivo
    fclose(relatorioparadas);
    printf("Cadastro realizado com sucesso!\n");
}

int main(void){
    paradas();
    return 0;
}