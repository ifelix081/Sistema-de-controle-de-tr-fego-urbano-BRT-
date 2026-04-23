# 📁 Guia: Resolvendo Problemas com `fopen()` em C

## 🎯 Seu Problema

```c
#define conf_path "src/config/admin.txt"
FILE *f = fopen(conf_path, "r");  // Retorna NULL
```

**Por que não funciona?** O caminho `"src/config/admin.txt"` é **relativo ao diretório de execução (CWD)**, não ao local do código.

---

## 📚 Como Caminhos Relativos Funcionam em C

### Diretório de Execução vs Diretório do Código

```
Estrutura do projeto:
/projeto/
├── src/
│   ├── main.c         ← O código está aqui
│   ├── config.h
│   └── config/
│       ├── admin.txt
│       └── operadores.txt
├── build/
└── CMakeLists.txt

Se você executar do /projeto/:
  Diretório de execução = /projeto
  ✅ fopen("src/config/admin.txt") funciona!

Se você executar do /projeto/src/:
  Diretório de execução = /projeto/src
  ❌ fopen("src/config/admin.txt") FALHA!
  (procura por /projeto/src/src/config/...)

Se você executar do /projeto/build/:
  ❌ fopen("src/config/admin.txt") FALHA!
  (procura por /projeto/build/src/config/...)
```

### Ver o Diretório de Execução

```c
#include <unistd.h>

char cwd[256];
if (getcwd(cwd, sizeof(cwd))) {
    printf("Executando de: %s\n", cwd);
}
```

---

## 🔍 Diagnosticando Erros com `perror()`

### Sem Diagnóstico ❌

```c
FILE *f = fopen("src/config/admin.txt", "r");
if (!f) {
    printf("Erro: arquivo não encontrado\n");
    return 0;
}
```

**Por que é ruim?** Você não sabe qual foi o erro real (ficheiro não existe, sem permissão, etc).

### Com Diagnóstico ✅

```c
#include <stdio.h>
#include <unistd.h>

FILE *f = fopen("src/config/admin.txt", "r");
if (!f) {
    fprintf(stderr, "❌ Erro ao abrir: src/config/admin.txt\n");
    perror("  Motivo");  // ← Mostra o erro do SO!
    
    // Diagnóstico adicional
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd))) {
        fprintf(stderr, "  CWD: %s\n", cwd);
    }
    return 0;
}
```

**Possíveis mensagens de `perror()`:**

```
Motivo: No such file or directory    ← arquivo não existe
Motivo: Permission denied             ← sem permissão de leitura
Motivo: Is a directory                ← você abriu um diretório
Motivo: Too many open files           ← muitos arquivos abertos
```

---

## ✅ 4 Estratégias para Localizar Arquivos

### Estratégia 1: Relativa ao CWD (mais comum em desenvolvimento)

```c
fopen("src/config/admin.txt", "r");
```

✅ **Quando usar:** Testes locais, desenvolvimento na IDE
❌ **Problema:** Quebra se o programa for movido ou executado de outro lugar

---

### Estratégia 2: Relativa ao Executável (mais robusta)

```c
#include <unistd.h>
#include <libgen.h>

char exe_path[256];
ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
if (len > 0) {
    exe_path[len] = '\0';
    // exe_path agora contém: /projeto/src/output/main
    
    char config_path[256];
    snprintf(config_path, sizeof(config_path), 
             "%s/../config/admin.txt",  // ← relativo ao exe
             dirname(exe_path));
    
    FILE *f = fopen(config_path, "r");
}
```

✅ **Quando usar:** Aplicações instaladas em `/usr/bin` ou distribuídas
✅ **Vantagem:** Funciona independentemente de onde o programa é executado

---

### Estratégia 3: Variável de Ambiente

```bash
# Antes de executar:
export CONFIG_PATH=/usr/local/etc/brt
./main
```

```c
const char *config_dir = getenv("CONFIG_PATH");
if (config_dir) {
    char path[256];
    snprintf(path, sizeof(path), "%s/admin.txt", config_dir);
    FILE *f = fopen(path, "r");
}
```

✅ **Quando usar:** Sistemas em produção com múltiplas instalações
✅ **Vantagem:** Flexibilidade máxima

---

### Estratégia 4: Diretório Home do Usuário

```c
const char *home = getenv("HOME");
if (home) {
    char path[256];
    snprintf(path, 256, "%s/.config/brt/admin.txt", home);
    FILE *f = fopen(path, "r");
}
```

✅ **Quando usar:** Dados por usuário (configs pessoais)

---

## 🚀 Solução Implementada: Busca Automática

O arquivo `config.h` implementa uma busca **em cascata** que tenta encontrar os arquivos em múltiplos locais:

```c
// Em config.h - função localizar_arquivo()
static int localizar_arquivo(const char *nome_arquivo, 
                             char *resultado, size_t tam)
{
    // 1️⃣ Tenta relativo ao CWD
    if (arquivo_existe("src/config/admin.txt")) return 1;
    
    // 2️⃣ Tenta relativo ao executável
    if (arquivo_existe("../config/admin.txt")) return 1;
    
    // 3️⃣ Tenta em CONFIG_PATH
    const char *config_env = getenv("CONFIG_PATH");
    if (config_env && arquivo_existe(config_env)) return 1;
    
    // 4️⃣ Tenta no home do usuário
    const char *home = getenv("HOME");
    if (home && arquivo_existe(home "/.config/brt/admin.txt")) return 1;
    
    return 0;  // Não encontrou em nenhum lugar
}
```

### Como Usar

```c
// No main():
ConfigPaths cfg = {0};
if (!config_inicializar(&cfg)) {
    fprintf(stderr, "Erro: arquivos não encontrados\n");
    return EXIT_FAILURE;
}

// As variáveis globais agora contêm os caminhos encontrados:
// g_admin_path
// g_operadores_path
```

---

## 🛠️ Boas Práticas em Produção

### 1. Sempre Verificar com `perror()`

```c
FILE *f = fopen(path, "r");
if (!f) {
    fprintf(stderr, "Erro ao abrir %s: ", path);
    perror("");  // Mostra errno em forma legível
    return -1;
}
```

### 2. Usar Caminhos Absolutos Quando Possível

```c
// ❌ Ruim - relativo
fopen("config/admin.txt", "r");

// ✅ Melhor - absoluto (conhecer o caminho)
fopen("/etc/brt/admin.txt", "r");

// ✅ Melhor - dinâmico
char path[256];
snprintf(path, sizeof(path), "%s/admin.txt", config_dir);
fopen(path, "r");
```

### 3. Logar o Caminho Tentado

```c
fprintf(stderr, "[DEBUG] Tentando abrir: %s\n", path);
FILE *f = fopen(path, "r");
if (!f) {
    perror("Erro");
    return -1;
}
```

### 4. Validar Permissões

```c
#include <sys/stat.h>

struct stat st;
if (stat(path, &st) != 0) {
    perror("Arquivo não existe");
    return -1;
}

if (!(st.st_mode & S_IRUSR)) {
    fprintf(stderr, "Sem permissão de leitura\n");
    return -1;
}

if (S_ISDIR(st.st_mode)) {
    fprintf(stderr, "É um diretório, não um arquivo\n");
    return -1;
}

FILE *f = fopen(path, "r");
```

---

## 📋 Checklist de Troubleshooting

Quando `fopen()` falha:

- [ ] **Arquivo realmente existe?** 
  ```bash
  ls -la src/config/admin.txt
  ```

- [ ] **Qual é o CWD atual?**
  ```c
  printf("CWD: %s\n", getcwd(NULL, 0));
  ```

- [ ] **Permissões corretas?**
  ```bash
  chmod 644 src/config/admin.txt
  ```

- [ ] **Caminho relativo está correto?**
  ```bash
  # Se está em /projeto/src/, o caminho deve ser ../config/admin.txt
  # Se está em /projeto/, o caminho deve ser src/config/admin.txt
  ```

- [ ] **Usar `perror()`?**
  ```c
  FILE *f = fopen(path, "r");
  if (!f) {
    perror("fopen");  // Mostra o erro específico
    return -1;
  }
  ```

- [ ] **Buffer aberto é excedido?**
  ```bash
  lsof | grep main | wc -l  # Ver quantos arquivos estão abertos
  ```

---

## 💡 Exemplos Práticos

### Exemplo 1: Sistema Multiplataforma

```c
#ifdef _WIN32
    const char *sep = "\\";
#else
    const char *sep = "/";
#endif

char path[256];
snprintf(path, sizeof(path), "src%sconfig%sadmin.txt", sep, sep);
FILE *f = fopen(path, "r");
```

### Exemplo 2: Suportar Variáveis de Ambiente

```c
char path[256];
const char *config_dir = getenv("BRT_CONFIG");

if (config_dir) {
    snprintf(path, sizeof(path), "%s/admin.txt", config_dir);
} else {
    snprintf(path, sizeof(path), "src/config/admin.txt");
}

FILE *f = fopen(path, "r");
if (!f) {
    fprintf(stderr, "Defina BRT_CONFIG ou execute da raiz do projeto\n");
    return -1;
}
```

### Exemplo 3: Log Completo

```c
void debug_arquivo(const char *path) {
    fprintf(stderr, "\n=== DEBUG ARQUIVO ===\n");
    fprintf(stderr, "Caminho: %s\n", path);
    
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)))
        fprintf(stderr, "CWD: %s\n", cwd);
    
    struct stat st;
    if (stat(path, &st) == 0) {
        fprintf(stderr, "Status: EXISTE ✓\n");
        fprintf(stderr, "Tamanho: %ld bytes\n", (long)st.st_size);
        fprintf(stderr, "Permissões: %o\n", (unsigned)st.st_mode);
    } else {
        fprintf(stderr, "Status: NÃO ENCONTRADO ✗\n");
        perror("Motivo");
    }
    fprintf(stderr, "==================\n\n");
}
```

---

## 🔗 Referências

- `man fopen` - Documentação oficial
- `man getcwd` - Obter diretório de execução
- `man readlink` - Obter caminho do executável
- `man perror` - Mensagens de erro do SO

---

## ✅ Resumo das Correções Aplicadas

Arquivo `main.c` foi atualizado com:

1. **Novo header `config.h`** com busca automática de arquivos
2. **Variáveis globais** `g_admin_path` e `g_operadores_path`
3. **Inicialização no `main()`** que busca e localiza os arquivos
4. **`perror()` integrado** para diagnóstico de erros
5. **Múltiplas estratégias** de busca para máxima compatibilidade

Agora execute sempre do diretório raiz do projeto:
```bash
cd /workspaces/Sistema-de-controle-de-tr-fego-urbano-BRT-
./src/output/main
```

Ou defina a variável de ambiente:
```bash
export CONFIG_PATH=/workspaces/Sistema-de-controle-de-tr-fego-urbano-BRT-/src/config
./qualquer/caminho/main
```
