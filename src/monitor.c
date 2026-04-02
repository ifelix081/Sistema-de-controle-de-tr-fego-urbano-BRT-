#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

struct Status {
    int onibus;
    int motorista;
    int rota;
    char chegada[10];
    char saida[10];
    char situacao[50];
};

void monitorTempoReal() {

    FILE *arquivo = fopen("status.txt", "r");
    struct Status s;

    if (arquivo == NULL) {
        printf("Aguardando dados do sistema...\n");
        return;
    }

    printf("========= MONITOR BRT - TEMPO REAL =========\n\n");

    printf("ONIBUS | MOTORISTA | ROTA | STATUS | SAIDA\n");
    printf("--------------------------------------------\n");

    while (fscanf(arquivo, "%d %d %d %s %s %s",
                  &s.onibus,
                  &s.motorista,
                  &s.rota,
                  s.chegada,
                  s.saida,
                  s.situacao) != EOF) {

        printf("%5d | %9d | %4d | %-7s | %s\n",
               s.onibus, s.motorista, s.rota, s.situacao, s.saida);
    }

    fclose(arquivo);
}

/* --------- ATUALIZAÇÃO AUTOMÁTICA --------- */
int main() {

    while(1) {

        system("cls");     // limpa a tela
        monitorTempoReal();

        Sleep(3000);       // atualiza a cada 3 segundos
    }

    return 0;
}