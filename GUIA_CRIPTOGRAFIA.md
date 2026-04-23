# 🔐 Sistema de Criptografia de Senhas - BRT

## 📋 Visão Geral

Este sistema implementa **criptografia AES-256-CBC** para proteger os arquivos de configuração contendo senhas:

```
┌─────────────────────────────────────┐
│ Arquivo Original (TEXTO):           │
│ admin:senha123                      │
│ operador:op123                      │
├─────────────────────────────────────┤
│           ENCRIPTA COM ↓             │
│     python3 crypto_util.py encrypt   │
│           senha_mestre              │
├─────────────────────────────────────┤
│ Arquivo Criptografado (.enc):       │
│ BRT_ENC_V1|R4NPmL9K...|7x3Qw...    │
│ [ilegível sem senha_mestre]         │
└─────────────────────────────────────┘
```

---

## 🛠️ Quickstart (5 min)

### 1. Instalar Dependências

```bash
# Python (para utilitário)
pip install cryptography

# OpenSSL (para compilação C)
sudo apt-get install libssl-dev
```

### 2. Preparar Arquivos

```bash
cd /workspaces/Sistema-de-controle-de-tr-fego-urbano-BRT-

# Verificar arquivos existentes
ls -la src/config/
# admin.txt
# operadores.txt
```

### 3. Criptografar Senhas

```bash
# Criptografar admin.txt
python3 crypto_util.py encrypt src/config/admin.txt
# Pede: senha_mestre_123
# Cria: src/config/admin.txt.enc

# Criptografar operadores.txt
python3 crypto_util.py encrypt src/config/operadores.txt
# Pede: senha_mestre_123
# Cria: src/config/operadores.txt.enc
```

### 4. Remover Originais (Importante!)

```bash
# Depois de confirmar que .enc foi criado:
rm src/config/admin.txt src/config/operadores.txt

# Verificar
ls -la src/config/
# admin.txt.enc (criptografado)
# operadores.txt.enc (criptografado)
```

### 5. Adicionar ao .gitignore

```bash
cat >> .gitignore << 'EOF'
# Senhas (criptografadas e descriptografadas)
src/config/admin.txt
src/config/operadores.txt
src/config/*.txt
src/config/*.txt.enc
EOF

git add .gitignore
git commit -m "Security: protect encrypted password files"
```

---

## 🔄 Como Funciona

### Estrutura de um Arquivo Criptografado

```
BRT_ENC_V1|SALT_BASE64|CIPHERTEXT_BASE64

Componentes:
├─ BRT_ENC_V1     : Versão do formato
├─ SALT_BASE64    : Salt aleatório (derivação de chave)
└─ CIPHERTEXT_BASE64 : Dados criptografados com AES-256
```

### Processo de Criptografia

```
Arquivo: admin.txt
Conteúdo: admin:senha123

        ↓ [python3 crypto_util.py encrypt]
        
Pede senha_mestre (ex: "chave_secreta_123")

        ↓ [PBKDF2: 100.000 iterações]
        
Gera chave de 256 bits e salt aleatório

        ↓ [AES-256-CBC]
        
Criptografa: admin:senha123 → 8xD3kP9...

        ↓
        
Resultado: admin.txt.enc
BRT_ENC_V1|R4N...Pm|7x3...Qw
```

### Processo de Descriptografia (em Tempo de Execução)

```
Programa BRT inicia

        ↓
        
Procura: admin.txt.enc

        ↓
        
Pede ao usuário (ou variável de ambiente):
"🔐 Senha mestre: "

        ↓ [PBKDF2 com salt armazenado]
        
Regenera chave 256 bits

        ↓ [AES-256-CBC]
        
Descriptografa: BRT_ENC_V1|R4N...|7x3...Qw → admin:senha123

        ↓
        
Usa credenciais em memória (não grava em disco)
```

---

## 📊 Segurança Implementada

### ✅ O Que Protege

| Nível | Proteção | Implementado |
|-------|----------|--------------|
| **Armazenamento** | AES-256-CBC | ✅ Criptografia de ponta |
| **Derivação** | PBKDF2 100k iterações | ✅ Contra força bruta |
| **Aleatoriedade** | Salt único por arquivo | ✅ 16 bytes aleatórios |
| **Integridade** | Fernet (autenticação) | ✅ Detecta corrupção |
| **Não-Repetição** | IV derivado de chave | ✅ Determinístico e seguro |

### 🔒 Características Específicas

**PBKDF2 (Password-Based Key Derivation Function 2)**
- 100.000 iterações (resistente a força bruta)
- Função hash: SHA-256
- Salt: 16 bytes aleatórios

**AES-256-CBC (Advanced Encryption Standard)**
- Chave: 256 bits (32 bytes)
- Modo: CBC (Cipher Block Chaining)
- IV (Initialization Vector): 16 bytes

**Fernet (Symmetric Encryption)**
- Autenticação: HMAC-SHA256
- Garante integridade e autenticidade
- Detecta arquivos corrompidos

---

## 🚀 Modo de Uso Completo

### Desenvolvimento Local

```bash
# 1. Ter arquivo de texto com senhas de teste
cat src/config/admin.txt
# admin:test123

# 2. Criptografar
python3 crypto_util.py encrypt src/config/admin.txt
# Digite: senha_desenvolvimento

# 3. Remover original
rm src/config/admin.txt

# 4. Executar programa
# O programa pedirá a senha_mestre ao iniciaar
./src/output/main
# 🔐 Senha mestre: [digite: senha_desenvolvimento]
# ✅ Verificado admin:test123
```

### Produção

```bash
# 1. Criar arquivo com senhas reais
sudo nano /etc/brt/admin.txt
# admin:senha_real_muito_secreta

# 2. Criptografar
sudo python3 crypto_util.py encrypt /etc/brt/admin.txt
# Digite: chave_producao_ultra_secreta

# 3. Remover original
sudo rm /etc/brt/admin.txt

# 4. Definir permissões restritivas
sudo chmod 600 /etc/brt/admin.txt.enc
sudo chown root:root /etc/brt/admin.txt.enc

# 5. Passar senha via variável de ambiente ou arquivo seguro
export BRT_ADMIN_MASTER_PASSWORD="chave_producao_ultra_secreta"
./main

# Alternativa: arquivo .env seguro
cat /etc/brt/.env.secure
# BRT_ADMIN_MASTER_PASSWORD="chave_producao_ultra_secreta"
source /etc/brt/.env.secure
./main
```

---

## 🔧 Utilitário `crypto_util.py`

### Comandos Disponíveis

**1. Criptografar**
```bash
python3 crypto_util.py encrypt <arquivo>

Exemplo:
  python3 crypto_util.py encrypt src/config/admin.txt
  
Resultado:
  ✅ Arquivo criptografado com sucesso!
     Original:       src/config/admin.txt
     Criptografado:  src/config/admin.txt.enc
     Permissões:     600 (rw------)
     Tamanho:        15 → 128 bytes
```

**2. Descriptografar**
```bash
python3 crypto_util.py decrypt <arquivo.enc>

Exemplo:
  python3 crypto_util.py decrypt src/config/admin.txt.enc
  
Resultado:
  ✅ Arquivo descriptografado com sucesso!
     Criptografado:  src/config/admin.txt.enc
     Original:       src/config/admin.txt
     Permissões:     600 (rw------)
     Tamanho:        128 → 15 bytes
```

**3. Ver Informações**
```bash
python3 crypto_util.py show <arquivo.enc>

Exemplo:
  python3 crypto_util.py show src/config/admin.txt.enc
  
Resultado:
  📄 Informações do Arquivo Criptografado
  ==================================================
  Arquivo:        src/config/admin.txt.enc
  Tamanho:        128 bytes
  Versão:         BRT_ENC_V1
  Permissões:     600
  Salt (hex):     f3b2e1c4d5a6b7c8f3b2e1c4d5a6b7c8...
  Ciphertext:     96 bytes criptografados
  ==================================================
```

---

## 💻 Usando em Código C

### Exemplo 1: Carregar Senhas Criptografadas

```c
#include "src/config_crypto.h"

int main(void) {
    Credencial admin;
    
    // Prompt para senha mestre
    char senha_mestre[100];
    printf("🔐 Senha mestre: ");
    fgets(senha_mestre, sizeof(senha_mestre), stdin);
    
    // Carregar credenciais (tenta .enc primeiro, depois .txt)
    if (!carregar_credencial("admin.txt", senha_mestre, &admin)) {
        fprintf(stderr, "Erro ao carregar credenciais\n");
        return 1;
    }
    
    // Usar credenciais em memória (SEGURO!)
    printf("Usuário: %s\n", admin.usuario);
    printf("Tipo: %s\n", 
           admin.criptografado ? "criptografado" : "texto");
    
    return 0;
}
```

### Exemplo 2: Compilar com OpenSSL

```bash
gcc -Wall -Wextra -g3 \
    src/main.c \
    -o src/output/main \
    -lssl -lcrypto   # ← Importante!
```

### Exemplo 3: Com Variáveis de Ambiente

```c
// Se estiver criptografado e senha_mestre vem de variável:
const char *senha_mestre = getenv("BRT_MASTER_PASSWORD");
if (!senha_mestre) {
    fprintf(stderr, "Defina BRT_MASTER_PASSWORD\n");
    return 1;
}

carregar_credencial("admin.txt", senha_mestre, &admin);
```

---

## 🔐 Segurança em Produção

### ✅ Checklist de Segurança

- [ ] Senhas originais removidas: `rm src/config/*.txt`
- [ ] Apenas `.enc` versionado no Git
- [ ] Permissões restrictivas: `chmod 600 *.enc`
- [ ] Dono correto: `chown root:root *.enc`
- [ ] Senha mestre em variável de ambiente (não em código)
- [ ] Senha mestre guardada em local seguro (Vault, K8s Secrets, etc)
- [ ] Acesso de read-only quando possível
- [ ] Backups criptografados também

### ⚠️ O Que NÃO Fazer

❌ **Gravar senha mestre no código**
```c
// NUNCA faça isso!
const char *senha = "chave123";  // ❌ PÉSSIMO
carregar_credencial("admin.txt", senha, &admin);
```

✅ **Usar variáveis de ambiente**
```c
// Assim!
const char *senha = getenv("BRT_MASTER_PASSWORD");  // ✅ Melhor
carregar_credencial("admin.txt", senha, &admin);
```

❌ **Deixar senhas em strings de erro**
```c
// NUNCA faça isso!
fprintf(stderr, "Senha: %s\n", admin.senha);  // ❌
```

✅ **Mascarar em mensagens**
```c
// Assim!
fprintf(stderr, "✅ Credenciais carregadas\n");  // ✅
```

---

## 🐛 Troubleshooting

### "Erro: arquivo não encontrado"
```bash
# Verificar se arquivo .enc existe
ls -la src/config/admin.txt.enc

# Se não existir, criptografar de novo
python3 crypto_util.py encrypt src/config/admin.txt
```

### "Erro: senha incorreta ou arquivo corrompido"
```bash
# Senha mestre está errada
# Ou arquivo foi corrompido
# Verificar com show:
python3 crypto_util.py show src/config/admin.txt.enc

# Se houver suspeita de corrupção:
# 1. Ter backup do .enc original
# 2. Restore do backup
# 3. Tentar novamente
```

### "Permissões negadas"
```bash
# Arquivo não tem permissão de leitura
ls -la src/config/admin.txt.enc
# Deve mostrar: -rw------- (600)

# Se diferente, corrigir:
chmod 600 src/config/admin.txt.enc
```

### "OpenSSL não encontrado"
```bash
# Instalar OpenSSL
sudo apt-get install libssl-dev

# Recompilar
gcc ... -lssl -lcrypto
```

---

## 📊 Comparação: Antes vs Depois

```
ANTES (INSEGURO):
├─ src/config/admin.txt         [TEXTO PURO - VISÍVEL!]
│  admin:senha123
├─ src/config/operadores.txt    [TEXTO PURO - VISÍVEL!]
│  op01:op123
└─ .git/logs/ 
   Histórico completo de senhas! [PÉSSIMO]

DEPOIS (SEGURO):
├─ src/config/admin.txt.enc      [CRIPTOGRAFADO - ilegível]
│  BRT_ENC_V1|R4N...Pm|7x3...Qw
├─ src/config/operadores.txt.enc [CRIPTOGRAFADO - ilegível]
│  BRT_ENC_V1|K9L...Xy|2j5...Rt
├─ .gitignore                    [Arquivos .txt ignorados]
│  src/config/*.txt
│  src/config/*.enc
└─ .git/logs/
   Sem histórico de senhas! [SEGURO]
```

---

## 🎯 Fluxo Recomendado

### Fase 1: Desenvolvimento (Agora)
```bash
# 1. Criptografar com senha fraca (para teste)
python3 crypto_util.py encrypt src/config/admin.txt
# Digite: dev123

# 2. Remover original
rm src/config/admin.txt

# 3. Testar programa
./src/output/main
# Pede: 🔐 Senha mestre: dev123
```

### Fase 2: Produção
```bash
# 1. Gerar senha mestre forte
openssl rand -base64 32

# 2. Criptografar com senha forte
python3 crypto_util.py encrypt src/config/admin.txt
# Digite: [senha forte acima]

# 3. Guardar senha em local seguro
# - Vault (HashiCorp)
# - AWS Secrets Manager
# - Azure Key Vault
# - Environment variable (CI/CD)
# - K8s secret (Docker/Kubernetes)

# 4. Executar
export BRT_MASTER_PASSWORD="[senha forte]"
./main
```

---

## 📚 Referências

**Criptografia**
- PBKDF2: RFC 2898
- AES-256: NIST FIPS 197
- Fernet: https://github.com/fernet/spec

**OpenSSL**
- EVP (Envelope) API: https://www.openssl.org/docs/manmaster/man3/
- PBKDF2: https://www.openssl.org/docs/manmaster/man3/PKCS5_PBKDF2_HMAC

**Python cryptography**
- Fernet: https://cryptography.io/en/latest/fernet/

---

## ✅ Próximos Passos

1. **Agora:** Criptografar arquivos existentes
   ```bash
   python3 crypto_util.py encrypt src/config/admin.txt
   rm src/config/admin.txt
   ```

2. **Depois:** Integrar ao programa principal
   - Incluir `config_crypto.h` no `main.c`
   - Pedir senha mestre ao iniciar
   - Carregar credenciais criptografadas

3. **Produção:** 
   - Usar variáveis de ambiente
   - Implementar Vault ou K8s Secrets
   - Configurar permissões de arquivo

---

**Status:** ✅ Sistema de Criptografia Implementado  
**Versão:** 1.0  
**Data:** 23 de Abril de 2026

Com este sistema, suas senhas estão **muito mais seguras**! 🔐
