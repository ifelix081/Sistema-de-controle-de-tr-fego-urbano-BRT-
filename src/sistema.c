#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define SEP  "===================================================="
#define DIV  "----------------------------------------------------"
#define MAX  20

typedef struct {
    int  lin;
    int  num;
    char pla[10];
    char gar[50];
    char mot[50];
    char tur[20];
} Onibus;

typedef struct {
    int  cod;
    char end[100];
} Parada;

typedef struct {
    int codLin;
    int codPar;
} Linha;

typedef struct {
    char nom[50];
    char cpf[15];
    char tel[16];
    int  lin;
} Motorista;

typedef struct {
    char prot[20];
    char dth[25];
    char tip[50];
    char grav[20];
    char loc[100];
    char desc[200];
} Aviso;

typedef struct {
    char mat[15];
    char nom[50];
    char lin[20];
    char vei[20];
    char ini[25];
    int  ativo;
    Aviso av[MAX];
    int  tot;
} Sessao;

Sessao ses;

void limpar()
{
    system("cls || clear");
}

void pausar()
{
    printf("\n  Pressione ENTER para continuar...");
    getchar();
    getchar();
}

void dataHora(char *buf, int tam)
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(buf, tam, "%d/%m/%Y %H:%M:%S", tm);
}

void trim(char *s)
{
    int i = 0, f = strlen(s) - 1;
    while (isspace((unsigned char)s[i])) i++;
    while (f > i && isspace((unsigned char)s[f])) f--;
    memmove(s, s + i, f - i + 1);
    s[f - i + 1] = '\0';
}

void cabec(const char *tit)
{
    limpar();
    printf("\n%s\n", SEP);
    printf("        SISTEMA DE CONTROLE BRT\n");
    printf("%s\n", SEP);
    printf("  %s\n", tit);
    printf("%s\n\n", DIV);
}

void ok(const char *msg)
{
    printf("\n  [OK] %s\n", msg);
}

void erro(const char *msg)
{
    printf("\n  [!] %s\n", msg);
}

void aviso(const char *msg)
{
    printf("\n  [~] %s\n", msg);
}

int confirmar(const char *txt)
{
    char r;
    printf("\n  %s (s/n): ", txt);
    scanf(" %c", &r);
    return (r == 's' || r == 'S');
}

int vazio(const char *val, const char *campo)
{
    if (strlen(val) == 0) {
        printf("  [!] O campo '%s' nao pode ficar vazio.\n", campo);
        return 1;
    }
    return 0;
}

int cpfOk(const char *c)
{
    if (strlen(c) != 11) return 0;
    for (int i = 0; i < 11; i++)
        if (!isdigit((unsigned char)c[i])) return 0;
    return 1;
}

int placaOk(const char *p)
{
    return strlen(p) >= 7;
}

int loginAdmin()
{
    char usr[20], sen[20];

    cabec("LOGIN — ADMINISTRADOR");
    printf("  Informe suas credenciais de acesso:\n\n");
    printf("  Usuario : ");
    scanf("%19s", usr);
    printf("  Senha   : ");
    scanf("%19s", sen);

    if (strcmp(usr, "admin") == 0 && strcmp(sen, "1234") == 0) {
        ok("Acesso liberado! Bem-vindo ao painel administrativo.");
        pausar();
        return 1;
    }

    erro("Usuario ou senha incorretos. Tente novamente.");
    pausar();
    return 0;
}

void cadOnibus()
{
    Onibus b;
    int tent = 0;

    cabec("CADASTRO DE ONIBUS");
    printf("  Preencha os dados do veiculo:\n\n");

    printf("  Numero da linha    : ");
    while (scanf("%d", &b.lin) != 1 || b.lin <= 0) {
        erro("Numero de linha invalido. Digite um inteiro positivo.");
        printf("  Numero da linha    : ");
        while (getchar() != '\n');
    }

    printf("  Numero do onibus   : ");
    while (scanf("%d", &b.num) != 1 || b.num <= 0) {
        erro("Numero de onibus invalido.");
        printf("  Numero do onibus   : ");
        while (getchar() != '\n');
    }

    do {
        printf("  Placa (ex: ABC1234): ");
        scanf("%9s", b.pla);
        if (!placaOk(b.pla)) {
            erro("Placa invalida. Use pelo menos 7 caracteres.");
            tent++;
        }
    } while (!placaOk(b.pla) && tent < 3);

    if (tent >= 3) {
        aviso("Muitas tentativas invalidas. Cadastro cancelado.");
        pausar();
        return;
    }

    printf("  Garagem            : ");
    scanf(" %49[^\n]", b.gar);
    trim(b.gar);
    if (vazio(b.gar, "Garagem")) { pausar(); return; }

    printf("  Motorista          : ");
    scanf(" %49[^\n]", b.mot);
    trim(b.mot);
    if (vazio(b.mot, "Motorista")) { pausar(); return; }

    printf("  Turno (manha/tarde/noite): ");
    scanf("%19s", b.tur);

    printf("\n  Resumo do cadastro:\n");
    printf("  %s\n", DIV);
    printf("  Linha    : %d\n", b.lin);
    printf("  Onibus   : %d\n", b.num);
    printf("  Placa    : %s\n", b.pla);
    printf("  Garagem  : %s\n", b.gar);
    printf("  Motorista: %s\n", b.mot);
    printf("  Turno    : %s\n", b.tur);
    printf("  %s\n", DIV);

    if (!confirmar("Confirmar o cadastro deste onibus?")) {
        aviso("Cadastro cancelado pelo usuario.");
        pausar();
        return;
    }

    FILE *f = fopen("dados_onibus.csv", "a");
    if (!f) {
        erro("Nao foi possivel abrir o arquivo de dados.");
        pausar();
        return;
    }
    fprintf(f, "%d;%d;%s;%s;%s;%s\n", b.lin, b.num, b.pla, b.gar, b.mot, b.tur);
    fclose(f);

    ok("Onibus cadastrado com sucesso no sistema!");
    pausar();
}

void cadParada()
{
    Parada p;

    cabec("CADASTRO DE PARADA");
    printf("  Informe os dados da nova parada:\n\n");

    printf("  Codigo da parada: ");
    while (scanf("%d", &p.cod) != 1 || p.cod <= 0) {
        erro("Codigo invalido. Digite um inteiro positivo.");
        printf("  Codigo da parada: ");
        while (getchar() != '\n');
    }

    printf("  Endereco completo: ");
    scanf(" %99[^\n]", p.end);
    trim(p.end);
    if (vazio(p.end, "Endereco")) { pausar(); return; }

    printf("\n  Resumo:\n");
    printf("  Codigo  : %d\n", p.cod);
    printf("  Endereco: %s\n", p.end);

    if (!confirmar("Confirmar o cadastro desta parada?")) {
        aviso("Cadastro cancelado.");
        pausar();
        return;
    }

    FILE *f = fopen("dados_paradas.csv", "a");
    if (!f) {
        erro("Erro ao acessar o arquivo de paradas.");
        pausar();
        return;
    }
    fprintf(f, "%d;%s\n", p.cod, p.end);
    fclose(f);

    ok("Parada cadastrada com sucesso!");
    pausar();
}

void cadMotorista()
{
    Motorista m;

    cabec("CADASTRO DE MOTORISTA");
    printf("  Preencha os dados do novo motorista:\n\n");

    printf("  Nome completo   : ");
    scanf(" %49[^\n]", m.nom);
    trim(m.nom);
    if (vazio(m.nom, "Nome")) { pausar(); return; }

    do {
        printf("  CPF (so numeros): ");
        scanf("%14s", m.cpf);
        if (!cpfOk(m.cpf))
            erro("CPF invalido. Digite exatamente 11 digitos numericos.");
    } while (!cpfOk(m.cpf));

    printf("  Telefone        : ");
    scanf("%15s", m.tel);

    printf("  Linha de atuacao: ");
    while (scanf("%d", &m.lin) != 1 || m.lin <= 0) {
        erro("Numero de linha invalido.");
        printf("  Linha de atuacao: ");
        while (getchar() != '\n');
    }

    printf("\n  Resumo do cadastro:\n");
    printf("  %s\n", DIV);
    printf("  Nome : %s\n", m.nom);
    printf("  CPF  : %s\n", m.cpf);
    printf("  Fone : %s\n", m.tel);
    printf("  Linha: %d\n", m.lin);
    printf("  %s\n", DIV);

    if (!confirmar("Confirmar o cadastro deste motorista?")) {
        aviso("Cadastro cancelado.");
        pausar();
        return;
    }

    FILE *f = fopen("dados_motoristas.csv", "a");
    if (!f) {
        erro("Erro ao acessar o arquivo de motoristas.");
        pausar();
        return;
    }
    fprintf(f, "%s;%s;%s;%d\n", m.nom, m.cpf, m.tel, m.lin);
    fclose(f);

    ok("Motorista cadastrado com sucesso!");
    pausar();
}

void cadLinha()
{
    Linha l;
    char cont;
    int tot = 0;

    cabec("CADASTRO DE LINHA");
    printf("  Defina o trajeto da nova linha:\n\n");

    printf("  Codigo da linha: ");
    while (scanf("%d", &l.codLin) != 1 || l.codLin <= 0) {
        erro("Codigo invalido. Digite um inteiro positivo.");
        printf("  Codigo da linha: ");
        while (getchar() != '\n');
    }

    FILE *f = fopen("dados_linhas.csv", "a");
    if (!f) {
        erro("Erro ao acessar o arquivo de linhas.");
        pausar();
        return;
    }
    fprintf(f, "%d", l.codLin);

    printf("\n  Agora adicione as paradas desta linha.\n\n");

    do {
        printf("  Codigo da parada %d: ", tot + 1);
        while (scanf("%d", &l.codPar) != 1 || l.codPar <= 0) {
            erro("Codigo invalido.");
            printf("  Codigo da parada %d: ", tot + 1);
            while (getchar() != '\n');
        }
        fprintf(f, ";%d", l.codPar);
        tot++;
        printf("  Deseja adicionar mais uma parada? (s/n): ");
        scanf(" %c", &cont);
    } while (cont == 's' || cont == 'S');

    fprintf(f, "\n");
    fclose(f);

    printf("\n  Linha %d cadastrada com %d parada(s).\n", l.codLin, tot);
    ok("Linha registrada com sucesso no sistema!");
    pausar();
}

void exibirArq(const char *arq, const char *tit)
{
    char lin[300];
    int tot = 0;
    FILE *f = fopen(arq, "r");

    printf("\n  --- %s ---\n", tit);
    if (!f) {
        printf("  (nenhum registro encontrado)\n");
        return;
    }
    while (fgets(lin, sizeof(lin), f)) {
        printf("  %s", lin);
        tot++;
    }
    fclose(f);
    if (tot == 0)
        printf("  (arquivo vazio)\n");
    else
        printf("  Total: %d registro(s)\n", tot);
}

void relatorio()
{
    char dth[25];
    dataHora(dth, sizeof(dth));

    cabec("RELATORIO GERAL DO SISTEMA");
    printf("  Gerado em: %s\n", dth);

    exibirArq("dados_onibus.csv",     "ONIBUS CADASTRADOS");
    exibirArq("dados_paradas.csv",    "PARADAS CADASTRADAS");
    exibirArq("dados_motoristas.csv", "MOTORISTAS CADASTRADOS");
    exibirArq("dados_linhas.csv",     "LINHAS CADASTRADAS");
    exibirArq("avisos.csv",           "AVISOS REGISTRADOS");

    printf("\n%s\n", DIV);
    pausar();
}

void loginMotorista()
{
    cabec("LOGIN — MOTORISTA");
    printf("  Ola! Identifique-se para iniciar sua jornada:\n\n");

    printf("  Matricula : ");
    scanf("%14s", ses.mat);

    printf("  Seu nome  : ");
    scanf(" %49[^\n]", ses.nom);
    trim(ses.nom);

    printf("  Linha     : ");
    scanf("%19s", ses.lin);

    printf("  Veiculo   : ");
    scanf("%19s", ses.vei);

    dataHora(ses.ini, sizeof(ses.ini));
    ses.tot   = 0;
    ses.ativo = 1;

    printf("\n%s\n", DIV);
    printf("  SESSAO INICIADA COM SUCESSO\n");
    printf("%s\n", DIV);
    printf("  Motorista : %s\n", ses.nom);
    printf("  Matricula : %s\n", ses.mat);
    printf("  Linha     : %s\n", ses.lin);
    printf("  Veiculo   : %s\n", ses.vei);
    printf("  Inicio    : %s\n", ses.ini);
    printf("%s\n", DIV);
    printf("  Bom turno, %s! Dirija com seguranca.\n", ses.nom);

    pausar();
}

void enviarAviso()
{
    if (ses.tot >= MAX) {
        erro("Limite de avisos da sessao atingido. Encerre e inicie nova jornada.");
        pausar();
        return;
    }

    Aviso *a = &ses.av[ses.tot];

    cabec("ENVIAR AVISO AO CONTROLE");
    printf("  Use este formulario para reportar problemas durante o percurso.\n\n");

    dataHora(a->dth, sizeof(a->dth));
    srand((unsigned int)time(NULL));
    sprintf(a->prot, "AV-%s-%04d", ses.mat, rand() % 9999);

    printf("  Tipo do problema\n");
    printf("  (ex: Mecanico, Passageiro, Via, Atraso, Outro)\n");
    printf("  > ");
    scanf(" %49[^\n]", a->tip);
    trim(a->tip);
    if (vazio(a->tip, "Tipo do problema")) { pausar(); return; }

    printf("\n  Gravidade\n");
    printf("  (Baixa / Media / Alta / Urgente)\n");
    printf("  > ");
    scanf(" %19[^\n]", a->grav);

    printf("\n  Local exato onde ocorreu:\n");
    printf("  > ");
    scanf(" %99[^\n]", a->loc);
    trim(a->loc);

    printf("\n  Descreva o que aconteceu (seja o mais claro possivel):\n");
    printf("  > ");
    scanf(" %199[^\n]", a->desc);
    trim(a->desc);

    printf("\n  %s\n", DIV);
    printf("  RESUMO DO AVISO\n");
    printf("  Protocolo : %s\n", a->prot);
    printf("  Data/Hora : %s\n", a->dth);
    printf("  Motorista : %s (linha %s)\n", ses.nom, ses.lin);
    printf("  Tipo      : %s\n", a->tip);
    printf("  Gravidade : %s\n", a->grav);
    printf("  Local     : %s\n", a->loc);
    printf("  Descricao : %s\n", a->desc);
    printf("  %s\n", DIV);

    if (!confirmar("Confirmar o envio deste aviso?")) {
        aviso("Aviso nao enviado. Voce pode preencher novamente.");
        pausar();
        return;
    }

    FILE *f = fopen("avisos.csv", "a");
    if (!f) {
        erro("Nao foi possivel registrar o aviso. Tente novamente.");
        pausar();
        return;
    }
    fprintf(f, "%s;%s;%s;%s;%s;%s;%s;%s\n",
            a->prot, a->dth, ses.nom, ses.lin,
            a->tip, a->grav, a->loc, a->desc);
    fclose(f);

    ses.tot++;
    printf("\n  Aviso registrado com protocolo: %s\n", a->prot);
    ok("Aviso enviado ao controle central com sucesso!");
    pausar();
}

void historico()
{
    cabec("HISTORICO DA JORNADA");

    printf("  Motorista : %s\n", ses.nom);
    printf("  Inicio    : %s\n", ses.ini);
    printf("  Avisos    : %d registrado(s)\n\n", ses.tot);

    if (ses.tot == 0) {
        printf("  Nenhum aviso foi enviado nesta jornada.\n");
    } else {
        for (int i = 0; i < ses.tot; i++) {
            printf("  %s\n", DIV);
            printf("  Aviso #%d\n", i + 1);
            printf("  Protocolo : %s\n", ses.av[i].prot);
            printf("  Horario   : %s\n", ses.av[i].dth);
            printf("  Tipo      : %s | Gravidade: %s\n", ses.av[i].tip, ses.av[i].grav);
            printf("  Local     : %s\n", ses.av[i].loc);
        }
        printf("  %s\n", DIV);
    }
    pausar();
}

void encerrar()
{
    char fim[25];
    dataHora(fim, sizeof(fim));

    cabec("ENCERRAR JORNADA");
    printf("  Resumo da sua jornada:\n\n");
    printf("  Motorista : %s\n", ses.nom);
    printf("  Linha     : %s\n", ses.lin);
    printf("  Veiculo   : %s\n", ses.vei);
    printf("  Inicio    : %s\n", ses.ini);
    printf("  Termino   : %s\n", fim);
    printf("  Avisos    : %d enviado(s)\n", ses.tot);

    if (ses.tot > 0)
        aviso("Voce enviou avisos nesta jornada. O controle ja foi notificado.");

    printf("\n  Bom descanso, %s! Ate o proximo turno.\n", ses.nom);
    ses.ativo = 0;
    pausar();
}

void menuAdmin()
{
    int op;

    do {
        cabec("PAINEL DO ADMINISTRADOR");
        printf("  O que voce deseja fazer?\n\n");
        printf("  1. Cadastrar novo onibus\n");
        printf("  2. Cadastrar nova parada\n");
        printf("  3. Cadastrar novo motorista\n");
        printf("  4. Cadastrar nova linha\n");
        printf("  5. Ver relatorio geral\n");
        printf("  0. Sair do painel\n\n");
        printf("  Sua escolha: ");

        if (scanf("%d", &op) != 1) {
            while (getchar() != '\n');
            op = -1;
        }

        switch (op) {
            case 1: cadOnibus();    break;
            case 2: cadParada();    break;
            case 3: cadMotorista(); break;
            case 4: cadLinha();     break;
            case 5: relatorio();    break;
            case 0: printf("\n  Saindo do painel administrativo...\n"); break;
            default:
                erro("Opcao invalida. Escolha um numero entre 0 e 5.");
                pausar();
        }
    } while (op != 0);
}

void menuMotorista()
{
    int op;

    do {
        cabec("PAINEL DO MOTORISTA");
        printf("  Ola, %s!\n", ses.nom);
        printf("  Linha: %-10s | Veiculo: %-10s | Avisos: %d\n\n",
               ses.lin, ses.vei, ses.tot);
        printf("  %s\n\n", DIV);
        printf("  1. Enviar aviso ao controle\n");
        printf("  2. Ver historico da jornada\n");
        printf("  0. Encerrar jornada\n\n");
        printf("  Sua escolha: ");

        if (scanf("%d", &op) != 1) {
            while (getchar() != '\n');
            op = -1;
        }

        switch (op) {
            case 1: enviarAviso(); break;
            case 2: historico();   break;
            case 0: encerrar();    break;
            default:
                erro("Opcao invalida. Escolha 0, 1 ou 2.");
                pausar();
        }
    } while (op != 0);
}

int main()
{
    setlocale(LC_ALL, "");
    int op;

    do {
        cabec("BEM-VINDO AO SISTEMA BRT");
        printf("  Escolha o tipo de acesso:\n\n");
        printf("  1. Entrar como Administrador\n");
        printf("  2. Entrar como Motorista\n");
        printf("  0. Sair do sistema\n\n");
        printf("  Sua escolha: ");

        if (scanf("%d", &op) != 1) {
            while (getchar() != '\n');
            op = -1;
        }

        switch (op) {
            case 1:
                if (loginAdmin())
                    menuAdmin();
                break;
            case 2:
                loginMotorista();
                menuMotorista();
                break;
            case 0:
                printf("\n  Sistema encerrado. Ate logo!\n\n");
                break;
            default:
                erro("Opcao invalida. Digite 0, 1 ou 2.");
                pausar();
        }
    } while (op != 0);

    return 0;
}