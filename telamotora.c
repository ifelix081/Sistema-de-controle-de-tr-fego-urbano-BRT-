//bibliotecas necessarias

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//vou testar isso no codigo, vai que fica legal
#define linha_sep "========================================"
#define linha_div "----------------------------------------"

//variavei para adicionar
//variaveis do aviso
struct 
{
    char protocolo[20];
    char hora[6];
    char tipo[20];
    char gravidade[20];
    char local[100];
    char descricao[200];
} aviso;
//variaveis do login motorista
struct 
{
    char matricula[10];
    char nome[50];
    char linha[20];
    char veiculo[20];
    char hora_inicio[32];
    int logado;
    aviso avisos[10];
} motorista;
//funcao para obter a hora atual
void obter_hora(char *buf, int tam)
{
    time_t t = time(NULL); //pega a hora atual
    struct tm *tm_info = localtime(&t); //converte para hora local
    strftime(buf, tam, "%d/%m/%y %H;%M:%S", tm_info); //formata a hora
}
//funcao para gerar protocolo
void gerar_protocolo(char *buf)
{
    time_t t = time(NULL);
    srand((unsigned int)t); //inicia o gerador de numeros aleat com horario
    snprintf(buf, 20, "prot-%04d%02d", rand() % 9999, rand() % 99); // formata o protocolo PROT + 4 numeros + 2 numeros
}
//funcao para ver linhas
void ler_linhas(const char *prompt, char *buf, int tam)
{
    printf("%s", prompt); //mostra o prompt
    fflush(stdout); //garante que o prompt seja mostrado
    if (fgets(buf, tam, stdin)) //lê a linha
    {
        buf[strcspn(buf, "\n")] = '\0'; //remove o \n
    }
}
//cabecalho
void cabecalho()
{
    printf("%s\n", linha_sep);
    printf("SISTEMA DE CONTROLE DE TRAFEGO URBANO BRT\n");
    printf("%s\n", linha_sep);
//so exibe os dados do motorista se ele estiver logado
    if (motorista.logado)
    {
        printf("Motorista: %s\n", motorista.nome);
        printf("Linha: %s\n", motorista.linha);
        printf("Veiculo: %s\n", motorista.veiculo);
        printf("Hora de inicio: %s\n", motorista.hora_inicio);
    }
}
//salva o aviso em txt
void salvar_aviso(const aviso *av)
{
    char nome_arq[256];
    snprintf(nome_arq, sizeof(nome_arq), "aviso_%s.txt", motorista.matricula);

    FILE *f = fopen(nome_arq, "a");
    if (!f)
    {
        printf("%s [ERRO] Nao foi possivel abrir o arquivo %s\n", linha_sep, nome_arq);
        return;
    }
    fprintf(f, "%s %s %s %s %s %s\n", aviso.protocolo, aviso.hora, aviso.tipo, aviso.gravidade, aviso.local, aviso.descricao);
    fclose(f);

//cabecalho do aviso, so faz 1 vez
if(motorista.total_aviso == 1)
{
    fprintf(f, linha_sep);
    fprintf(f, "  SISTEMA DE AVISOS - SISBUS\n");
    fprintf(f, "  Motorista : %s  |  Mat.: %s\n", motorista.nome, motorista.matricula);
    fprintf(f, "  Linha : %s  |  Veiculo : %s\n", motorista.linha, motorista.veiculo);
    fprintf(f, "  Inicio : %s", motorista.hora_inicio);
    printf("%s", linha_sep);
    printf(f, "\n");
}
// imprime o aviso
fprint(f, linha_div);
fprint(f, "AVISO #%d - PROTOCOLO: %s\n", motorista.total_aviso, a->protocolo);
fprint(f, linha_div);
fprintf(f, "  Data/Hora  : %s\n", a->hora);
fprintf(f, "  Tipo       : %s\n", a->tipo);
fprintf(f, "  Gravidade  : %s\n", a->gravidade);
fprintf(f, "  Localizacao: %s\n", a->local);
fprintf(f, "  Descricao  : ");
//codigo para quebrar a linha a cada 50 caracteres
const char *p = a->descricao;
int col = 0;
while (*p)
{
   fputc(*p, f);
   col++;
// quebra de linha a cada 50 caracteres
   if(col >= 50 && *p == ' ')
   {
    fprintf(f, "\n           "); //quebra de linha com o campo "Descricao"
    col = 0; //reseta a contagem
   }
   p++;
}
fprintf(f, "\n");
fclose(f);
printf("\n%s Aviso registrado em:%s%s%s\n", nome_arq);
}

void salvar_jornada()
{
 char hora_fim[32] 
 obter_hora(hora_fim, sizeof(hora_fim)); // registra hora de fim
 //nome do arquivo inclui a matricula do motorista
 char nome_arq[64];
 snprintf(nome_arq, sizeof(nome_arq), "jornada_%s.txt", motorista.matricula);
   //abre em modo de escrita, apagando o arquivo se ele já existir
   FILE *f =fopen(nome_arq, "w");
   if(!f)
   {
    printf("%s [ERRO] Nao foi possivel abrir o arquivo %s\n", linha_sep, nome_arq);
    return;
   }
   fprintf(f, LINHA_SEP);
   fprintf(f, "      SISBUS - RELATORIO DE JORNADA\n");
   fprintf(f, LINHA_SEP);
   fprintf(f, "\n");
   fprintf(f, "  Motorista   : %s\n", motorista.nome);
   fprintf(f, "  Matricula   : %s\n", motorista.matricula);
   fprintf(f, "  Linha       : %s\n", motorista.linha);
   fprintf(f, "  Veiculo     : %s\n", motorista.veiculo);
   fprintf(f, "  Inicio      : %s\n", motorista.hora_inicio);
   fprintf(f, "  Encerramento: %s\n", hora_fim);
   fprintf(f, "  Avisos      : %d\n", motorista.total_avisos);
   fprintf(f, "\n");

   if(motorista.total_avisos > 0)
   {
    fprint(f, LINHA_DIV);
    fprintf(f, "  resumo das ocorrencias\n");
    fprintf(f, LINHA_DIV);
        aviso *a = &motorista.total_avisos; i++)
        {
            fprint(f, " [%d] %s | %-35s | %s\n", i+1, a->protocolo, a->tipo, a->gravidade);
        }
        fprintf(f, "\n");
        char arq_avisos[64];
        snprintf(arq_avisos, sizeof(arq_avisos), "avisos_%s.txt", motorista.matricula);
        fprintf(f, "  Ver arquivo completo: %s\n", arq_avisos);
   }
   fprintf(f, LINHA_SEP);
   fclose(f);
   printf("\n%s Relatorio de jornada: %s%s%s\n", nome_arq);
}

void tela_login()
{
 cabecalho();
printf("Login do Motorista %s\n\n");
char mat[32], nome[64], linha[64], veic[64];

   /* Lê cada campo e valida imediatamente. Se vazio, exibe erro e sai */
    ler_linha("  Matrícula  : ", mat, sizeof(mat));
    if (strlen(mat) == 0) { printf("%s  Matrícula inválida.%s\n"); pausar(); return; }

    ler_linha("  Nome       : ", nome, sizeof(nome));
    if (strlen(nome) == 0) { printf("%s  Nome inválido.%s\n"); pausar(); return; }

    ler_linha("  Linha      : ", linha, sizeof(linha));
    if (strlen(linha) == 0) { printf("%s  Linha inválida.%s\n"); pausar(); return; }

    ler_linha("  Veículo    : ", veic, sizeof(veic));
    if (strlen(veic) == 0) { printf("%s  Veículo inválido.%s\n", , ); pausar(); return; }

    strncpy(motorista.matricula, mat,   sizeof(motorista.matricula) - 1); //strncpy serve para copiar uma string de um lugar para outro
    strncpy(motorista.nome,      nome,  sizeof(motorista.nome)      - 1);
    strncpy(motorista.linha,     linha, sizeof(motorista.linha)     - 1);
    strncpy(motorista.veiculo,   veic,  sizeof(motorista.veiculo)   - 1);

}


//em progresso, ta indo bem