#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ═══════════════════════════════════════
//              CONSTANTES
// ═══════════════════════════════════════

#define MAX_AVISOS 50
#define TAM_STR    128
#define TAM_DESC   512

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

struct Aviso
{
    char protocolo[32];
    char hora[32];
    char tipo[TAM_STR];
    char gravidade[16];
    char local[TAM_STR];
    char descricao[TAM_DESC];
};

struct Motorista
{
    char matricula[32];
    char nome[TAM_STR];
    char linha[TAM_STR];
    char veiculo[TAM_STR];
    char hora_inicio[32];
    int  logado;
    struct Aviso avisos[MAX_AVISOS];
    int  total_avisos;
};

struct Motorista motorista;

// ═══════════════════════════════════════
//     FUNÇÕES OBRIGATÓRIAS DO PROJETO
// ═══════════════════════════════════════

int verificar_onibus(int numero)
{
    FILE *f = fopen("relatorio_brt.txt", "r");
    if (f == NULL)
        return 0;

    struct BRT_onibus b;

    while (fscanf(f, "%d %d %s %s %s %s",
                  &b.linha, &b.numero_onibus,
                  b.placa, b.garagem,
                  b.motorista, b.turno) != EOF)
    {
        if (b.numero_onibus == numero)
        {
            fclose(f);
            return 1;
        }
    }

    fclose(f);
    return 0;
}

int verificar_motorista(char cpf[])
{
    FILE *f = fopen("relatorio_motorista.txt", "r");
    if (f == NULL)
        return 0;

    char linha[200];
    char nome[50], cpf_lido[14], telefone[15];
    int linha_atuacao;

    while (fgets(linha, sizeof(linha), f))
    {
        sscanf(linha, "%49[^;];%13[^;];%14[^;];%d",
               nome, cpf_lido, telefone, &linha_atuacao);

        if (strcmp(cpf_lido, cpf) == 0)
        {
            fclose(f);
            return 1;
        }
    }

    fclose(f);
    return 0;
}

int validar_linha(int linha)
{
    FILE *f = fopen("relatorio_linhas.txt", "r");
    if (f == NULL)
        return 0;

    int codigo;

    while (fscanf(f, "%d", &codigo) != EOF)
    {
        if (codigo == linha)
        {
            fclose(f);
            return 1;
        }

        while (fgetc(f) != '\n' && !feof(f));
    }

    fclose(f);
    return 0;
}

int associar_motorista(int onibus, char cpf[])
{
    if (!verificar_onibus(onibus))
        return 0;

    if (!verificar_motorista(cpf))
        return 0;

    return 1;
}

// ═══════════════════════════════════════
//              FUNCOES BASE
// ═══════════════════════════════════════

void obter_hora(char *buf, int tam)
{
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buf, tam, "%d/%m/%Y %H:%M:%S", tm_info);
}

void gerar_protocolo(char *buf)
{
    time_t t = time(NULL);
    srand((unsigned int)t);
    snprintf(buf, 32, "AVS-%04d%02d", rand() % 9999, rand() % 99);
}

// ═══════════════════════════════════════
//              LOGIN
// ═══════════════════════════════════════

void login()
{
    char cpf[14];
    int onibus, linha;

    printf("\n---Login do Motorista---\n");

    printf("CPF: ");
    scanf("%s", cpf);

    if (!verificar_motorista(cpf))
    {
        printf("Motorista nao cadastrado!\n");
        return;
    }

    printf("Numero do onibus: ");
    scanf("%d", &onibus);

    if (!verificar_onibus(onibus))
    {
        printf("Onibus nao cadastrado!\n");
        return;
    }

    printf("Linha: ");
    scanf("%d", &linha);

    if (!validar_linha(linha))
    {
        printf("Linha invalida!\n");
        return;
    }

    if (!associar_motorista(onibus, cpf))
    {
        printf("Erro na associacao!\n");
        return;
    }

    strcpy(motorista.matricula, cpf);
    sprintf(motorista.nome, "Motorista CPF %s", cpf);
    sprintf(motorista.linha, "%d", linha);
    sprintf(motorista.veiculo, "%d", onibus);

    obter_hora(motorista.hora_inicio, sizeof(motorista.hora_inicio));

    motorista.logado = 1;
    motorista.total_avisos = 0;

    printf("Login realizado com sucesso!\n");
}

// ═══════════════════════════════════════
//              ENVIAR AVISO
// ═══════════════════════════════════════

void enviar_aviso()
{
    if (!motorista.logado)
    {
        printf("Voce precisa fazer login primeiro!\n");
        return;
    }

    const char *tipos[] = {
        "Pane mecanica", "Pane eletrica", "Acidente", "Pneu furado",
        "Transito intenso", "Bloqueio na via", "Obra na pista",
        "Alagamento", "Manifestacao", "Superlotacao", "Outros"
    };

    int tipo, grav;
    char local[TAM_STR], desc[TAM_DESC];

    printf("\nTipo de ocorrencia (1-11): ");
    scanf("%d", &tipo);

    printf("Gravidade (1-BAIXA / 2-MEDIA / 3-ALTA): ");
    scanf("%d", &grav);

    printf("Local: ");
    scanf(" %127[^\n]", local);

    printf("Descricao: ");
    scanf(" %511[^\n]", desc);

    struct Aviso *a = &motorista.avisos[motorista.total_avisos];

    gerar_protocolo(a->protocolo);
    obter_hora(a->hora, sizeof(a->hora));

    strcpy(a->tipo, tipos[tipo - 1]);
    if (grav == 1) strcpy(a->gravidade, "BAIXA");
    if (grav == 2) strcpy(a->gravidade, "MEDIA");
    if (grav == 3) strcpy(a->gravidade, "ALTA");

    strcpy(a->local, local);
    strcpy(a->descricao, desc);

    motorista.total_avisos++;

    char nome_arq[64];
    sprintf(nome_arq, "avisos_%s.txt", motorista.matricula);

    FILE *f = fopen(nome_arq, "a");

    fprintf(f, "%s;%s;%s;%s;%s;%s\n",
            a->protocolo,
            a->hora,
            a->tipo,
            a->gravidade,
            a->local,
            a->descricao);

    fclose(f);

    printf("Aviso enviado com sucesso!\n");
}

// ═══════════════════════════════════════
//              MENU
// ═══════════════════════════════════════

int main()
{
    int op;

    memset(&motorista, 0, sizeof(motorista));

    do
    {
        printf("\n===========================\n");
        printf("      SISBUS - MOTORISTA\n");
        printf("===========================\n");

        if (!motorista.logado)
        {
            printf("1 - Login\n");
            printf("0 - Sair\n");
        }
        else
        {
            printf("Motorista: %s\n", motorista.nome);
            printf("Linha: %s | Onibus: %s\n", motorista.linha, motorista.veiculo);
            printf("---------------------------\n");
            printf("1 - Enviar Aviso\n");
            printf("2 - Logoff\n");
            printf("0 - Sair\n");
        }

        printf("Escolha: ");
        scanf("%d", &op);

        if (!motorista.logado)
        {
            if (op == 1) login();
        }
        else
        {
            if (op == 1) enviar_aviso();
            if (op == 2) motorista.logado = 0;
        }

    } while (op != 0);

    return 0;
}