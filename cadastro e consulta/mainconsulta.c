#include <stdio.h> // Biblioteca para entrada e saída de dados
#include <stdlib.h> // Biblioteca para funções de alocação de memória e outros
#include <locale.h> // Biblioteca para configuração de localização e deixar acentos

// Função que lê e exibe os dados do arquivo de paradas
void exibirParadas() {
    FILE *relatorioparadas = fopen("relatorioparadas.txt", "r"); // Abre em modo leitura
    int codigo;
    char endereco[100];
    float latitude, longitude;

    // Verifica se o arquivo foi aberto corretamente
    if (relatorioparadas == NULL) {
        printf("Erro ao abrir relatorioparadas.txt!\n");
        return;
    }

    printf("===== PARADAS CADASTRADAS =====\n\n");

    // Lê registro por registro até o fim do arquivo
    while (fscanf(relatorioparadas, "%d ", &codigo) == 1) {

        // Lê o endereço caractere por caractere até encontrar um número
        int i = 0;
        char c;
        while ((c = fgetc(relatorioparadas)) != EOF) {
            // Para quando encontrar um dígito ou sinal de menos
            if ((c >= '0' && c <= '9') || c == '-') {
                ungetc(c, relatorioparadas); // devolve o char pro arquivo
                break;
            }
            endereco[i++] = c;
        }
        endereco[i - 1] = '\0'; // remove o espaço do final

        // Lê latitude e longitude
        fscanf(relatorioparadas, "%f %f\n", &latitude, &longitude);

        printf("Código   : %d\n", codigo);
        printf("Endereço : %s\n", endereco);
        printf("Latitude : %.6f\n", latitude);
        printf("Longitude: %.6f\n", longitude);
        printf("-------------------------------\n");
    }

    fclose(relatorioparadas); // Fecha o arquivo
}

// Função que lê e exibe os dados do arquivo de rotas
void exibirRotas() {
    FILE *relatoriorotas = fopen("relatoriorotas.txt", "r"); // Abre em modo leitura
    char linha[300]; // Buffer para armazenar cada linha lida

    // Verifica se o arquivo foi aberto corretamente
    if (relatoriorotas == NULL) {
        printf("Erro ao abrir relatoriorotas.txt!\n");
        return;
    }

    printf("\n===== ROTAS CADASTRADAS =====\n\n");

    // Lê linha por linha até o fim do arquivo
    while (fgets(linha, sizeof(linha), relatoriorotas) != NULL) {
        int codigo_rota;

        // Extrai o código da rota
        sscanf(linha, "%d", &codigo_rota);
        printf("Rota: %d  |  Paradas: ", codigo_rota);

        // Pula o primeiro número e exibe o resto da linha (as paradas)
        int i = 0;
        while (linha[i] != ' ' && linha[i] != '\0') i++; // pula o código da rota
        printf("%s", linha + i + 1); // imprime o restante da linha
    }

    printf("==============================\n");
    fclose(relatoriorotas); // Fecha o arquivo
}

int main(void) {
    // Define o idioma para português (permite acentos no terminal)
    setlocale(LC_ALL, "Portuguese");

    // Exibe os dados dos dois arquivos
    exibirParadas();
    exibirRotas();

    return 0;
}
