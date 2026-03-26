#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

// ═══════════════════════════════════════
//              CONSTANTES
// ═══════════════════════════════════════

#define MAX_AVISOS 50    // maximo de avisos por jornada
#define TAM_STR    128   // tamanho padrao para textos curtos
#define TAM_DESC   512   // tamanho para textos longos

// ═══════════════════════════════════════
//              STRUCTS
// ═══════════════════════════════════════

// Struct que guarda os dados de um aviso enviado pelo motorista
struct Aviso
{
    char protocolo[32];    // codigo unico do aviso, ex: AVS-123456
    char hora[32];         // data e hora em que o aviso foi enviado
    char tipo[TAM_STR];    // tipo da ocorrencia, ex: Pane mecanica
    char gravidade[16];    // nivel de urgencia: BAIXA, MEDIA ou ALTA
    char local[TAM_STR];   // onde ocorreu
    char descricao[TAM_DESC]; // descricao detalhada do problema
};

// Struct que guarda os dados do motorista logado no sistema
struct Motorista
{
    char matricula[32];               // numero de identificacao do motorista
    char nome[TAM_STR];               // nome completo
    char linha[TAM_STR];              // linha que esta operando
    char veiculo[TAM_STR];            // identificacao do onibus
    char hora_inicio[32];             // horario em que fez o login
    int  logado;                      // 1 = logado, 0 = nao logado
    struct Aviso avisos[MAX_AVISOS];  // lista de avisos enviados na jornada
    int  total_avisos;                // quantos avisos foram enviados
};

// ═══════════════════════════════════════
//          VARIAVEL GLOBAL
// ═══════════════════════════════════════

struct Motorista motorista; // motorista que esta usando o sistema no momento

// ═══════════════════════════════════════
//              FUNCOES
// ═══════════════════════════════════════

// Captura a data e hora atual do sistema no formato dd/mm/aaaa hh:mm:ss
void obter_hora(char *buf, int tam)
{
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buf, tam, "%d/%m/%Y %H:%M:%S", tm_info);
}

// Gera um codigo unico para o aviso no formato AVS-XXXXXXXX
void gerar_protocolo(char *buf)
{
    time_t t = time(NULL);
    srand((unsigned int)t); // inicializa o gerador com o horario atual
    snprintf(buf, 32, "AVS-%04d%02d", rand() % 9999, rand() % 99);
}

// Realiza o login do motorista coletando seus dados e marcando logado = 1
void login()
{
    char mat[32], nome[TAM_STR], linha[TAM_STR], veic[TAM_STR];

    printf("---Login do Motorista---\n");
    printf("Matricula: ");
    scanf(" %31s", mat);
    printf("Nome: ");
    scanf(" %127[^\n]", nome);
    printf("Linha: ");
    scanf(" %127[^\n]", linha);
    printf("Veiculo: ");
    scanf(" %127[^\n]", veic);

    // Copia os dados para a struct global do motorista
    strncpy(motorista.matricula, mat,   sizeof(motorista.matricula)  - 1);
    strncpy(motorista.nome,      nome,  sizeof(motorista.nome)       - 1);
    strncpy(motorista.linha,     linha, sizeof(motorista.linha)      - 1);
    strncpy(motorista.veiculo,   veic,  sizeof(motorista.veiculo)    - 1);

    obter_hora(motorista.hora_inicio, sizeof(motorista.hora_inicio));
    motorista.logado       = 1; // marca como logado
    motorista.total_avisos = 0; // zera o contador de avisos

    printf("Jornada iniciada as %s\n", motorista.hora_inicio);
}

// Coleta os dados de uma ocorrencia e salva no arquivo avisos_MATRICULA.txt
void enviar_aviso()
{
    // Lista de tipos de ocorrencia disponiveis
    const char *tipos[] = {
        "Pane mecanica", "Pane eletrica", "Acidente", "Pneu furado",
        "Problema com portas", "Ar-condicionado com defeito",
        "Transito intenso", "Bloqueio na via / desvio", "Obra na pista",
        "Alagamento / via intransitavel", "Manifestacao na rota",
        "Ponto de parada obstruido", "Passageiro com mal-estar",
        "Ocorrencia de seguranca", "Superlotacao", "Outros"
    };

    // Exibe o menu de tipos de ocorrencia
    printf("\n---Tipos de ocorrencia---\n");
    printf("--- Onibus ---\n");
    printf(" 1. Pane mecanica\n");
    printf(" 2. Pane eletrica\n");
    printf(" 3. Acidente\n");
    printf(" 4. Pneu furado\n");
    printf(" 5. Problema com portas\n");
    printf(" 6. Ar-condicionado com defeito\n");
    printf("--- Rota / Linha ---\n");
    printf(" 7. Transito intenso\n");
    printf(" 8. Bloqueio na via / desvio\n");
    printf(" 9. Obra na pista\n");
    printf("10. Alagamento / via intransitavel\n");
    printf("11. Manifestacao na rota\n");
    printf("12. Ponto de parada obstruido\n");
    printf("--- Passageiros / Seguranca ---\n");
    printf("13. Passageiro com mal-estar\n");
    printf("14. Ocorrencia de seguranca\n");
    printf("15. Superlotacao\n");
    printf("16. Outros\n");

    int opcao_tipo;
    printf("Tipo (1-16): ");
    scanf("%d", &opcao_tipo);
    if (opcao_tipo < 1 || opcao_tipo > 16)
    {
        printf("Opcao invalida!\n");
        return;
    }

    // Exibe e le a gravidade
    printf("\nGravidade:\n");
    printf("1. BAIXA\n");
    printf("2. MEDIA\n");
    printf("3. ALTA\n");

    const char *gravidades[] = {"BAIXA", "MEDIA", "ALTA"};
    int opcao_grav;
    printf("Gravidade (1-3): ");
    scanf("%d", &opcao_grav);
    if (opcao_grav < 1 || opcao_grav > 3)
    {
        printf("Opcao invalida!\n");
        return;
    }

    char local[TAM_STR], desc[TAM_DESC];
    printf("Localizacao atual: ");
    scanf(" %127[^\n]", local);
    printf("Descricao: ");
    scanf(" %511[^\n]", desc);

    // Verifica se ainda ha espaco na lista de avisos
    if (motorista.total_avisos >= MAX_AVISOS)
    {
        printf("Limite de avisos atingido!\n");
        return;
    }

    // Preenche o proximo aviso na lista
    struct Aviso *a = &motorista.avisos[motorista.total_avisos];
    gerar_protocolo(a->protocolo);
    obter_hora(a->hora, sizeof(a->hora));
    strncpy(a->tipo,      tipos[opcao_tipo - 1],      sizeof(a->tipo)      - 1);
    strncpy(a->gravidade, gravidades[opcao_grav - 1], sizeof(a->gravidade) - 1);
    strncpy(a->local,     local,                      sizeof(a->local)     - 1);
    strncpy(a->descricao, desc,                       sizeof(a->descricao) - 1);

    motorista.total_avisos++; // incrementa o contador apos preencher

    // Monta o nome do arquivo com a matricula do motorista
    char nome_arq[64];
    snprintf(nome_arq, sizeof(nome_arq), "avisos_%s.txt", motorista.matricula);

    // Abre em modo "a" (append) para acumular avisos sem apagar os anteriores
    FILE *f = fopen(nome_arq, "a");
    if (f == NULL)
    {
        printf("Erro ao abrir o arquivo\n");
        return;
    }

    // Escreve o cabecalho apenas no primeiro aviso da jornada
    if (motorista.total_avisos == 1)
    {
        fprintf(f, "======================================================\n");
        fprintf(f, "      SISTEMA DE AVISOS - SISBUS\n");
        fprintf(f, "      Motorista : %s  |  Mat.: %s\n", motorista.nome, motorista.matricula);
        fprintf(f, "      Linha     : %s  |  Veiculo: %s\n", motorista.linha, motorista.veiculo);
        fprintf(f, "      Inicio    : %s\n", motorista.hora_inicio);
        fprintf(f, "======================================================\n\n");
    }

    // Escreve os dados do aviso no arquivo
    fprintf(f, "------------------------------------------------------\n");
    fprintf(f, "  AVISO #%d - PROTOCOLO: %s\n", motorista.total_avisos, a->protocolo);
    fprintf(f, "------------------------------------------------------\n");
    fprintf(f, "  Data/Hora  : %s\n", a->hora);
    fprintf(f, "  Tipo       : %s\n", a->tipo);
    fprintf(f, "  Gravidade  : %s\n", a->gravidade);
    fprintf(f, "  Localizacao: %s\n", a->local);
    fprintf(f, "  Descricao  : %s\n\n", a->descricao);

    fclose(f);
    printf("Aviso registrado com sucesso! Protocolo: %s\n", a->protocolo);
    printf("Arquivo gerado: %s\n", nome_arq);
}

// Exibe os avisos enviados na jornada atual
void ver_avisos()
{
    if (motorista.total_avisos == 0)
    {
        printf("Nenhum aviso enviado ainda.\n");
        return;
    }

    printf("\n---Avisos da Jornada---\n");
    int i;
    for (i = 0; i < motorista.total_avisos; i++) // declaracao do i fora do for (C89)
    {
        struct Aviso *a = &motorista.avisos[i];
        printf("[%d] %s | %s | %s | %s\n",
               i + 1, a->protocolo, a->tipo, a->gravidade, a->hora);
    }
}

// Encerra a jornada, gera o resumo em jornada_MATRICULA.txt e zera os dados
void logoff()
{
    char conf;
    printf("Confirmar logoff? (s/n): ");
    scanf(" %c", &conf);

    if (conf != 's' && conf != 'S')
    {
        printf("Logoff cancelado.\n");
        return;
    }

    // Registra o horario de encerramento
    char hora_fim[32];
    obter_hora(hora_fim, sizeof(hora_fim));

    // Monta o nome do arquivo de resumo com a matricula
    char nome_arq[64];
    snprintf(nome_arq, sizeof(nome_arq), "jornada_%s.txt", motorista.matricula);

    // Abre em modo "w" para criar do zero (substitui jornadas anteriores)
    FILE *f = fopen(nome_arq, "w");
    if (f == NULL)
    {
        printf("Erro ao criar o arquivo de resumo\n");
        return;
    }

    // Escreve o cabecalho e os dados da jornada
    fprintf(f, "======================================================\n");
    fprintf(f, "      SISBUS - RELATORIO DE JORNADA\n");
    fprintf(f, "======================================================\n\n");
    fprintf(f, "  Motorista   : %s\n", motorista.nome);
    fprintf(f, "  Matricula   : %s\n", motorista.matricula);
    fprintf(f, "  Linha       : %s\n", motorista.linha);
    fprintf(f, "  Veiculo     : %s\n", motorista.veiculo);
    fprintf(f, "  Inicio      : %s\n", motorista.hora_inicio);
    fprintf(f, "  Encerramento: %s\n", hora_fim);
    fprintf(f, "  Avisos      : %d\n\n", motorista.total_avisos);

    // So escreve o resumo de ocorrencias se houver ao menos 1 aviso
    if (motorista.total_avisos > 0)
    {
        fprintf(f, "------------------------------------------------------\n");
        fprintf(f, "  RESUMO DAS OCORRENCIAS\n");
        fprintf(f, "------------------------------------------------------\n");

        int i;
        for (i = 0; i < motorista.total_avisos; i++) // i declarado fora do for (C89)
        {
            struct Aviso *a = &motorista.avisos[i];
            fprintf(f, "  [%d] %s | %-35s | %s\n",
                    i + 1, a->protocolo, a->tipo, a->gravidade);
        }

        // Referencia o arquivo de avisos detalhados
        char arq_avisos[64];
        snprintf(arq_avisos, sizeof(arq_avisos), "avisos_%s.txt", motorista.matricula);
        fprintf(f, "\n  Detalhes completos em: %s\n", arq_avisos);
    }

    fprintf(f, "\n======================================================\n");
    fclose(f);

    printf("Jornada encerrada. Relatorio gerado: %s\n", nome_arq);
    printf("Ate logo, %s!\n", motorista.nome);

    // Zera todos os dados do motorista na memoria
    memset(&motorista, 0, sizeof(struct Motorista));
}

// ═══════════════════════════════════════
//              MAIN
// ═══════════════════════════════════════

int main()
{
    int opcao; // guarda a opcao digitada pelo usuario

    // Zera a struct antes de comecar para evitar lixo na memoria
    memset(&motorista, 0, sizeof(struct Motorista));

    // Loop principal: fica rodando ate o usuario escolher sair (opcao 0)
    do
    {
        // Exibe o menu de acordo com o estado de login
        printf("\n===================================\n");
        printf("        SISBUS - AVISOS\n");
        printf("===================================\n");

        if (!motorista.logado)
        {
            // Menu para motorista deslogado
            printf("1 - Login\n");
            printf("0 - Sair\n");
        }
        else
        {
            // Menu para motorista logado
            printf("Motorista: %s | Linha: %s\n", motorista.nome, motorista.linha);
            printf("Avisos enviados: %d\n", motorista.total_avisos);
            printf("-----------------------------------\n");
            printf("1 - Enviar Aviso\n");
            printf("2 - Ver Avisos da Jornada\n");
            printf("3 - Logoff\n");
            printf("0 - Sair\n");
        }

        printf("===================================\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        // Chama a funcao correspondente dependendo do estado de login
        if (!motorista.logado)
        {
            switch (opcao)
            {
                case 1: login();                   break;
                case 0: printf("Saindo...\n");     break;
                default: printf("Opcao invalida!\n");
            }
        }
        else
        {
            switch (opcao)
            {
                case 1: enviar_aviso();            break;
                case 2: ver_avisos();              break;
                case 3: logoff();                  break;
                case 0: printf("Saindo...\n");     break;
                default: printf("Opcao invalida!\n");
            }
        }

    } while (opcao != 0);

    return 0;
}