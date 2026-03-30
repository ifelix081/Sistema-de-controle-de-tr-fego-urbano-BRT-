#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

// ═══════════════════════════════════════
//              STRUCTS
// ═══════════════════════════════════════

struct BRT_onibus
{
    int  linha;
    int  numero_onibus;
    char placa[8];
    char garagem[50];
    char motorista[50];
    char turno[20];
};

struct BRT_paradas
{
    int  codigo_parada;
    char endereco[100];
};

struct BRT_linhas
{
    int codigo_linha;
    int codigo_parada;
};

struct BRT_motorista
{
    char nome[50];
    char cpf[14];
    char telefone[15];
    int  linha_atuacao;
};

// ═══════════════════════════════════════
//         FUNÇÕES DE VALIDAÇÃO
// ═══════════════════════════════════════

int campo_vazio(char texto[])
{
    return strlen(texto) == 0;
}

int placa_existe(char placa[])
{
    FILE *relabrt = fopen("relatorio_brt.txt", "r");
    if (relabrt == NULL)
        return 0;

    struct BRT_onibus brt;

    while (fscanf(relabrt, "%d %d %s %s %s %s",
                  &brt.linha, &brt.numero_onibus,
                  brt.placa, brt.garagem,
                  brt.motorista, brt.turno) != EOF)
    {
        if (strcmp(brt.placa, placa) == 0)
        {
            fclose(relabrt);
            return 1;
        }
    }

    fclose(relabrt);
    return 0;
}

int parada_existe(int codigo)
{
    FILE *relaparada = fopen("relatorio_paradas.txt", "r");
    if (relaparada == NULL)
        return 0;

    struct BRT_paradas parada;

    while (fscanf(relaparada, "%d %99[^\n]",
                  &parada.codigo_parada,
                  parada.endereco) != EOF)
    {
        if (parada.codigo_parada == codigo)
        {
            fclose(relaparada);
            return 1;
        }
    }

    fclose(relaparada);
    return 0;
}

int cpf_existe(char cpf[])
{
    FILE *relamotorista = fopen("relatorio_motorista.txt", "r");
    if (relamotorista == NULL)
        return 0;

    char linha[200];
    char nome[50], cpf_lido[14], telefone[15];
    int linha_atuacao;

    while (fgets(linha, sizeof(linha), relamotorista))
    {
        sscanf(linha, "%49[^;];%13[^;];%14[^;];%d",
               nome, cpf_lido, telefone, &linha_atuacao);

        if (strcmp(cpf_lido, cpf) == 0)
        {
            fclose(relamotorista);
            return 1;
        }
    }

    fclose(relamotorista);
    return 0;
}

// ═══════════════════════════════════════
//              CADASTRO ONIBUS
// ═══════════════════════════════════════

void cadastro()
{
    struct BRT_onibus brt;

    printf("\n---Cadastro de BRT---\n");

    printf("Digite o numero da linha: ");
    scanf("%d", &brt.linha);

    printf("Digite o numero do onibus: ");
    scanf("%d", &brt.numero_onibus);

    printf("Digite a placa: ");
    scanf("%s", brt.placa);

    if (campo_vazio(brt.placa))
    {
        printf("Placa nao pode ser vazia!\n");
        return;
    }

    if (placa_existe(brt.placa))
    {
        printf("Essa placa ja esta cadastrada!\n");
        return;
    }

    printf("Digite a garagem: ");
    scanf("%s", brt.garagem);

    printf("Digite o motorista: ");
    scanf("%s", brt.motorista);

    printf("Digite o turno: ");
    scanf("%s", brt.turno);

    FILE *relabrt = fopen("relatorio_brt.txt", "a");

    fprintf(relabrt, "%d %d %s %s %s %s\n",
            brt.linha, brt.numero_onibus, brt.placa,
            brt.garagem, brt.motorista, brt.turno);

    fclose(relabrt);

    printf("Cadastro realizado com sucesso!\n");
}

// ═══════════════════════════════════════
//              CADASTRO PARADAS
// ═══════════════════════════════════════

void cadastro_paradas()
{
    struct BRT_paradas parada;

    printf("\n---Cadastro de Paradas---\n");

    printf("Digite o codigo da parada: ");
    scanf("%d", &parada.codigo_parada);

    if (parada_existe(parada.codigo_parada))
    {
        printf("Esse codigo ja existe!\n");
        return;
    }

    printf("Digite o endereco: ");
    scanf(" %99[^\n]", parada.endereco);

    if (campo_vazio(parada.endereco))
    {
        printf("Endereco nao pode ser vazio!\n");
        return;
    }

    FILE *relaparada = fopen("relatorio_paradas.txt", "a");

    fprintf(relaparada, "%d %s\n",
            parada.codigo_parada,
            parada.endereco);

    fclose(relaparada);

    printf("Cadastro realizado com sucesso!\n");
}

// ═══════════════════════════════════════
//          CADASTRO MOTORISTA
// ═══════════════════════════════════════

void cadastro_motorista()
{
    struct BRT_motorista motorista;

    printf("\n---Cadastro de Motorista---\n");

    printf("Digite o nome: ");
    scanf(" %49[^\n]", motorista.nome);

    if (campo_vazio(motorista.nome))
    {
        printf("Nome nao pode ser vazio!\n");
        return;
    }

    printf("Digite o CPF: ");
    scanf("%s", motorista.cpf);

    if (campo_vazio(motorista.cpf))
    {
        printf("CPF nao pode ser vazio!\n");
        return;
    }

    if (cpf_existe(motorista.cpf))
    {
        printf("CPF ja cadastrado!\n");
        return;
    }

    printf("Digite o telefone: ");
    scanf("%s", motorista.telefone);

    printf("Digite a linha de atuacao: ");
    scanf("%d", &motorista.linha_atuacao);

    FILE *relamotorista = fopen("relatorio_motorista.txt", "a");

    // AGORA SALVA COM ; (formato seguro)
    fprintf(relamotorista, "%s;%s;%s;%d\n",
            motorista.nome,
            motorista.cpf,
            motorista.telefone,
            motorista.linha_atuacao);

    fclose(relamotorista);

    printf("Cadastro realizado com sucesso!\n");
}

// ═══════════════════════════════════════
//              CADASTRO LINHAS
// ═══════════════════════════════════════

void cadastro_linhas()
{
    struct BRT_linhas linha;
    char continuar;

    FILE *relalinha = fopen("relatorio_linhas.txt", "a");

    printf("\n---Cadastro de Linhas---\n");

    printf("Digite o codigo da linha: ");
    scanf("%d", &linha.codigo_linha);

    fprintf(relalinha, "%d", linha.codigo_linha);

    do
    {
        printf("Digite o codigo da parada: ");
        scanf("%d", &linha.codigo_parada);

        fprintf(relalinha, " %d", linha.codigo_parada);

        printf("Deseja cadastrar mais uma parada? (s/n): ");
        scanf(" %c", &continuar);

    } while (continuar == 's' || continuar == 'S');

    fprintf(relalinha, "\n");

    fclose(relalinha);

    printf("Cadastro de linhas finalizado!\n");
}

// ═══════════════════════════════════════
//              RELATORIO
// ═══════════════════════════════════════

void relatorio_geral()
{
    char linha[200];

    printf("\n--- ONIBUS ---\n");
    FILE *f = fopen("relatorio_brt.txt", "r");
    if (f != NULL)
    {
        while (fgets(linha, sizeof(linha), f))
            printf("%s", linha);
        fclose(f);
    }

    printf("\n--- PARADAS ---\n");
    f = fopen("relatorio_paradas.txt", "r");
    if (f != NULL)
    {
        while (fgets(linha, sizeof(linha), f))
            printf("%s", linha);
        fclose(f);
    }

    printf("\n--- MOTORISTAS ---\n");
    f = fopen("relatorio_motorista.txt", "r");
    if (f != NULL)
    {
        while (fgets(linha, sizeof(linha), f))
            printf("%s", linha);
        fclose(f);
    }

    printf("\n--- LINHAS ---\n");
    f = fopen("relatorio_linhas.txt", "r");
    if (f != NULL)
    {
        while (fgets(linha, sizeof(linha), f))
            printf("%s", linha);
        fclose(f);
    }
}

// ═══════════════════════════════════════
//              MAIN
// ═══════════════════════════════════════

int main()
{
    int opcao;

    do
    {
        printf("\n============================\n");
        printf("        SISTEMA BRT\n");
        printf("============================\n");
        printf("1 - Cadastrar Onibus\n");
        printf("2 - Cadastrar Parada\n");
        printf("3 - Cadastrar Motorista\n");
        printf("4 - Cadastrar Linha\n");
        printf("5 - Relatorio Geral\n");
        printf("0 - Sair\n");
        printf("============================\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        switch (opcao)
        {
            case 1: cadastro(); break;
            case 2: cadastro_paradas(); break;
            case 3: cadastro_motorista(); break;
            case 4: cadastro_linhas(); break;
            case 5: relatorio_geral(); break;
        }

    } while (opcao != 0);

    return 0;
}