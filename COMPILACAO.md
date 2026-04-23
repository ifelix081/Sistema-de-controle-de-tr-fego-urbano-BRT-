# 🔧 Compilação com Criptografia - Sistema BRT

## 📋 Visão Rápida

```bash
# Compilação com criptografia AES-256
gcc -Wall -Wextra -g3 \
    src/main.c \
    -o src/output/main \
    -lssl -lcrypto  # ← Importante!
```

---

## 📦 Dependências Necessárias

### 1. OpenSSL (C/C++)

**Debian/Ubuntu:**
```bash
sudo apt-get update
sudo apt-get install libssl-dev
```

**Verificar instalação:**
```bash
pkg-config --cflags --libs openssl
# Resultado esperado: -I... -lssl -lcrypto
```

### 2. Python cryptography (para utilitário)

```bash
pip install cryptography
```

**Verificar:**
```bash
python3 -c "import cryptography; print(cryptography.__version__)"
```

---

## 🛠️ Variações de Compilação

### Compilação Básica (Sem Criptografia)

```bash
gcc -Wall -Wextra -g3 src/main.c -o src/output/main
```

❌ **Problema:** Senhas em texto puro

### Compilação com Criptografia (Recomendado)

```bash
gcc -Wall -Wextra -g3 \
    src/main.c \
    -o src/output/main \
    -lssl -lcrypto
```

✅ **Benefício:** AES-256-CBC + PBKDF2 + Fernet

### Compilação com Debug Completo

```bash
gcc -Wall -Wextra -g3 -DDEBUG \
    src/main.c \
    -o src/output/main \
    -lssl -lcrypto
```

Ativa mensagens de debug

### Compilação Otimizada (Produção)

```bash
gcc -Wall -Wextra -O2 \
    src/main.c \
    -o src/output/main \
    -lssl -lcrypto
```

**-O2:** Otimização de velocidade
**Sem -g:** Remove símbolos de debug

---

## 📝 Arquivo Makefile

Crie um `Makefile` para simplificar:

```makefile
# Makefile para Sistema BRT com Criptografia

CC = gcc
CFLAGS = -Wall -Wextra -g3
LDFLAGS = -lssl -lcrypto

# Paths
SRC_DIR = src
OUTPUT_DIR = src/output
MAIN_SRC = $(SRC_DIR)/main.c
MAIN_OUT = $(OUTPUT_DIR)/main

# Targets
all: compile

compile: $(MAIN_OUT)

$(OUTPUT_DIR)/main: $(MAIN_SRC)
	@mkdir -p $(OUTPUT_DIR)
	$(CC) $(CFLAGS) $(MAIN_SRC) -o $(MAIN_OUT) $(LDFLAGS)
	@echo "✅ Compilado com sucesso: $(MAIN_OUT)"

clean:
	rm -f $(OUTPUT_DIR)/main
	@echo "✅ Limpado"

test: compile
	./$(MAIN_OUT)

help:
	@echo "Targets disponíveis:"
	@echo "  make          - Compilar"
	@echo "  make clean    - Limpar binários"
	@echo "  make test     - Compilar e executar"
	@echo "  make help     - Esta mensagem"

.PHONY: all compile clean test help
```

**Usar:**
```bash
# Compilar
make

# Compilar e executar
make test

# Limpar
make clean
```

---

## 🔗 Arquivos Necessários

```
seu_projeto/
├── src/
│   ├── main.c              ← Programa principal
│   ├── config.h            ← Localização de arquivos
│   ├── config_crypto.h     ← Criptografia integrada
│   ├── crypto.h            ← AES-256-CBC
│   ├── exemplo_crypto.h    ← Exemplos de uso
│   └── config/
│       ├── admin.txt.enc   ← Criptografado
│       └── operadores.txt.enc
├── crypto_util.py          ← Utilitário de encrypt/decrypt
├── setup_crypto.sh         ← Setup automático
├── Makefile                ← Build system
└── GUIA_CRIPTOGRAFIA.md    ← Esta documentação
```

---

## ⚙️ Configuração do Build System

### CMake (Alternativa ao Makefile)

```cmake
# CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(BRT_Sistema)

# Encontrar OpenSSL
find_package(OpenSSL REQUIRED)

# Executável
add_executable(brt_main src/main.c)

# Link com OpenSSL
target_link_libraries(brt_main PRIVATE OpenSSL::Crypto OpenSSL::SSL)

# Flags de compilação
target_compile_options(brt_main PRIVATE -Wall -Wextra -g3)

# Output directory
set_target_properties(brt_main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/output"
)
```

**Usar:**
```bash
mkdir build
cd build
cmake ..
make
```

---

## 🧪 Verificar Compilação

### Testar Links Dinâmicos

```bash
# Ver quais bibliotecas o executável usa
ldd src/output/main

# Esperado:
# libssl.so.X => /lib/x86_64-linux-gnu/libssl.so.X
# libcrypto.so.X => /lib/x86_64-linux-gnu/libcrypto.so.X
```

### Testar Símbolos

```bash
# Ver se símbolos OpenSSL estão presentes
nm src/output/main | grep -i ssl

# Ou
objdump -t src/output/main | grep -i crypto
```

### Executar com LD_DEBUG

```bash
# Ver quais bibliotecas são carregadas
LD_DEBUG=libs src/output/main 2>&1 | grep -i ssl
```

---

## 🚀 Fluxo Completo de Setup

### 1. Instalar Dependências

```bash
# OpenSSL
sudo apt-get install libssl-dev

# Python
pip install cryptography
```

### 2. Preparar Arquivos de Configuração

```bash
# Criar arquivos com senhas reais
echo "admin:senha_real" > src/config/admin.txt
echo "op01:op_senha_real" > src/config/operadores.txt
```

### 3. Criptografar

```bash
# Usar o script de setup
bash setup_crypto.sh

# Ou manualmente
python3 crypto_util.py encrypt src/config/admin.txt
python3 crypto_util.py encrypt src/config/operadores.txt
rm src/config/*.txt  # Remover originais
```

### 4. Compilar Programa

```bash
# Com Makefile
make

# Ou manualmente
gcc -Wall -Wextra -g3 src/main.c -o src/output/main -lssl -lcrypto
```

### 5. Executar

```bash
# Com exportação de variável
export BRT_MASTER_PASSWORD="senha_mestre_123"
./src/output/main

# Ou com prompt (menos seguro)
./src/output/main
# 🔐 Senha mestre: [digite]
```

---

## 🔍 Troubleshooting

### "error: openssl/aes.h: No such file or directory"

**Causa:** OpenSSL não instalado ou headers não encontrados

**Solução:**
```bash
# Instalar headers
sudo apt-get install libssl-dev

# Verificar localização
pkg-config --cflags openssl
```

### "undefined reference to `OPENSSL_init_crypto'"

**Causa:** Falta de flag `-lssl -lcrypto` na compilação

**Solução:**
```bash
# Compilar com flags corretas
gcc ... -lssl -lcrypto
```

### "libssl.so.3: cannot open shared object file"

**Causa:** Biblioteca OpenSSL não instalada em runtime

**Solução:**
```bash
# Instalar libssl em runtime
sudo apt-get install libssl3

# Ou usar build estático
gcc ... -static -lssl -lcrypto
```

### "crypto_util.py not found"

**Causa:** Script Python não tem permissão ou está no lugar errado

**Solução:**
```bash
# Dar permissão
chmod +x crypto_util.py

# Verificar localização
ls -la crypto_util.py
```

### "ModuleNotFoundError: No module named 'cryptography'"

**Causa:** Módulo Python não instalado

**Solução:**
```bash
pip install cryptography

# Ou para sistema
sudo apt-get install python3-cryptography
```

---

## 📊 Script de Build Completo

Crie `build.sh`:

```bash
#!/bin/bash

set -e

echo "🔨 Build Script - Sistema BRT"
echo "════════════════════════════════════════"

# Verificar dependências
echo "📦 Verificando dependências..."
pkg-config --cflags --libs openssl > /dev/null || {
    echo "❌ OpenSSL não encontrado"
    echo "   Instale: sudo apt-get install libssl-dev"
    exit 1
}

python3 -c "import cryptography" > /dev/null || {
    echo "❌ cryptography não encontrado"
    echo "   Instale: pip install cryptography"
    exit 1
}

# Criar diretório de output
mkdir -p src/output

# Compilar
echo "🔨 Compilando..."
gcc -Wall -Wextra -g3 \
    src/main.c \
    -o src/output/main \
    -lssl -lcrypto

echo "✅ Build concluído com sucesso!"
echo ""
echo "Próximas etapas:"
echo "  1. Criptografar senhas: bash setup_crypto.sh"
echo "  2. Executar: ./src/output/main"
```

**Usar:**
```bash
chmod +x build.sh
./build.sh
```

---

## 🎯 Checklist de Compilação

- [ ] OpenSSL instalado (`apt-get install libssl-dev`)
- [ ] Python cryptography instalado (`pip install cryptography`)
- [ ] Headers em `src/crypto.h` inclusos
- [ ] config.h integrado em main.c
- [ ] config_crypto.h no diretório src/
- [ ] Compilação com `-lssl -lcrypto`
- [ ] Verificação com `ldd src/output/main`
- [ ] Teste com dados criptografados

---

## 📝 Exemplo de Compilação Passo-a-Passo

```bash
# 1. Ir para diretório
cd /workspaces/Sistema-de-controle-de-tr-fego-urbano-BRT-

# 2. Instalar OpenSSL
sudo apt-get install libssl-dev

# 3. Criptografar senhas
bash setup_crypto.sh
# Escolha opção 1: Criptografar

# 4. Compilar
gcc -Wall -Wextra -g3 \
    src/main.c \
    -o src/output/main \
    -lssl -lcrypto

# 5. Verificar
ldd src/output/main | grep ssl

# 6. Executar
export BRT_MASTER_PASSWORD="senha_mestre"
./src/output/main
```

---

## ✅ Status da Implementação

```
✅ crypto.h                    (385 linhas) - Criptografia AES-256
✅ config_crypto.h            (225 linhas) - Integração
✅ crypto_util.py             (350 linhas) - Utilitário Python
✅ setup_crypto.sh            (300 linhas) - Setup automático
✅ exemplo_crypto.h           (400 linhas) - Exemplos de uso
✅ GUIA_CRIPTOGRAFIA.md       (500 linhas) - Documentação
✅ COMPILACAO.md              (Este arquivo)

Total: ~2.5K linhas de código + documentação
```

---

**Status:** ✅ Sistema de Criptografia Pronto  
**Versão:** 1.0  
**Data:** 23 de Abril de 2026

Suas senhas agora estão **muito mais seguras**! 🔐
