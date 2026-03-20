#include <stdio.h> // Biblioteca para entrada e saída de dados
#include <stdlib.h> // Biblioteca para funções de alocação de memória e outros
#include <locale.h> // Biblioteca para configuração de localização e deixar acentos

//esse codigo foi chatinhoe ein, pqp

// Struct que armazena os dados de uma rota e suas paradas
struct rotas {
    int codigo_rota;  // Código identificador da rota
    int codigo_parada; // Código identificador da parada
};
void rotas() {
    // Abre o arquivo em modo append (adiciona ao final sem apagar o que já tem)
    FILE *relatoriorotas = fopen("relatoriorotas.txt", "a");
    struct rotas r; // Variável do tipo struct rotas para armazenar os dados
    char continuar; // Variável para controlar se o usuário quer continuar cadastrando

    // Verifica se o arquivo foi aberto corretamente
    if (relatoriorotas == NULL) {
        printf("Erro na criação do arquivo!\n");
        return;
    }
    // Lê o código da rota uma única vez
    printf("Código da rota: ");
    scanf("%d", &r.codigo_rota);

    // Escreve o código da rota no arquivo uma única vez
    fprintf(relatoriorotas, "%d ", r.codigo_rota);

    // Loop que continua cadastrando paradas até o usuário digitar 'n'
    do {
        // Lê o código da parada
        printf("Código da parada: ");
        scanf("%d", &r.codigo_parada);

        // Escreve o código da parada na mesma linha do arquivo
        fprintf(relatoriorotas, "%d ", r.codigo_parada);

        printf("Cadastro realizado com sucesso!\n");

        // Pergunta se deseja cadastrar mais uma parada
        printf("Adicionar outra parada? (s/n): ");
        scanf(" %c", &continuar);

    } while (continuar == 's'); // Continua enquanto o usuário digitar 's'

    // Pula linha no arquivo ao encerrar o cadastro da rota
    fprintf(relatoriorotas, "\n");

    // Fecha o arquivo
    fclose(relatoriorotas);
    printf("Encerrando cadastro.\n");
}

int main(void) {
    // Define o idioma para português (permite acentos no terminal)
    setlocale(LC_ALL, "Portuguese");

    // Chama a função de cadastro de rotas
    rotas();

    return 0;
}
