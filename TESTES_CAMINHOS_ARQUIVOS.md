# 🧪 Testes: Verificando Localização de Arquivos

## Teste Rápido no Terminal

### 1. Verificar se Arquivos Existem

```bash
cd /workspaces/Sistema-de-controle-de-tr-fego-urbano-BRT-

# Listar arquivos de configuração
ls -la src/config/
# Saída esperada:
# -rw-r--r-- ... admin.txt
# -rw-r--r-- ... operadores.txt
```

### 2. Executar do Diretório Raiz (Deve Funcionar ✅)

```bash
cd /workspaces/Sistema-de-controle-de-tr-fego-urbano-BRT-
./src/output/main
```

**Saída esperada no início:**
```
[DEBUG] Iniciando busca de arquivos de configuração...
[OK] Encontrado: src/config/admin.txt
[OK] Encontrado: src/config/operadores.txt
[OK] Todos os arquivos de configuração encontrados
```

### 3. Executar de Outro Diretório (Antes Falhava ❌ → Agora Funciona ✅)

```bash
cd /tmp
/workspaces/Sistema-de-controle-de-tr-fego-urbano-BRT-/src/output/main
```

**Com a nova solução:** Funciona! 
(As 4 estratégias de busca encontram os arquivos)

### 4. Usando Variável de Ambiente

```bash
export CONFIG_PATH=/workspaces/Sistema-de-controle-de-tr-fego-urbano-BRT-/src/config
/tmp/main
```

**Resultado:** Funciona! (estratégia 3 de busca)

---

## Teste Detalhado com Debug

Se quiser ver exatamente qual caminho foi encontrado:

```bash
cd /workspaces/Sistema-de-controle-de-tr-fego-urbano-BRT-
./src/output/main 2>&1 | grep -E "DEBUG|OK|Encontrado"
```

**Saída esperada:**
```
[DEBUG] Iniciando busca de arquivos de configuração...
[OK] Encontrado: src/config/admin.txt
[OK] Encontrado: src/config/operadores.txt
[OK] Todos os arquivos de configuração encontrados
```

---

## Simulando Erro (Teste Negativo)

Se quiser testar o tratamento de erro, mude o arquivo temporariamente:

```bash
# Simular arquivo faltando
mv src/config/admin.txt src/config/admin.txt.bak

# Executar
./src/output/main
```

**Saída esperada:**
```
[DEBUG] Iniciando busca de arquivos de configuração...
❌ Arquivo não encontrado: admin.txt
   Locais procurados:
   • src/config/admin.txt
   • (relativo ao executável)/config/admin.txt
   • /workspaces/Sistema-de-controle-de-tr-fego-urbano-BRT-/src/config/admin.txt

[ERRO CRÍTICO] Não foi possível localizar arquivos de configuração.
Certifique-se que:
  • Os arquivos existem em src/config/
  • Você executou o programa a partir da raiz do projeto
  • Ou defina CONFIG_PATH=/caminho/para/config
```

**Restaurar:**
```bash
mv src/config/admin.txt.bak src/config/admin.txt
```

---

## Teste de Permissões

```bash
# Verificar permissões atuais
ls -la src/config/admin.txt
# -rw-r--r-- (644) é correto

# Se necessário, restaurar permissões
chmod 644 src/config/admin.txt
chmod 644 src/config/operadores.txt
```

---

## Verificar Conteúdo dos Arquivos

```bash
echo "=== admin.txt ==="
cat src/config/admin.txt

echo "=== operadores.txt ==="
cat src/config/operadores.txt
```

**Formato esperado:**
```
usuario:senha
admin:admin123
```

---

## Teste de Compilação

```bash
# Compilar com warnings
gcc -Wall -Wextra -g3 \
    -I/workspaces/Sistema-de-controle-de-tr-fego-urbano-BRT-/src/auth \
    /workspaces/Sistema-de-controle-de-tr-fego-urbano-BRT-/src/main.c \
    -o /workspaces/Sistema-de-controle-de-tr-fego-urbano-BRT-/src/output/main

# Verificar se compilou
echo "Status: $?"  # 0 = sucesso
ls -la src/output/main
```

---

## Teste de CWD

Para ver qual é o diretório de execução do seu programa:

```bash
# Adicione no seu código (em main(), após config_inicializar):
char cwd[256];
if (getcwd(cwd, sizeof(cwd))) {
    fprintf(stderr, "[DEBUG] CWD: %s\n", cwd);
}
```

Você verá algo como:
```
[DEBUG] CWD: /workspaces/Sistema-de-controle-de-tr-fego-urbano-BRT-
```

---

## Verificação com `strace` (Linux)

Para ver exatamente quais arquivos o programa tenta abrir:

```bash
strace -e openat ./src/output/main 2>&1 | grep -E "admin|operadores"
```

**Saída mostrará as tentativas de abertura:**
```
openat(AT_FDCWD, "src/config/admin.txt", O_RDONLY) = 3
```

Se retorna `-1`, o arquivo não foi encontrado naquele caminho.

---

## Checklist de Teste

- [ ] Arquivos existem: `ls -la src/config/`
- [ ] Permissões corretas: `644` (rw-r--r--)
- [ ] Compilou sem erros: `gcc ... -Wall -Wextra`
- [ ] Executa da raiz do projeto: ✅ Funciona
- [ ] Executa de outro diretório: ✅ Funciona (novo!)
- [ ] Exibe mensagens de debug: ✅ [OK] encontrado em...
- [ ] Conteúdo dos arquivos é válido: usuario:senha
- [ ] Login funciona com credenciais corretas

---

## Próximas Etapas

1. **Testar a função de login:**
   ```
   Usuário: [insira um do admin.txt]
   Senha: [insira a senha correspondente]
   ```

2. **Verificar mensagens de erro:**
   - Teste com credenciais erradas
   - Teste com arquivo faltando

3. **Integrar em pipeline CI/CD:**
   - Garantir que `src/config/` está no repositório
   - Definir `CONFIG_PATH` em variáveis de ambiente

---

## Troubleshooting

| Problema | Causa | Solução |
|----------|-------|---------|
| `[ERRO] admin.txt não encontrado` | CWD errado | Execute de `/projeto/` |
| `[ERRO] Arquivo nao encontrado` | Permissões | `chmod 644 src/config/*` |
| Compilação falha | Header ausente | Verifique `#include "config.h"` |
| Teste OK, login falha | Credenciais erradas | Verifique conteúdo do arquivo |
| Múltiplas execuções | Arquivo aberto | Use `fclose(f)` ao terminar |

