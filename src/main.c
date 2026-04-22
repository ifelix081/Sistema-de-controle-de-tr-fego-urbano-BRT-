#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define SEP      "===================================================="
#define DIV      "----------------------------------------------------"
#define MAX      20
#define MAX_AV   200
#define MAX_RES  200
#define MAX_SES  100
#define conf_path "src/config/admin.conf"

#define COR_RESET  "\033[0m"
#define COR_VERM   "\033[31m"
#define COR_AMAR   "\033[33m"
#define COR_VERD   "\033[32m"
#define COR_CIAN   "\033[36m"
#define COR_BOLD   "\033[1m"

typedef struct {
    char user[8];
    char password[8];
} credencial;

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

typedef struct {
    char prot[20];
    char dth[25];
    char mot[50];
    char lin[20];
    char tip[50];
    char grav[20];
    char loc[100];
    char desc[200];
    int  respondido;
} AvisoCC;

typedef struct {
    char prot[20];
    char dth[25];
    char oper[50];
    char acao[200];
} Resposta;

typedef struct {
    char mat[15];
    char nom[50];
    char lin[20];
    char vei[20];
    char ini[25];
    char fim[25];
    int  ativo;
} SessaoCC;

typedef struct {
    char nom[50];
    char mat[15];
} Operador;

Sessao   ses;
Operador op_logado;

void limpar() { system("cls || clear"); }

void pausar(void)
{
    printf("\n  Pressione ENTER para continuar...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void dataHora(char *buf, int tam)
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(buf, tam, "%d/%m/%Y %H:%M:%S", tm);
}

void trim(char *s)
{
    if (!s || strlen(s) == 0) return;
    int i = 0, f = (int)strlen(s) - 1;
    while (isspace((unsigned char)s[i])) i++;
    while (f > i && isspace((unsigned char)s[f])) f--;
    if (i > f) { s[0] = '\0'; return; }   /* string era só espaços  */
    memmove(s, s + i, f - i + 1);
    s[f - i + 1] = '\0';
}

void cabec(const char *tit)
{
    limpar();
    printf("\n%s\n", SEP);
    printf("        SISTEMA BRT INTEGRADO\n");
    printf("%s\n", SEP);
    printf("  %s\n", tit);
    printf("%s\n\n", DIV);
}

void ok   (const char *m) { printf("\n  [OK] %s\n", m); }
void erro (const char *m) { printf("\n  [!]  %s\n", m); }
void aviso(const char *m) { printf("\n  [~]  %s\n", m); }

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

int placaOk(const char *p) { return strlen(p) >= 7; }

const char *corGrav(const char *g)
{
    if (strstr(g, "rgente")) return COR_VERM;
    if (strstr(g, "lta"))    return COR_VERM;
    if (strstr(g, "edia"))   return COR_AMAR;
    return COR_VERD;
}

int carregarAvisosCC(AvisoCC *lista, int max)
{
    FILE *f = fopen("avisos.csv", "r");
    if (!f) return 0; 
    char linha[600];
    int tot = 0;
    while (fgets(linha, sizeof(linha), f) && tot < max) {
        trim(linha);
        if (strlen(linha) == 0) continue;
        AvisoCC a;
        memset(&a, 0, sizeof(a));
        char *t = strtok(linha, ";");
        if (t) { strncpy(a.prot, t, 19); t = strtok(NULL, ";"); }
        if (t) { strncpy(a.dth,  t, 24); t = strtok(NULL, ";"); }
        if (t) { strncpy(a.mot,  t, 49); t = strtok(NULL, ";"); }
        if (t) { strncpy(a.lin,  t, 19); t = strtok(NULL, ";"); }
        if (t) { strncpy(a.tip,  t, 49); t = strtok(NULL, ";"); }
        if (t) { strncpy(a.grav, t, 19); t = strtok(NULL, ";"); }
        if (t) { strncpy(a.loc,  t, 99); t = strtok(NULL, ";"); }
        if (t) { strncpy(a.desc, t, 199); }
        a.respondido = 0;
        lista[tot++] = a;
    }
    fclose(f);
    return tot;
}

int carregarRespostas(Resposta *lista, int max)
{
    FILE *f = fopen("respostas.csv", "r");
    if (!f) return 0;
    char linha[400];
    int tot = 0;
    while (fgets(linha, sizeof(linha), f) && tot < max) {
        trim(linha);
        if (strlen(linha) == 0) continue;
        Resposta r;
        memset(&r, 0, sizeof(r));
        char *t = strtok(linha, ";");
        if (t) { strncpy(r.prot, t, 19); t = strtok(NULL, ";"); }
        if (t) { strncpy(r.dth,  t, 24); t = strtok(NULL, ";"); }
        if (t) { strncpy(r.oper, t, 49); t = strtok(NULL, ";"); }
        if (t) { strncpy(r.acao, t, 199); }
        lista[tot++] = r;
    }
    fclose(f);
    return tot;
}

void cruzarRespostas(AvisoCC *av, int totAv, Resposta *res, int totRes)
{
    for (int i = 0; i < totAv; i++)
        for (int j = 0; j < totRes; j++)
            if (strcmp(av[i].prot, res[j].prot) == 0) {
                av[i].respondido = 1;
                break;
            }
}

int carregarSessoesCC(SessaoCC *lista, int max)
{
    FILE *f = fopen("sessoes.csv", "r");
    if (!f) return 0;
    char linha[300];
    int tot = 0;
    while (fgets(linha, sizeof(linha), f) && tot < max) {
        trim(linha);
        if (strlen(linha) == 0) continue;
        SessaoCC s;
        memset(&s, 0, sizeof(s));
        char *t = strtok(linha, ";");
        if (t) { strncpy(s.mat, t, 14); t = strtok(NULL, ";"); }
        if (t) { strncpy(s.nom, t, 49); t = strtok(NULL, ";"); }
        if (t) { strncpy(s.lin, t, 19); t = strtok(NULL, ";"); }
        if (t) { strncpy(s.vei, t, 19); t = strtok(NULL, ";"); }
        if (t) { strncpy(s.ini, t, 24); t = strtok(NULL, ";"); }
        if (t) { strncpy(s.fim, t, 24); t = strtok(NULL, ";"); }
        if (t) { s.ativo = (strcmp(t, "ativo") == 0) ? 1 : 0; }
        lista[tot++] = s;
    }
    fclose(f);
    return tot;
}

void atualizarSessao(const char *mat, const char *fim)
{
    SessaoCC lista[MAX_SES];
    int tot = carregarSessoesCC(lista, MAX_SES);
    int atualizado = 0;

    for (int i = tot - 1; i >= 0; i--) {
        if (strcmp(lista[i].mat, mat) == 0 && lista[i].ativo) {
            lista[i].ativo = 0;
            strncpy(lista[i].fim, fim, 24);
            atualizado = 1;
            break;
        }
    }

    if (!atualizado) return;

    FILE *f = fopen("sessoes.csv", "w");
    if (!f) return;
    for (int i = 0; i < tot; i++) {
        fprintf(f, "%s;%s;%s;%s;%s;%s;%s\n",
                lista[i].mat, lista[i].nom,
                lista[i].lin, lista[i].vei,
                lista[i].ini, lista[i].fim,
                lista[i].ativo ? "ativo" : "encerrado");
    }
    fclose(f);
}


int lerCredenciais(credencial *cred) {
    // Monta o caminho baseado em onde o executável está rodando
    // Ajuste esse caminho para o seu projeto
    const char *caminho = "config/admin.conf";

    printf(" [debug] Procurando arquivo em: %s\n", caminho);

    FILE *f = fopen(caminho, "r");

    if (f == NULL) {
        printf(" Erro: '%s' nao encontrado.\n", caminho);
        printf(" Dica: rode o programa da pasta raiz do projeto.\n");
        return 0;  // retorna 0 = falhou
    }

    char linha[64];
    while (fgets(linha, sizeof(linha), f)) {
        if (linha[0] == '\n' || linha[0] == '#')
            continue;
        if (sscanf(linha, "usr=%19s", cred->user) == 1)
            continue;
        if (sscanf(linha, "sen=%19s", cred->password) == 1)
            continue;
    }

    fclose(f);

    // Verifica se realmente leu os dois campos
    if (cred->user[0] == '\0' || cred->password[0] == '\0') {
        printf(" Erro: arquivo .conf incompleto ou mal formatado.\n");
        return 0;
    }

    return 1;  // sucesso
}


int loginAdmin() {
    credencial cred;
    memset(&cred, 0, sizeof(cred));

    // ✅ SE FALHOU, PARA AQUI — não chega no strcmp
    if (!lerCredenciais(&cred)) {
        printf(" Encerrando login.\n");
        return 0;
    }

    char usr[8], sen[8];
    memset(usr, 0, 8);
    memset(sen, 0, 8);

    printf(" Usuario : ");
    scanf("%19s", usr);

    printf(" Senha   : ");
    scanf("%19s", sen);

    int resultado = (strcmp(usr, cred.user) == 0 &&
                     strcmp(sen, cred.password) == 0);

    memset(&cred, 0, sizeof(cred));  // apaga da RAM

    if (!resultado) {
        printf("\n Acesso negado!\n");
        return 0;
    }

    printf("\n Bem-vindo, Administrador!\n");
    return 1;
}

void cadOnibus(void)
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
        if (!placaOk(b.pla)) { erro("Placa invalida. Use pelo menos 7 caracteres."); tent++; }
    } while (!placaOk(b.pla) && tent < 3);
    if (tent >= 3) { aviso("Muitas tentativas invalidas. Cadastro cancelado."); pausar(); return; }

    printf("  Garagem            : "); scanf(" %49[^\n]", b.gar); trim(b.gar);
    if (vazio(b.gar, "Garagem")) { pausar(); return; }
    printf("  Motorista          : "); scanf(" %49[^\n]", b.mot); trim(b.mot);
    if (vazio(b.mot, "Motorista")) { pausar(); return; }
    printf("  Turno (manha/tarde/noite): "); scanf("%19s", b.tur);

    printf("\n  Resumo:\n  %s\n", DIV);
    printf("  Linha: %d | Onibus: %d | Placa: %s\n", b.lin, b.num, b.pla);
    printf("  Garagem: %s | Motorista: %s | Turno: %s\n", b.gar, b.mot, b.tur);
    printf("  %s\n", DIV);

    if (!confirmar("Confirmar o cadastro deste onibus?")) { aviso("Cadastro cancelado."); pausar(); return; }

    FILE *f = fopen("dados_onibus.csv", "a");
    if (!f) { erro("Nao foi possivel abrir o arquivo de dados."); pausar(); return; }
    fprintf(f, "%d;%d;%s;%s;%s;%s\n", b.lin, b.num, b.pla, b.gar, b.mot, b.tur);
    fclose(f);
    ok("Onibus cadastrado com sucesso!");
    pausar();
}

void cadParada(void)
{
    Parada p;
    cabec("CADASTRO DE PARADA");
    printf("  Codigo da parada: ");
    while (scanf("%d", &p.cod) != 1 || p.cod <= 0) {
        erro("Codigo invalido."); printf("  Codigo da parada: ");
        while (getchar() != '\n');
    }
    printf("  Endereco completo: "); scanf(" %99[^\n]", p.end); trim(p.end);
    if (vazio(p.end, "Endereco")) { pausar(); return; }

    printf("\n  Cod: %d | End: %s\n", p.cod, p.end);
    if (!confirmar("Confirmar?")) { aviso("Cancelado."); pausar(); return; }

    FILE *f = fopen("dados_paradas.csv", "a");
    if (!f) { erro("Erro ao acessar arquivo."); pausar(); return; }
    fprintf(f, "%d;%s\n", p.cod, p.end);
    fclose(f);
    ok("Parada cadastrada com sucesso!");
    pausar();
}

void cadMotorista(void)
{
    Motorista m;
    cabec("CADASTRO DE MOTORISTA");
    printf("  Nome completo   : "); scanf(" %49[^\n]", m.nom); trim(m.nom);
    if (vazio(m.nom, "Nome")) { pausar(); return; }
    do {
        printf("  CPF (so numeros): "); scanf("%14s", m.cpf);
        if (!cpfOk(m.cpf)) erro("CPF invalido. Digite 11 digitos.");
    } while (!cpfOk(m.cpf));
    printf("  Telefone        : "); scanf("%15s", m.tel);
    printf("  Linha de atuacao: ");
    while (scanf("%d", &m.lin) != 1 || m.lin <= 0) {
        erro("Linha invalida."); printf("  Linha de atuacao: ");
        while (getchar() != '\n');
    }

    printf("\n  Nome: %s | CPF: %s | Tel: %s | Linha: %d\n", m.nom, m.cpf, m.tel, m.lin);
    if (!confirmar("Confirmar?")) { aviso("Cancelado."); pausar(); return; }

    FILE *f = fopen("dados_motoristas.csv", "a");
    if (!f) { erro("Erro ao acessar arquivo."); pausar(); return; }
    fprintf(f, "%s;%s;%s;%d\n", m.nom, m.cpf, m.tel, m.lin);
    fclose(f);
    ok("Motorista cadastrado com sucesso!");
    pausar();
}

void cadLinha(void)
{
    Linha l;
    char cont;
    int tot = 0;
    cabec("CADASTRO DE LINHA");
    printf("  Codigo da linha: ");
    while (scanf("%d", &l.codLin) != 1 || l.codLin <= 0) {
        erro("Codigo invalido."); printf("  Codigo da linha: ");
        while (getchar() != '\n');
    }
    FILE *f = fopen("dados_linhas.csv", "a");
    if (!f) { erro("Erro ao acessar arquivo."); pausar(); return; }
    fprintf(f, "%d", l.codLin);
    printf("\n  Adicione as paradas desta linha.\n\n");
    do {
        printf("  Codigo da parada %d: ", tot + 1);
        while (scanf("%d", &l.codPar) != 1 || l.codPar <= 0) {
            erro("Codigo invalido.");
            printf("  Codigo da parada %d: ", tot + 1);
            while (getchar() != '\n');
        }
        fprintf(f, ";%d", l.codPar);
        tot++;
        printf("  Adicionar mais uma parada? (s/n): ");
        scanf(" %c", &cont);
    } while (cont == 's' || cont == 'S');
    fprintf(f, "\n");
    fclose(f);
    printf("\n  Linha %d cadastrada com %d parada(s).\n", l.codLin, tot);
    ok("Linha registrada com sucesso!");
    pausar();
}

void exibirArq(const char *arq, const char *tit)
{
    char lin[300];
    int tot = 0;
    FILE *f = fopen(arq, "r");
    printf("\n  %s--- %s ---%s\n", COR_BOLD, tit, COR_RESET);
    if (!f) { printf("  (nenhum registro encontrado)\n"); return; }
    while (fgets(lin, sizeof(lin), f)) { printf("  %s", lin); tot++; }
    fclose(f);
    if (tot == 0) printf("  (arquivo vazio)\n");
    else printf("  Total: %d registro(s)\n", tot);
}

void relatorio(void)
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
    exibirArq("respostas.csv",        "RESPOSTAS DO CONTROLE");
    printf("\n%s\n", DIV);
    pausar();
}

void menuAdmin(void)
{
    int op;
    do {
        cabec("PAINEL DO ADMINISTRADOR");
        printf("  1. Cadastrar novo onibus\n");
        printf("  2. Cadastrar nova parada\n");
        printf("  3. Cadastrar novo motorista\n");
        printf("  4. Cadastrar nova linha\n");
        printf("  5. Ver relatorio geral\n");
        printf("  0. Voltar ao menu principal\n\n");
        printf("  Sua escolha: ");
        if (scanf("%d", &op) != 1) { while (getchar() != '\n'); op = -1; }
        switch (op) {
            case 1: cadOnibus();    break;
            case 2: cadParada();    break;
            case 3: cadMotorista(); break;
            case 4: cadLinha();     break;
            case 5: relatorio();    break;
            case 0: break;
            default: erro("Opcao invalida."); pausar();
        }
    } while (op != 0);
}

void loginMotorista(void)
{
    cabec("LOGIN — MOTORISTA");
    printf("  Ola! Identifique-se para iniciar sua jornada:\n\n");
    printf("  Matricula : "); scanf("%14s", ses.mat);
    printf("  Seu nome  : "); scanf(" %49[^\n]", ses.nom); trim(ses.nom);
    printf("  Linha     : "); scanf("%19s", ses.lin);
    printf("  Veiculo   : "); scanf("%19s", ses.vei);

    dataHora(ses.ini, sizeof(ses.ini));
    ses.tot   = 0;
    ses.ativo = 1;

    FILE *fs = fopen("sessoes.csv", "a");
    if (fs) {
        fprintf(fs, "%s;%s;%s;%s;%s;;ativo\n",
                ses.mat, ses.nom, ses.lin, ses.vei, ses.ini);
        fclose(fs);
    }

    printf("\n%s\n", DIV);
    printf("  %sSESSAO INICIADA COM SUCESSO%s\n", COR_VERD, COR_RESET);
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

void enviarAviso(void)
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
    snprintf(a->prot, sizeof(a->prot), "AV-%.8s-%04d", ses.mat, rand() % 10000);

    printf("  Tipo do problema\n");
    printf("  (ex: Mecanico, Passageiro, Via, Atraso, Outro)\n");
    printf("  > "); scanf(" %49[^\n]", a->tip); trim(a->tip);
    if (vazio(a->tip, "Tipo do problema")) { pausar(); return; }

    printf("\n  Gravidade (Baixa / Media / Alta / Urgente)\n");
    printf("  > "); scanf(" %19[^\n]", a->grav);

    printf("\n  Local exato onde ocorreu:\n");
    printf("  > "); scanf(" %99[^\n]", a->loc); trim(a->loc);

    printf("\n  Descreva o que aconteceu:\n");
    printf("  > "); scanf(" %199[^\n]", a->desc); trim(a->desc);

    printf("\n  %s\n  RESUMO DO AVISO\n  %s\n", DIV, DIV);
    printf("  Protocolo : %s\n", a->prot);
    printf("  Data/Hora : %s\n", a->dth);
    printf("  Motorista : %s (linha %s)\n", ses.nom, ses.lin);
    printf("  Tipo      : %s\n", a->tip);
    printf("  Gravidade : %s%s%s\n", corGrav(a->grav), a->grav, COR_RESET);
    printf("  Local     : %s\n", a->loc);
    printf("  Descricao : %s\n", a->desc);
    printf("  %s\n", DIV);

    if (!confirmar("Confirmar o envio deste aviso?")) {
        aviso("Aviso nao enviado."); pausar(); return;
    }

    FILE *f = fopen("avisos.csv", "a");
    if (!f) { erro("Nao foi possivel registrar o aviso."); pausar(); return; }
    fprintf(f, "%s;%s;%s;%s;%s;%s;%s;%s\n",
            a->prot, a->dth, ses.nom, ses.lin,
            a->tip, a->grav, a->loc, a->desc);
    fclose(f);

    ses.tot++;
    printf("\n  Protocolo gerado: %s%s%s\n", COR_CIAN, a->prot, COR_RESET);
    ok("Aviso enviado ao controle central com sucesso!");
    pausar();
}

void verRespostas(void)
{
    cabec("RESPOSTAS DO CONTROLE CENTRAL");
    printf("  Motorista : %s | Jornada iniciada: %s\n\n", ses.nom, ses.ini);

    if (ses.tot == 0) {
        aviso("Voce nao enviou nenhum aviso nesta jornada.");
        pausar();
        return;
    }

    Resposta res[MAX_RES];
    int totRes = carregarRespostas(res, MAX_RES);
    int alguma = 0;

    for (int i = 0; i < ses.tot; i++) {
        printf("  %s\n", DIV);
        printf("  Aviso #%d | Protocolo: %s%s%s\n",
               i + 1, COR_CIAN, ses.av[i].prot, COR_RESET);
        printf("  Tipo: %s | Gravidade: %s%s%s\n",
               ses.av[i].tip, corGrav(ses.av[i].grav), ses.av[i].grav, COR_RESET);

        int respondido = 0;
        for (int j = 0; j < totRes; j++) {
            if (strcmp(ses.av[i].prot, res[j].prot) == 0) {
                printf("  %sResposta recebida:%s\n", COR_VERD, COR_RESET);
                printf("  Operador  : %s\n", res[j].oper);
                printf("  Em        : %s\n", res[j].dth);
                printf("  Instrucao : %s\n", res[j].acao);
                respondido = 1;
                alguma = 1;
                break;
            }
        }
        if (!respondido)
            printf("  %sAguardando resposta do controle...%s\n", COR_AMAR, COR_RESET);
    }

    printf("\n  %s\n", DIV);
    if (!alguma)
        aviso("Nenhum dos seus avisos foi respondido ainda. Tente novamente em breve.");

    pausar();
}

void historico(void)
{
    cabec("HISTORICO DA JORNADA");
    printf("  Motorista : %s\n", ses.nom);
    printf("  Inicio    : %s\n", ses.ini);
    printf("  Avisos    : %d registrado(s)\n\n", ses.tot);
    if (ses.tot == 0) {
        printf("  Nenhum aviso foi enviado nesta jornada.\n");
    } else {
        for (int i = 0; i < ses.tot; i++) {
            printf("  %s\n  Aviso #%d\n", DIV, i + 1);
            printf("  Protocolo : %s\n", ses.av[i].prot);
            printf("  Horario   : %s\n", ses.av[i].dth);
            printf("  Tipo      : %s | Gravidade: %s%s%s\n",
                   ses.av[i].tip, corGrav(ses.av[i].grav), ses.av[i].grav, COR_RESET);
            printf("  Local     : %s\n", ses.av[i].loc);
        }
        printf("  %s\n", DIV);
    }
    pausar();
}

void encerrar(void)
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

    atualizarSessao(ses.mat, fim);

    ses.ativo = 0;
    printf("\n  Bom descanso, %s! Ate o proximo turno.\n", ses.nom);
    pausar();
}

void menuMotorista(void)
{
    int op;
    do {
        cabec("PAINEL DO MOTORISTA");
        printf("  Ola, %s!\n", ses.nom);
        printf("  Linha: %-10s | Veiculo: %-10s | Avisos: %d\n\n",
               ses.lin, ses.vei, ses.tot);
        printf("  %s\n\n", DIV);
        printf("  1. Enviar aviso ao controle\n");
        printf("  2. Ver respostas do controle\n");
        printf("  3. Ver historico da jornada\n");
        printf("  0. Encerrar jornada\n\n");
        printf("  Sua escolha: ");
        if (scanf("%d", &op) != 1) { while (getchar() != '\n'); op = -1; }
        switch (op) {
            case 1: enviarAviso();  break;
            case 2: verRespostas(); break;
            case 3: historico();    break;
            case 0: encerrar();     break;
            default: erro("Opcao invalida. Escolha entre 0 e 3."); pausar();
        }
    } while (op != 0);
}

int loginOperador(void)
{
    char usr[20], sen[20];
    char fileUsr[20], fileSen[20];
    char linha[200];
    int  autenticado = 0;

    FILE *f = fopen("credenciais_operadores.txt", "r");
    if (!f) {
        erro("Arquivo de credenciais de operadores nao encontrado.");
        aviso("Crie o arquivo 'credenciais_operadores.txt' com o formato:");
        printf("  usuario:senha\n");
        pausar();
        return 0;
    }

    cabec("LOGIN — OPERADOR DE CONTROLE");
    printf("  Matricula : "); scanf("%14s", usr);
    printf("  Senha     : "); scanf("%19s", sen);

    while (fgets(linha, sizeof(linha), f)) {
        if (sscanf(linha, "%19[^:]:%19s", fileUsr, fileSen) != 2) continue;

        if (strcmp(usr, fileUsr) == 0) {
            if (strcmp(sen, fileSen) == 0)
                autenticado = 1;
            break;
        }
    }
    fclose(f);

    if (autenticado) {
        strncpy(op_logado.mat, usr, 14);
        snprintf(op_logado.nom, sizeof(op_logado.nom), "Operador %s", usr);
        ok("Acesso liberado!");
        printf("  Operador: %s\n", op_logado.nom);
        pausar();
        return 1;
    }

    erro("Credenciais invalidas. Acesso negado.");
    pausar();
    return 0;
}

void exibirAvisoCC(const AvisoCC *a, int idx, int mostrarStatus)
{
    printf("\n  %s\n", DIV);
    printf("  %s[AVISO #%d]%s  Protocolo: %s%s%s\n",
           COR_BOLD, idx, COR_RESET, COR_CIAN, a->prot, COR_RESET);
    if (mostrarStatus)
        printf("  Status    : %s%s%s\n",
               a->respondido ? COR_VERD : COR_VERM,
               a->respondido ? "RESPONDIDO" : "PENDENTE",
               COR_RESET);
    printf("  Data/Hora : %s\n", a->dth);
    printf("  Motorista : %s  (Linha %s)\n", a->mot, a->lin);
    printf("  Tipo      : %s\n", a->tip);
    printf("  Gravidade : %s%s%s\n", corGrav(a->grav), a->grav, COR_RESET);
    printf("  Local     : %s\n", a->loc);
    printf("  Descricao : %s\n", a->desc);
}

void verPendentes(void)
{
    AvisoCC  av[MAX_AV];
    Resposta res[MAX_RES];
    int totAv  = carregarAvisosCC(av, MAX_AV);
    int totRes = carregarRespostas(res, MAX_RES);
    cruzarRespostas(av, totAv, res, totRes);

    cabec("AVISOS PENDENTES");
    int pend = 0;
    for (int i = 0; i < totAv; i++) if (!av[i].respondido) pend++;

    if (totAv == 0) { aviso("Nenhum aviso registrado ainda."); pausar(); return; }

    printf("  Total: %d  |  Pendentes: %s%d%s  |  Respondidos: %d\n",
           totAv, pend > 0 ? COR_VERM : COR_VERD, pend, COR_RESET, totAv - pend);

    if (pend == 0) { ok("Todos os avisos ja foram respondidos."); pausar(); return; }

    for (int i = 0; i < totAv; i++)
        if (!av[i].respondido)
            exibirAvisoCC(&av[i], i + 1, 0);

    printf("\n  %s\n", DIV);
    pausar();
}

void responderAviso(void)
{
    AvisoCC  av[MAX_AV];
    Resposta res[MAX_RES];
    int totAv  = carregarAvisosCC(av, MAX_AV);
    int totRes = carregarRespostas(res, MAX_RES);
    cruzarRespostas(av, totAv, res, totRes);

    cabec("RESPONDER AVISO");

    if (totAv == 0) { aviso("Nenhum aviso disponivel."); pausar(); return; }

    int pend = 0;
    printf("  Avisos sem resposta:\n\n");
    for (int i = 0; i < totAv; i++) {
        if (!av[i].respondido) {
            printf("  [%d] %s%s%s — %s — %s%s%s — %s\n",
                   i + 1, COR_CIAN, av[i].prot, COR_RESET,
                   av[i].mot, corGrav(av[i].grav), av[i].grav, COR_RESET,
                   av[i].tip);
            pend++;
        }
    }

    if (pend == 0) { ok("Todos os avisos ja foram respondidos."); pausar(); return; }

    printf("\n  Numero do aviso a responder: ");
    int escolha;
    if (scanf("%d", &escolha) != 1 || escolha < 1 || escolha > totAv) {
        erro("Numero invalido."); pausar(); return;
    }

    AvisoCC *alvo = &av[escolha - 1];
    if (alvo->respondido) { aviso("Este aviso ja possui resposta."); pausar(); return; }

    exibirAvisoCC(alvo, escolha, 0);

    printf("\n%s\n  INSTRUCAO AO MOTORISTA\n%s\n\n", DIV, DIV);
    printf("  A) Acionar equipe de manutencao\n");
    printf("  B) Aguardar no local — suporte a caminho\n");
    printf("  C) Solicitar onibus reserva\n");
    printf("  D) Acionar policia / SAMU\n");
    printf("  E) Registrar e monitorar remotamente\n");
    printf("  F) Liberar rota alternativa\n");
    printf("  G) Digitar instrucao personalizada\n\n");
    printf("  Sua escolha (A-G): ");

    char letra;
    scanf(" %c", &letra);
    letra = toupper((unsigned char)letra);

    Resposta r;
    memset(&r, 0, sizeof(r));
    strncpy(r.prot, alvo->prot, 19);
    dataHora(r.dth, sizeof(r.dth));
    strncpy(r.oper, op_logado.nom, 49);

    switch (letra) {
        case 'A': strncpy(r.acao, "Equipe de manutencao acionada. Aguarde no local seguro.", 199); break;
        case 'B': strncpy(r.acao, "Permaneca parado em local seguro. Suporte a caminho.", 199); break;
        case 'C': strncpy(r.acao, "Onibus reserva em deslocamento. ETA aproximado: 15 min.", 199); break;
        case 'D': strncpy(r.acao, "Servicos de emergencia acionados. Isole a area e aguarde.", 199); break;
        case 'E': strncpy(r.acao, "Ocorrencia registrada. Continue o percurso normalmente.", 199); break;
        case 'F': strncpy(r.acao, "Rota alternativa liberada. Desvie pelo corredor secundario.", 199); break;
        case 'G':
            printf("\n  Digite a instrucao:\n  > ");
            scanf(" %199[^\n]", r.acao); trim(r.acao);
            if (strlen(r.acao) == 0) { erro("Instrucao vazia. Cancelado."); pausar(); return; }
            break;
        default:
            erro("Opcao invalida. Cancelado."); pausar(); return;
    }

    printf("\n  %s\n  Protocolo : %s\n  Operador  : %s\n  Instrucao : %s\n  %s\n",
           DIV, r.prot, r.oper, r.acao, DIV);

    if (!confirmar("Confirmar o envio desta resposta?")) {
        aviso("Resposta cancelada."); pausar(); return;
    }

    FILE *f = fopen("respostas.csv", "a");
    if (!f) { erro("Falha ao gravar resposta."); pausar(); return; }
    fprintf(f, "%s;%s;%s;%s\n", r.prot, r.dth, r.oper, r.acao);
    fclose(f);

    printf("\n  %sResposta registrada para %s%s\n", COR_VERD, r.prot, COR_RESET);
    ok("Instrucao enviada ao motorista com sucesso!");
    pausar();
}

void verJornadas(void)
{
    SessaoCC ses_lista[MAX_SES];
    int tot = carregarSessoesCC(ses_lista, MAX_SES);

    cabec("MOTORISTAS EM JORNADA");

    if (tot == 0) {
        aviso("Nenhuma jornada registrada ainda.");
        printf("  As jornadas aparecem aqui assim que os motoristas fazem login.\n");
        pausar();
        return;
    }

    int ativos = 0, encerrados = 0;
    for (int i = 0; i < tot; i++)
        if (ses_lista[i].ativo) ativos++; else encerrados++;

    printf("  %sAtivos: %d%s  |  Encerrados: %d  |  Total: %d\n\n",
           COR_BOLD, ativos, COR_RESET, encerrados, tot);

    printf("  %sJORNADAS ATIVAS%s\n  %s\n", COR_VERD, COR_RESET, DIV);
    int algumAtivo = 0;
    for (int i = 0; i < tot; i++) {
        if (!ses_lista[i].ativo) continue;
        algumAtivo = 1;
        printf("  Mat: %-12s | Nome: %-25s\n", ses_lista[i].mat, ses_lista[i].nom);
        printf("  Linha: %-8s | Veiculo: %-10s | Inicio: %s\n\n",
               ses_lista[i].lin, ses_lista[i].vei, ses_lista[i].ini);
    }
    if (!algumAtivo) printf("  Nenhum motorista ativo no momento.\n\n");

    printf("  %sJORNADAS ENCERRADAS%s\n  %s\n", COR_AMAR, COR_RESET, DIV);
    int algumEnc = 0;
    for (int i = 0; i < tot; i++) {
        if (ses_lista[i].ativo) continue;
        algumEnc = 1;
        printf("  %-14s | %-25s | Linha: %-6s | Fim: %s\n",
               ses_lista[i].mat, ses_lista[i].nom,
               ses_lista[i].lin, ses_lista[i].fim);
    }
    if (!algumEnc) printf("  Nenhuma jornada encerrada registrada.\n");

    printf("\n  %s\n", DIV);
    pausar();
}

void historicoCompletoCC(void)
{
    AvisoCC  av[MAX_AV];
    Resposta res[MAX_RES];
    int totAv  = carregarAvisosCC(av, MAX_AV);
    int totRes = carregarRespostas(res, MAX_RES);
    cruzarRespostas(av, totAv, res, totRes);

    cabec("HISTORICO COMPLETO DE AVISOS");

    if (totAv == 0) { aviso("Nenhum aviso registrado."); pausar(); return; }

    printf("  Total: %d aviso(s)  |  Com resposta: %d\n\n", totAv, totRes);

    for (int i = 0; i < totAv; i++) {
        exibirAvisoCC(&av[i], i + 1, 1);
        for (int j = 0; j < totRes; j++) {
            if (strcmp(av[i].prot, res[j].prot) == 0) {
                printf("  %s>>> RESPOSTA:%s Operador: %s | Em: %s\n",
                       COR_VERD, COR_RESET, res[j].oper, res[j].dth);
                printf("  Instrucao : %s\n", res[j].acao);
                break;
            }
        }
    }
    printf("\n  %s\n", DIV);
    pausar();
}

void buscarProtocolo(void)
{
    AvisoCC  av[MAX_AV];
    Resposta res[MAX_RES];
    int totAv  = carregarAvisosCC(av, MAX_AV);
    int totRes = carregarRespostas(res, MAX_RES);
    cruzarRespostas(av, totAv, res, totRes);

    cabec("BUSCAR POR PROTOCOLO");
    printf("  Protocolo (ex: AV-0042-0713): ");
    char prot[20];
    scanf("%19s", prot);

    int achou = 0;
    for (int i = 0; i < totAv; i++) {
        if (strcmp(av[i].prot, prot) == 0) {
            exibirAvisoCC(&av[i], i + 1, 1);
            for (int j = 0; j < totRes; j++) {
                if (strcmp(av[i].prot, res[j].prot) == 0) {
                    printf("  %s>>> RESPOSTA:%s %s | Em: %s\n  Instrucao: %s\n",
                           COR_VERD, COR_RESET, res[j].oper, res[j].dth, res[j].acao);
                    break;
                }
            }
            achou = 1;
            break;
        }
    }
    if (!achou) erro("Protocolo nao encontrado.");
    printf("\n  %s\n", DIV);
    pausar();
}

void menuControle(void)
{
    int op;
    char dth[25];
    do {
        dataHora(dth, sizeof(dth));
        cabec("PAINEL DE CONTROLE CENTRAL");
        printf("  Operador : %s%s%s | %s\n\n", COR_BOLD, op_logado.nom, COR_RESET, dth);
        printf("  %s\n\n", DIV);
        printf("  1. Ver avisos %sPENDENTES%s\n",        COR_VERM, COR_RESET);
        printf("  2. Responder aviso de motorista\n");
        printf("  3. Verificar motoristas em jornada\n");
        printf("  4. Historico completo de avisos\n");
        printf("  5. Buscar aviso por protocolo\n");
        printf("  0. Voltar ao menu principal\n\n");
        printf("  Sua escolha: ");
        if (scanf("%d", &op) != 1) { while (getchar() != '\n'); op = -1; }
        switch (op) {
            case 1: verPendentes();         break;
            case 2: responderAviso();       break;
            case 3: verJornadas();          break;
            case 4: historicoCompletoCC();  break;
            case 5: buscarProtocolo();      break;
            case 0: break;
            default: erro("Opcao invalida."); pausar();
        }
    } while (op != 0);
}

int main(void)
{
    srand((unsigned int)time(NULL));   /* único ponto de semente — FIX #4 */
    setlocale(LC_ALL, "");
    int op;

    do {
        cabec("BEM-VINDO AO SISTEMA BRT INTEGRADO");
        printf("  Escolha o tipo de acesso:\n\n");
        printf("  1. %sAdministrador%s\n",       COR_BOLD, COR_RESET);
        printf("  2. %sMotorista%s\n",            COR_BOLD, COR_RESET);
        printf("  3. %sControle Central%s\n",     COR_BOLD, COR_RESET);
        printf("  0. Sair do sistema\n\n");
        printf("  Sua escolha: ");

        if (scanf("%d", &op) != 1) { while (getchar() != '\n'); op = -1; }

        switch (op) {
            case 1:
                if (loginAdmin())
                    menuAdmin();
                break;
            case 2:
                loginMotorista();
                menuMotorista();
                break;
            case 3: {
                int tent = 0;
                while (tent < 3) {
                    if (loginOperador()) { menuControle(); break; }
                    tent++;
                    if (tent < 3)
                        printf("\n  Tentativa %d/3.\n", tent);
                }
                if (tent >= 3)
                    erro("Numero maximo de tentativas atingido.");
                break;
            }
            case 0: {
                printf("\n  Salvando dados no banco...\n");
                int ret = system("python src/output/db_import.py");
                if (ret != 0) {
                    printf("  %s[!] Aviso: db_import.py encerrou com codigo %d."
                           " Verifique os logs.%s\n",
                           COR_AMAR, ret, COR_RESET);
                }
                printf("  Sistema encerrado. Ate logo!\n\n");
                break;
            }
            default:
                erro("Opcao invalida. Digite 0, 1, 2 ou 3.");
                pausar();
        }
    } while (op != 0);

    return 0;
}