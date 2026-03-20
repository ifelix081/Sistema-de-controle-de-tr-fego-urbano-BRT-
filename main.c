#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <windows.h>

// ═══════════════════════════════════════
//              STRUCTS
// ═══════════════════════════════════════

// Struct que guarda os dados de um onibus
struct BRT_onibus
{
    int  linha;           // numero da linha que o onibus pertence
    int  numero_onibus;   // numero de identificacao do onibus
    char placa[8];        // placa do onibus (ex: ABC-1234)
    char garagem[50];     // nome da garagem onde o onibus fica
    char motorista[50];   // nome do motorista responsavel
    char turno[20];       // turno de trabalho (manha, tarde, noite)
};

// Struct que guarda os dados de uma parada
struct BRT_paradas
{
    int  codigo_parada;   // numero de identificacao da parada
    char endereco[100];   // endereco completo da parada
};

// Struct que representa a relacao entre linha e parada
struct BRT_linhas
{
    int codigo_linha;     // codigo da linha
    int codigo_parada;    // codigo de uma parada pertencente a essa linha
};

// Struct que guarda os dados de um motorista
struct BRT_motorista
{
    char nome[50];        // nome completo do motorista
    char cpf[14];         // CPF no formato 000.000.000-00
    char telefone[15];    // telefone de contato
    int  linha_atuacao;   // numero da linha em que o motorista trabalha
};

// ═══════════════════════════════════════
//              FUNCOES
// ═══════════════════════════════════════

// Cadastra um novo onibus e salva no arquivo relatorio_brt.txt
void cadastro()
{
    struct BRT_onibus brt;

    // Abre o arquivo em modo "append" (adiciona sem apagar o que ja tem)
    FILE *relabrt = fopen("relatorio_brt.txt", "a");
    if (relabrt == NULL)
    {
        printf("Erro ao abrir o arquivo\n");
        return;
    }

    // Lê os dados do onibus digitados pelo usuario
    printf("---Cadastro de BRT---\n");
    printf("Digite o numero da linha: ");
    scanf("%d", &brt.linha);
    printf("Digite o numero do onibus: ");
    scanf("%d", &brt.numero_onibus);
    printf("Digite a placa: ");
    scanf("%s", brt.placa);
    printf("Digite a garagem: ");
    scanf("%s", brt.garagem);
    printf("Digite o motorista: ");
    scanf("%s", brt.motorista);
    printf("Digite o turno: ");
    scanf("%s", brt.turno);

    // Salva todos os dados em uma linha no arquivo
    fprintf(relabrt, "%d %d %s %s %s %s\n",
            brt.linha, brt.numero_onibus, brt.placa,
            brt.garagem, brt.motorista, brt.turno);

    fclose(relabrt);
    printf("Cadastro realizado com sucesso!\n");
}

// Cadastra uma nova parada e salva no arquivo relatorio_paradas.txt
void cadastro_paradas()
{
    struct BRT_paradas parada;

    FILE *relaparada = fopen("relatorio_paradas.txt", "a");
    if (relaparada == NULL)
    {
        printf("Erro ao abrir o arquivo\n");
        return;
    }

    // Lê o codigo e o endereco da parada
    printf("---Cadastro de Paradas---\n");
    printf("Digite o codigo da parada: ");
    scanf("%d", &parada.codigo_parada);
    printf("Digite o endereco: ");
    scanf(" %99[^\n]", parada.endereco); // lê a linha inteira, incluindo espacos

    fprintf(relaparada, "%d %s\n", parada.codigo_parada, parada.endereco);

    fclose(relaparada);
    printf("Cadastro realizado com sucesso!\n");
}

// Cadastra um novo motorista e salva no arquivo relatorio_motorista.txt
void cadastro_motorista()
{
    struct BRT_motorista motorista;

    FILE *relamotorista = fopen("relatorio_motorista.txt", "a");
    if (relamotorista == NULL)
    {
        printf("Erro ao abrir o arquivo\n");
        return;
    }

    // Lê os dados pessoais e profissionais do motorista
    printf("---Cadastro de Motorista---\n");
    printf("Digite o nome: ");
    scanf(" %49[^\n]", motorista.nome);    // lê nome com espacos
    printf("Digite o CPF: ");
    scanf(" %13s", motorista.cpf);
    printf("Digite o telefone: ");
    scanf(" %14s", motorista.telefone);
    printf("Digite a linha de atuacao: ");
    scanf("%d", &motorista.linha_atuacao);

    fprintf(relamotorista, "%s %s %s %d\n",
            motorista.nome, motorista.cpf,
            motorista.telefone, motorista.linha_atuacao);

    fclose(relamotorista);
    printf("Cadastro realizado com sucesso!\n");
}

// Cadastra uma linha com suas paradas e salva no arquivo relatorio_linhas.txt
void cadastro_linhas()
{
    struct BRT_linhas linha;
    char continuar;

    FILE *relalinha = fopen("relatorio_linhas.txt", "a");
    if (relalinha == NULL)
    {
        printf("Erro ao abrir o arquivo\n");
        return;
    }

    printf("---Cadastro de Linhas---\n");
    printf("Digite o codigo da linha: ");
    scanf("%d", &linha.codigo_linha);
    fprintf(relalinha, "%d", linha.codigo_linha); // salva o codigo da linha

    // Permite cadastrar varias paradas para a mesma linha
    do
    {
        printf("Digite o codigo da parada: ");
        scanf("%d", &linha.codigo_parada);
        fprintf(relalinha, " %d", linha.codigo_parada); // adiciona a parada na mesma linha do arquivo
        printf("Parada cadastrada com sucesso!\n");
        printf("Deseja cadastrar mais uma parada? (s/n): ");
        scanf(" %c", &continuar);
    } while (continuar == 's' || continuar == 'S'); // continua enquanto usuario digitar 's'

    fprintf(relalinha, "\n");
    fclose(relalinha);
    printf("Cadastro de linhas finalizado!\n");
}

// Exibe todos os dados cadastrados lendo diretamente dos arquivos
void relatorio_geral()
{
    char linha[200]; // buffer para guardar cada linha lida dos arquivos

    // Abre e imprime o arquivo de onibus
    printf("\n--- Onibus Cadastrados ---\n");
    FILE *relabrt = fopen("relatorio_brt.txt", "r");
    if (relabrt == NULL)
        printf("Nenhum onibus cadastrado.\n");
    else
    {
        while (fgets(linha, sizeof(linha), relabrt)) // le linha por linha
            printf("%s", linha);
        fclose(relabrt);
    }

    // Abre e imprime o arquivo de paradas
    printf("\n--- Paradas Cadastradas ---\n");
    FILE *relaparada = fopen("relatorio_paradas.txt", "r");
    if (relaparada == NULL)
        printf("Nenhuma parada cadastrada.\n");
    else
    {
        while (fgets(linha, sizeof(linha), relaparada))
            printf("%s", linha);
        fclose(relaparada);
    }

    // Abre e imprime o arquivo de motoristas
    printf("\n--- Motoristas Cadastrados ---\n");
    FILE *relamotorista = fopen("relatorio_motorista.txt", "r");
    if (relamotorista == NULL)
        printf("Nenhum motorista cadastrado.\n");
    else
    {
        while (fgets(linha, sizeof(linha), relamotorista))
            printf("%s", linha);
        fclose(relamotorista);
    }

    // Abre e imprime o arquivo de linhas
    printf("\n--- Linhas Cadastradas ---\n");
    FILE *relalinha = fopen("relatorio_linhas.txt", "r");
    if (relalinha == NULL)
        printf("Nenhuma linha cadastrada.\n");
    else
    {
        while (fgets(linha, sizeof(linha), relalinha))
            printf("%s", linha);
        fclose(relalinha);
    }

    printf("\n===============================\n");
}

// ═══════════════════════════════════════
//              MAIN
// ═══════════════════════════════════════

int main()
{
    int opcao; // guarda a opcao digitada pelo usuario

    // Loop principal: fica rodando ate o usuario escolher sair (opcao 0)
    do
    {
        // Exibe o menu de opcoes
        printf("\n===================================\n");
        printf("        SISTEMA BRT\n");
        printf("===================================\n");
        printf("1 - Cadastrar Onibus\n");
        printf("2 - Cadastrar Parada\n");
        printf("3 - Cadastrar Motorista\n");
        printf("4 - Cadastrar Linha\n");
        printf("5 - Relatorio Geral\n");
        printf("0 - Sair\n");
        printf("===================================\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        // Chama a funcao correspondente a opcao escolhida
        switch (opcao)
        {
            case 1: cadastro();            break;
            case 2: cadastro_paradas();    break;
            case 3: cadastro_motorista();  break;
            case 4: cadastro_linhas();     break;
            case 5: relatorio_geral();     break;
            case 0: printf("Saindo...\n"); break;
            default: printf("Opcao invalida!\n"); // caso o usuario digite algo fora do menu
        }

    } while (opcao != 0);

    return 0;
}