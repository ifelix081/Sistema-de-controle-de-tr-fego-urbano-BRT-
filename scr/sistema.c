#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <time.h>

/* ======================================================
   SISTEMA BRT COMPLETO (VERSÃO FINAL PROFISSIONAL)
   - Tela inicial
   - Login Admin
   - Login Motorista
   - Cadastro de ônibus
   - Cadastro de paradas
   - Cadastro de motoristas
   - Cadastro de linhas
   - Sistema de aviso do motorista
   - Relatório geral
   ====================================================== */

#define LINHA "===================================================="
#define DIV   "----------------------------------------------------"

// ======================================================
//                    FUNÇÕES VISUAIS
// ======================================================

void limpar_tela()
{
    system("cls || clear");
}

void pausar()
{
    printf("\nPressione ENTER para continuar...");
    getchar();
    getchar();
}

void cabecalho(const char *titulo)
{
    limpar_tela();
    printf("%s\n", LINHA);
    printf("        SISTEMA DE CONTROLE BRT\n");
    printf("%s\n", LINHA);
    printf("%s\n", titulo);
    printf("%s\n\n", DIV);
}

// ======================================================
//                    STRUCTS ADMIN
// ======================================================

struct BRT_onibus
{
    int  linha;
    int  numero_onibus;
    char placa[10];
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

// ======================================================
//                    STRUCTS MOTORISTA
// ======================================================

struct Aviso
{
    char protocolo[20];
    char data_hora[25];
    char tipo[50];
    char gravidade[20];
    char local[100];
    char descricao[200];
};

struct LoginMotorista
{
    char matricula[15];
    char nome[50];
    char linha[20];
    char veiculo[20];
    char inicio_jornada[25];
    int logado;
    struct Aviso avisos[20];
    int total_avisos;
} motorista;

// ======================================================
//                    FUNÇÕES AUXILIARES
// ======================================================

void obter_data_hora(char *buffer, int tamanho)
{
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, tamanho, "%d/%m/%Y %H:%M:%S", tm_info);
}

// ======================================================
//                    LOGIN ADMIN
// ======================================================

int login_admin()
{
    char usuario[20];
    char senha[20];

    cabecalho("LOGIN ADMINISTRADOR");

    printf("Usuario: ");
    scanf("%s", usuario);

    printf("Senha: ");
    scanf("%s", senha);

    if(strcmp(usuario, "admin") == 0 && strcmp(senha, "1234") == 0)
    {
        printf("\nLogin realizado com sucesso!\n");
        pausar();
        return 1;
    }

    printf("\nUsuario ou senha incorretos!\n");
    pausar();
    return 0;
}

// ======================================================
//                    CADASTRO ÔNIBUS
// ======================================================

void cadastrar_onibus()
{
    struct BRT_onibus brt;

    cabecalho("CADASTRO DE ÔNIBUS");

    printf("Numero da linha: ");
    scanf("%d", &brt.linha);

    printf("Numero do onibus: ");
    scanf("%d", &brt.numero_onibus);

    printf("Placa: ");
    scanf("%s", brt.placa);

    printf("Garagem: ");
    scanf(" %49[^\n]", brt.garagem);

    printf("Motorista: ");
    scanf(" %49[^\n]", brt.motorista);

    printf("Turno: ");
    scanf("%s", brt.turno);

    FILE *f = fopen("relatorio_brt.txt", "a");

    fprintf(f, "%d %d %s %s %s %s\n",
            brt.linha,
            brt.numero_onibus,
            brt.placa,
            brt.garagem,
            brt.motorista,
            brt.turno);

    fclose(f);

    printf("\nÔnibus cadastrado com sucesso!\n");
    pausar();
}

// ======================================================
//                    CADASTRO PARADAS
// ======================================================

void cadastrar_parada()
{
    struct BRT_paradas p;

    cabecalho("CADASTRO DE PARADAS");

    printf("Codigo da parada: ");
    scanf("%d", &p.codigo_parada);

    printf("Endereco: ");
    scanf(" %99[^\n]", p.endereco);

    FILE *f = fopen("relatorio_paradas.txt", "a");

    fprintf(f, "%d %s\n", p.codigo_parada, p.endereco);

    fclose(f);

    printf("\nParada cadastrada com sucesso!\n");
    pausar();
}

// ======================================================
//                    CADASTRO MOTORISTA
// ======================================================

void cadastrar_motorista()
{
    struct BRT_motorista m;

    cabecalho("CADASTRO DE MOTORISTA");

    printf("Nome: ");
    scanf(" %49[^\n]", m.nome);

    printf("CPF: ");
    scanf("%s", m.cpf);

    printf("Telefone: ");
    scanf("%s", m.telefone);

    printf("Linha de atuacao: ");
    scanf("%d", &m.linha_atuacao);

    FILE *f = fopen("relatorio_motorista.txt", "a");

    fprintf(f, "%s;%s;%s;%d\n",
            m.nome,
            m.cpf,
            m.telefone,
            m.linha_atuacao);

    fclose(f);

    printf("\nMotorista cadastrado com sucesso!\n");
    pausar();
}

// ======================================================
//                    CADASTRO LINHAS
// ======================================================

void cadastrar_linhas()
{
    struct BRT_linhas linha;
    char continuar;

    cabecalho("CADASTRO DE LINHAS");

    FILE *f = fopen("relatorio_linhas.txt", "a");

    printf("Codigo da linha: ");
    scanf("%d", &linha.codigo_linha);

    fprintf(f, "%d", linha.codigo_linha);

    do
    {
        printf("Codigo da parada: ");
        scanf("%d", &linha.codigo_parada);

        fprintf(f, " %d", linha.codigo_parada);

        printf("Adicionar outra parada? (s/n): ");
        scanf(" %c", &continuar);

    } while(continuar == 's' || continuar == 'S');

    fprintf(f, "\n");
    fclose(f);

    printf("\nLinha cadastrada com sucesso!\n");
    pausar();
}

// ======================================================
//                    RELATÓRIO GERAL
// ======================================================

void relatorio_geral()
{
    char linha[200];
    FILE *f;

    cabecalho("RELATÓRIO GERAL");

    printf("\n--- ÔNIBUS ---\n");
    f = fopen("relatorio_brt.txt", "r");
    if(f)
    {
        while(fgets(linha, sizeof(linha), f))
            printf("%s", linha);
        fclose(f);
    }

    printf("\n--- PARADAS ---\n");
    f = fopen("relatorio_paradas.txt", "r");
    if(f)
    {
        while(fgets(linha, sizeof(linha), f))
            printf("%s", linha);
        fclose(f);
    }

    printf("\n--- MOTORISTAS ---\n");
    f = fopen("relatorio_motorista.txt", "r");
    if(f)
    {
        while(fgets(linha, sizeof(linha), f))
            printf("%s", linha);
        fclose(f);
    }

    printf("\n--- LINHAS ---\n");
    f = fopen("relatorio_linhas.txt", "r");
    if(f)
    {
        while(fgets(linha, sizeof(linha), f))
            printf("%s", linha);
        fclose(f);
    }

    pausar();
}

// ======================================================
//                    LOGIN MOTORISTA
// ======================================================

void login_motorista()
{
    cabecalho("LOGIN DO MOTORISTA");

    printf("Matricula: ");
    scanf("%s", motorista.matricula);

    printf("Nome: ");
    scanf(" %49[^\n]", motorista.nome);

    printf("Linha: ");
    scanf("%s", motorista.linha);

    printf("Veiculo: ");
    scanf("%s", motorista.veiculo);

    obter_data_hora(motorista.inicio_jornada, sizeof(motorista.inicio_jornada));
    motorista.total_avisos = 0;
    motorista.logado = 1;

    printf("\nLogin realizado com sucesso!\n");
    pausar();
}

// ======================================================
//                    ENVIAR AVISO (COM MENUS)
// ======================================================

void enviar_aviso()
{
    struct Aviso *a = &motorista.avisos[motorista.total_avisos];

    cabecalho("ENVIAR AVISO");

    obter_data_hora(a->data_hora, sizeof(a->data_hora));
    sprintf(a->protocolo, "AV-%04d", rand() % 9999);

    // --- TIPO DO PROBLEMA ---
    printf("=== TIPO DO OCORRIDO ===\n");
    printf("1 - Acidente de transito\n");
    printf("2 - Falha mecanica\n");
    printf("3 - Passageiro com mal estar\n");
    printf("4 - Vandalismo / depredacao\n");
    printf("5 - Problema na via\n");
    printf("6 - Conflito com passageiro\n");
    printf("7 - Atraso operacional\n");
    printf("8 - Outro\n");
    printf("Escolha: ");

    int op_tipo;
    scanf("%d", &op_tipo);

    switch(op_tipo)
    {
        case 1: strcpy(a->tipo, "Acidente de transito"); break;
        case 2: strcpy(a->tipo, "Falha mecanica"); break;
        case 3: strcpy(a->tipo, "Passageiro com mal estar"); break;
        case 4: strcpy(a->tipo, "Vandalismo / depredacao"); break;
        case 5: strcpy(a->tipo, "Problema na via"); break;
        case 6: strcpy(a->tipo, "Conflito com passageiro"); break;
        case 7: strcpy(a->tipo, "Atraso operacional"); break;
        default: strcpy(a->tipo, "Outro"); break;
    }

    // --- GRAVIDADE ---
    printf("\n=== GRAVIDADE ===\n");
    printf("1 - Leve (sem feridos, sem bloqueio)\n");
    printf("2 - Moderada (interferencia no transito)\n");
    printf("3 - Grave (feridos ou via bloqueada)\n");
    printf("4 - Critica (emergencia imediata)\n");
    printf("Escolha: ");

    int op_grav;
    scanf("%d", &op_grav);

    switch(op_grav)
    {
        case 1: strcpy(a->gravidade, "Leve"); break;
        case 2: strcpy(a->gravidade, "Moderada"); break;
        case 3: strcpy(a->gravidade, "Grave"); break;
        case 4: strcpy(a->gravidade, "Critica"); break;
        default: strcpy(a->gravidade, "Leve"); break;
    }

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

    switch(op_local)
    {
        case 1: strcpy(a->local, "Dentro do veiculo"); break;
        case 2: strcpy(a->local, "Na parada / estacao"); break;
        case 3: strcpy(a->local, "Na via publica"); break;
        case 4: strcpy(a->local, "No terminal"); break;
        case 5: strcpy(a->local, "Na garagem"); break;
        default: strcpy(a->local, "Outro ponto da rota"); break;
    }

    // --- O QUE ACONTECEU ---
    printf("\n=== O QUE ACONTECEU ===\n");
    printf("1 - Colisao entre veiculos\n");
    printf("2 - Atropelamento\n");
    printf("3 - Pneu furado\n");
    printf("4 - Motor superaquecido\n");
    printf("5 - Porta com defeito\n");
    printf("6 - Passageiro desmaiou\n");
    printf("7 - Passageiro passou mal\n");
    printf("8 - Briga entre passageiros\n");
    printf("9 - Buraco / obstaculo na pista\n");
    printf("10 - Semaforo ou sinal com defeito\n");
    printf("11 - Alagamento na via\n");
    printf("12 - Objeto danificado no veiculo\n");
    printf("13 - Outro\n");
    printf("Escolha: ");

    int op_desc;
    scanf("%d", &op_desc);

    switch(op_desc)
    {
        case 1:  strcpy(a->descricao, "Colisao entre veiculos"); break;
        case 2:  strcpy(a->descricao, "Atropelamento"); break;
        case 3:  strcpy(a->descricao, "Pneu furado"); break;
        case 4:  strcpy(a->descricao, "Motor superaquecido"); break;
        case 5:  strcpy(a->descricao, "Porta com defeito"); break;
        case 6:  strcpy(a->descricao, "Passageiro desmaiou"); break;
        case 7:  strcpy(a->descricao, "Passageiro passou mal"); break;
        case 8:  strcpy(a->descricao, "Briga entre passageiros"); break;
        case 9:  strcpy(a->descricao, "Buraco / obstaculo na pista"); break;
        case 10: strcpy(a->descricao, "Semaforo ou sinal com defeito"); break;
        case 11: strcpy(a->descricao, "Alagamento na via"); break;
        case 12: strcpy(a->descricao, "Objeto danificado no veiculo"); break;
        default: strcpy(a->descricao, "Outro"); break;
    }

    // --- CONFIRMACAO ---
    printf("\n%s\n", DIV);
    printf("PROTOCOLO : %s\n", a->protocolo);
    printf("TIPO      : %s\n", a->tipo);
    printf("GRAVIDADE : %s\n", a->gravidade);
    printf("LOCAL     : %s\n", a->local);
    printf("OCORRIDO  : %s\n", a->descricao);
    printf("DATA/HORA : %s\n", a->data_hora);
    printf("%s\n", DIV);

    printf("Confirmar envio? (s/n): ");
    char confirmar;
    scanf(" %c", &confirmar);

    if(confirmar == 's' || confirmar == 'S')
    {
        FILE *f = fopen("avisos.txt", "a");
        fprintf(f, "%s | %s | %s | %s | %s | %s | %s | %s\n",
                motorista.nome,
                motorista.linha,
                a->protocolo,
                a->data_hora,
                a->tipo,
                a->gravidade,
                a->local,
                a->descricao);
        fclose(f);

        motorista.total_avisos++;
        printf("\nAviso enviado com sucesso!\n");
    }
    else
    {
        printf("\nAviso cancelado.\n");
    }

    pausar();
}

// ======================================================
//                    MENU ADMIN
// ======================================================

void menu_admin()
{
    int op;

    do
    {
        cabecalho("MENU ADMINISTRADOR");

        printf("1 - Cadastrar Onibus\n");
        printf("2 - Cadastrar Parada\n");
        printf("3 - Cadastrar Motorista\n");
        printf("4 - Cadastrar Linha\n");
        printf("5 - Relatorio Geral\n");
        printf("0 - Voltar\n\n");
        printf("Escolha: ");
        scanf("%d", &op);

        switch(op)
        {
            case 1: cadastrar_onibus(); break;
            case 2: cadastrar_parada(); break;
            case 3: cadastrar_motorista(); break;
            case 4: cadastrar_linhas(); break;
            case 5: relatorio_geral(); break;
        }

    } while(op != 0);
}

// ======================================================
//                    MENU MOTORISTA
// ======================================================

void menu_motorista()
{
    int op;

    do
    {
        cabecalho("MENU DO MOTORISTA");

        printf("Motorista: %s\n", motorista.nome);
        printf("Linha: %s\n", motorista.linha);
        printf("Veiculo: %s\n", motorista.veiculo);
        printf("Inicio: %s\n", motorista.inicio_jornada);

        printf("\n1 - Enviar Aviso\n");
        printf("0 - Encerrar Jornada\n\n");
        printf("Escolha: ");
        scanf("%d", &op);

        switch(op)
        {
            case 1: enviar_aviso(); break;
        }

    } while(op != 0);
}

// ======================================================
//                    TELA INICIAL
// ======================================================

int main()
{
    setlocale(LC_ALL, "");
    int opcao;

    do
    {
        cabecalho("TELA INICIAL");

        printf("1 - Login Admin\n");
        printf("2 - Login Motorista\n");
        printf("0 - Sair\n\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        switch(opcao)
        {
            case 1:
                if(login_admin())
                    menu_admin();
                break;

            case 2:
                login_motorista();
                menu_motorista();
                break;
        }

    } while(opcao != 0);

    return 0;
}
