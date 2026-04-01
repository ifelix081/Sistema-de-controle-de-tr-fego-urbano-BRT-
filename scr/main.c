#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LIMPAR system("cls || clear")
#define PAUSAR system("timeout /t 3 >nul 2>&1 || sleep 3")

// ═══════════════════════════════════════
//              CONSTANTES
// ═══════════════════════════════════════

#define MAX_AVISOS 50
#define TAM_STR    128
#define TAM_DESC   512

#define LINHA "=============================================="
#define DIV   "----------------------------------------------"

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

struct Status
{
    int  onibus;
    int  motorista;
    int  rota;
    char chegada[10];
    char saida[10];
    char situacao[50];
};

struct Motorista motorista;

// ═══════════════════════════════════════
//         FUNÇÕES AUXILIARES
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

void cabecalho(const char *titulo)
{
    LIMPAR;
    printf("%s\n", LINHA);
    printf("       SISBUS - CONTROLE BRT\n");
    printf("%s\n", LINHA);
    printf(" %s\n", titulo);
    printf("%s\n\n", DIV);
}

void pausar()
{
    printf("\nPressione ENTER para continuar...");
    getchar();
    getchar();
}

// ═══════════════════════════════════════
//     FUNÇÕES DE VERIFICAÇÃO
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
//              LOGIN
// ═══════════════════════════════════════

void login()
{
    char cpf[14];
    int onibus, linha;

    cabecalho("LOGIN DO MOTORISTA");

    printf("CPF: ");
    scanf("%s", cpf);

    if (!verificar_motorista(cpf))
    {
        printf("\nMotorista nao cadastrado no sistema!\n");
        pausar();
        return;
    }

    printf("Numero do onibus: ");
    scanf("%d", &onibus);

    if (!verificar_onibus(onibus))
    {
        printf("\nOnibus nao cadastrado no sistema!\n");
        pausar();
        return;
    }

    printf("Linha: ");
    scanf("%d", &linha);

    if (!validar_linha(linha))
    {
        printf("\nLinha invalida!\n");
        pausar();
        return;
    }

    if (!associar_motorista(onibus, cpf))
    {
        printf("\nErro na associacao motorista/onibus!\n");
        pausar();
        return;
    }

    strcpy(motorista.matricula, cpf);
    sprintf(motorista.nome, "Motorista CPF %s", cpf);
    sprintf(motorista.linha, "%d", linha);
    sprintf(motorista.veiculo, "%d", onibus);

    obter_hora(motorista.hora_inicio, sizeof(motorista.hora_inicio));

    motorista.logado = 1;
    motorista.total_avisos = 0;

    printf("\nLogin realizado com sucesso!\n");
    printf("Inicio da jornada: %s\n", motorista.hora_inicio);
    pausar();
}

// ═══════════════════════════════════════
//         ENVIAR AVISO (POR MENUS)
// ═══════════════════════════════════════

void enviar_aviso()
{
    if (!motorista.logado)
    {
        printf("\nVoce precisa fazer login primeiro!\n");
        pausar();
        return;
    }

    struct Aviso *a = &motorista.avisos[motorista.total_avisos];

    cabecalho("ENVIAR AVISO");

    gerar_protocolo(a->protocolo);
    obter_hora(a->hora, sizeof(a->hora));

    // --- TIPO DO OCORRIDO ---
    printf("=== TIPO DO OCORRIDO ===\n");
    printf(" 1 - Acidente de transito\n");
    printf(" 2 - Falha mecanica\n");
    printf(" 3 - Passageiro com mal estar\n");
    printf(" 4 - Vandalismo / depredacao\n");
    printf(" 5 - Problema na via\n");
    printf(" 6 - Conflito com passageiro\n");
    printf(" 7 - Atraso operacional\n");
    printf(" 8 - Superlotacao\n");
    printf(" 9 - Alagamento\n");
    printf("10 - Obra na pista\n");
    printf("11 - Outro\n");
    printf("Escolha: ");

    int op_tipo;
    scanf("%d", &op_tipo);

    const char *tipos[] = {
        "Acidente de transito", "Falha mecanica",
        "Passageiro com mal estar", "Vandalismo / depredacao",
        "Problema na via", "Conflito com passageiro",
        "Atraso operacional", "Superlotacao",
        "Alagamento", "Obra na pista", "Outro"
    };

    if (op_tipo >= 1 && op_tipo <= 11)
        strcpy(a->tipo, tipos[op_tipo - 1]);
    else
        strcpy(a->tipo, "Outro");

    // --- GRAVIDADE ---
    printf("\n=== GRAVIDADE ===\n");
    printf("1 - BAIXA  (sem feridos, sem bloqueio)\n");
    printf("2 - MEDIA  (interferencia no transito)\n");
    printf("3 - ALTA   (feridos ou via bloqueada)\n");
    printf("Escolha: ");

    int op_grav;
    scanf("%d", &op_grav);

    if      (op_grav == 1) strcpy(a->gravidade, "BAIXA");
    else if (op_grav == 2) strcpy(a->gravidade, "MEDIA");
    else if (op_grav == 3) strcpy(a->gravidade, "ALTA");
    else                   strcpy(a->gravidade, "BAIXA");

    // --- LOCAL ---
    printf("\n=== LOCAL DO OCORRIDO ===\n");
    printf("1 - Dentro do veiculo\n");
    printf("2 - Na parada / estacao\n");
    printf("3 - Na via publica\n");
    printf("4 - No terminal\n");
    printf("5 - Na garagem\n");
    printf("6 - Outro ponto da rota\n");
    printf("Escolha: ");

    int op_local;
    scanf("%d", &op_local);

    const char *locais[] = {
        "Dentro do veiculo", "Na parada / estacao",
        "Na via publica", "No terminal",
        "Na garagem", "Outro ponto da rota"
    };

    if (op_local >= 1 && op_local <= 6)
        strcpy(a->local, locais[op_local - 1]);
    else
        strcpy(a->local, "Outro ponto da rota");

    // --- O QUE ACONTECEU ---
    printf("\n=== O QUE ACONTECEU ===\n");
    printf(" 1 - Colisao entre veiculos\n");
    printf(" 2 - Atropelamento\n");
    printf(" 3 - Pneu furado\n");
    printf(" 4 - Motor superaquecido\n");
    printf(" 5 - Porta com defeito\n");
    printf(" 6 - Passageiro desmaiou\n");
    printf(" 7 - Passageiro passou mal\n");
    printf(" 8 - Briga entre passageiros\n");
    printf(" 9 - Buraco / obstaculo na pista\n");
    printf("10 - Semaforo ou sinal com defeito\n");
    printf("11 - Alagamento na via\n");
    printf("12 - Objeto danificado no veiculo\n");
    printf("13 - Outro\n");
    printf("Escolha: ");

    int op_desc;
    scanf("%d", &op_desc);

    const char *descricoes[] = {
        "Colisao entre veiculos", "Atropelamento",
        "Pneu furado", "Motor superaquecido",
        "Porta com defeito", "Passageiro desmaiou",
        "Passageiro passou mal", "Briga entre passageiros",
        "Buraco / obstaculo na pista", "Semaforo ou sinal com defeito",
        "Alagamento na via", "Objeto danificado no veiculo", "Outro"
    };

    if (op_desc >= 1 && op_desc <= 13)
        strcpy(a->descricao, descricoes[op_desc - 1]);
    else
        strcpy(a->descricao, "Outro");

    // --- RESUMO E CONFIRMACAO ---
    printf("\n%s\n", DIV);
    printf("PROTOCOLO : %s\n", a->protocolo);
    printf("TIPO      : %s\n", a->tipo);
    printf("GRAVIDADE : %s\n", a->gravidade);
    printf("LOCAL     : %s\n", a->local);
    printf("OCORRIDO  : %s\n", a->descricao);
    printf("DATA/HORA : %s\n", a->hora);
    printf("%s\n", DIV);

    printf("Confirmar envio? (s/n): ");
    char confirmar;
    scanf(" %c", &confirmar);

    if (confirmar == 's' || confirmar == 'S')
    {
        // Salva no arquivo do motorista
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

        // Salva tambem no status.txt para o monitor ler
        FILE *fs = fopen("status.txt", "a");
        fprintf(fs, "%s %s %s %s %s %s\n",
                motorista.veiculo,
                motorista.matricula,
                motorista.linha,
                a->hora,
                a->hora,
                a->gravidade);
        fclose(fs);

        motorista.total_avisos++;
        printf("\nAviso enviado com sucesso! Protocolo: %s\n", a->protocolo);
    }
    else
    {
        printf("\nAviso cancelado.\n");
    }

    pausar();
}

// ═══════════════════════════════════════
//         MONITOR TEMPO REAL
// ═══════════════════════════════════════

void monitor_tempo_real()
{
    FILE *arquivo = fopen("status.txt", "r");
    struct Status s;

    cabecalho("MONITOR BRT - TEMPO REAL");

    if (arquivo == NULL)
    {
        printf("Nenhum dado disponivel. Aguardando avisos do sistema...\n");
        printf("\nAtualiza automaticamente a cada 3 segundos.\n");
        printf("Pressione CTRL+C para sair do monitor.\n\n");
        PAUSAR;
        return;
    }

    printf("%-8s | %-15s | %-6s | %-8s | %s\n",
           "ONIBUS", "MOTORISTA/CPF", "ROTA", "STATUS", "HORA");
    printf("%s\n", DIV);

    while (fscanf(arquivo, "%d %d %d %s %s %s",
                  &s.onibus,
                  &s.motorista,
                  &s.rota,
                  s.chegada,
                  s.saida,
                  s.situacao) != EOF)
    {
        printf("%-8d | %-15d | %-6d | %-8s | %s\n",
               s.onibus, s.motorista, s.rota, s.situacao, s.saida);
    }

    fclose(arquivo);

    printf("\n%s\n", DIV);
    printf("Atualiza automaticamente a cada 3 segundos.\n");
    printf("Pressione CTRL+C para sair do monitor.\n\n");
    PAUSAR;
}

void iniciar_monitor()
{
    printf("\nIniciando monitor em tempo real...\n");
    printf("Pressione CTRL+C para encerrar.\n");

    while (1)
    {
        monitor_tempo_real();
    }
}

// ═══════════════════════════════════════
//              MENU MOTORISTA
// ═══════════════════════════════════════

void menu_motorista()
{
    int op;

    do
    {
        cabecalho("MENU DO MOTORISTA");

        if (!motorista.logado)
        {
            printf("1 - Fazer Login\n");
            printf("2 - Monitor Tempo Real\n");
            printf("0 - Sair\n\n");
            printf("Escolha: ");
            scanf("%d", &op);

            if (op == 1) login();
            if (op == 2) iniciar_monitor();
        }
        else
        {
            printf("Motorista : %s\n", motorista.nome);
            printf("Linha     : %s\n", motorista.linha);
            printf("Veiculo   : %s\n", motorista.veiculo);
            printf("Inicio    : %s\n", motorista.hora_inicio);
            printf("Avisos    : %d\n", motorista.total_avisos);
            printf("%s\n\n", DIV);

            printf("1 - Enviar Aviso\n");
            printf("2 - Monitor Tempo Real\n");
            printf("3 - Logoff\n");
            printf("0 - Sair\n\n");
            printf("Escolha: ");
            scanf("%d", &op);

            if (op == 1) enviar_aviso();
            if (op == 2) iniciar_monitor();
            if (op == 3)
            {
                motorista.logado = 0;
                printf("\nJornada encerrada.\n");
                pausar();
            }
        }

    } while (op != 0);
}

// ═══════════════════════════════════════
//                MAIN
// ═══════════════════════════════════════

int main()
{
    memset(&motorista, 0, sizeof(motorista));
    menu_motorista();
    return 0;
}