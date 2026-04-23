#!/bin/bash
# setup_crypto.sh - Setup de criptografia de senhas
# Facilita o processo de criptografar arquivos de configuração

set -e

PROJETO_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CONFIG_DIR="${PROJETO_DIR}/src/config"

echo ""
echo "╔════════════════════════════════════════════════════════════════╗"
echo "║          Setup de Criptografia - Sistema BRT                  ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""

# Cores
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

info() { echo -e "${BLUE}[INFO]${NC} $1"; }
sucesso() { echo -e "${GREEN}[✓]${NC} $1"; }
erro() { echo -e "${RED}[✗]${NC} $1"; }
aviso() { echo -e "${YELLOW}[!]${NC} $1"; }

# ========================================================================================
# 1. Verificar dependências
# ========================================================================================

echo "Passo 1: Verificando dependências..."
echo "─────────────────────────────────────────────────────────────────"

# Python
if command -v python3 &> /dev/null; then
    sucesso "Python 3 encontrado"
else
    erro "Python 3 não encontrado. Instale com: sudo apt-get install python3"
    exit 1
fi

# OpenSSL
if command -v openssl &> /dev/null; then
    sucesso "OpenSSL encontrado"
else
    erro "OpenSSL não encontrado. Instale com: sudo apt-get install openssl libssl-dev"
    exit 1
fi

# cryptography (Python)
if python3 -c "import cryptography" 2>/dev/null; then
    sucesso "Módulo cryptography encontrado"
else
    aviso "Módulo cryptography não encontrado"
    echo "Instalando com: pip install cryptography"
    pip install cryptography
fi

# ========================================================================================
# 2. Verificar arquivos
# ========================================================================================

echo ""
echo "Passo 2: Verificando arquivos..."
echo "─────────────────────────────────────────────────────────────────"

if [[ ! -d "$CONFIG_DIR" ]]; then
    erro "Diretório não encontrado: $CONFIG_DIR"
    exit 1
fi

sucesso "Diretório encontrado: $CONFIG_DIR"

# Listar arquivos de configuração
echo ""
info "Arquivos encontrados:"
ls -lh "$CONFIG_DIR"/*.txt 2>/dev/null || aviso "Nenhum arquivo .txt encontrado"

# ========================================================================================
# 3. Escolher opção
# ========================================================================================

echo ""
echo "Passo 3: Escolher ação..."
echo "─────────────────────────────────────────────────────────────────"
echo ""
echo "O que deseja fazer?"
echo ""
echo "1) Criptografar todos os arquivos .txt"
echo "2) Descriptografar todos os arquivos .enc"
echo "3) Visualizar informações de arquivo .enc"
echo "4) Apenas criar exemplos de arquivo"
echo "5) Sair"
echo ""

read -p "Escolha (1-5): " opcao

case $opcao in

# ========================================================================================
# Opção 1: Criptografar
# ========================================================================================
1)
    echo ""
    info "Criptografando arquivos..."
    
    # Encontrar arquivos .txt
    TXT_FILES=$(find "$CONFIG_DIR" -maxdepth 1 -name "*.txt" -type f)
    
    if [[ -z "$TXT_FILES" ]]; then
        erro "Nenhum arquivo .txt encontrado em $CONFIG_DIR"
        exit 1
    fi
    
    # Pedir senha
    echo ""
    read -sp "🔐 Digite a senha mestre para criptografia: " MASTER_PASS
    echo ""
    read -sp "🔐 Confirme a senha: " MASTER_PASS_CONFIRM
    echo ""
    
    if [[ "$MASTER_PASS" != "$MASTER_PASS_CONFIRM" ]]; then
        erro "Senhas não conferem!"
        exit 1
    fi
    
    if [[ -z "$MASTER_PASS" ]]; then
        erro "Senha não pode estar vazia!"
        exit 1
    fi
    
    # Criptografar cada arquivo
    TOTAL=$(echo "$TXT_FILES" | wc -l)
    COUNT=0
    
    echo ""
    info "Processando $TOTAL arquivo(s)..."
    
    while IFS= read -r arquivo; do
        COUNT=$((COUNT + 1))
        ARQUIVO_BASE=$(basename "$arquivo")
        
        echo ""
        info "[$COUNT/$TOTAL] Criptografando: $ARQUIVO_BASE"
        
        # Criptografar usando o utilitário Python
        echo "$MASTER_PASS" | python3 "${PROJETO_DIR}/crypto_util.py" encrypt "$arquivo" || {
            erro "Falha ao criptografar: $ARQUIVO_BASE"
            exit 1
        }
        
        sucesso "Criptografado: $ARQUIVO_BASE → $ARQUIVO_BASE.enc"
    done <<< "$TXT_FILES"
    
    # Perguntar se deseja remover originais
    echo ""
    echo "─────────────────────────────────────────────────────────────────"
    echo ""
    aviso "Arquivos .txt original ainda existem!"
    echo ""
    read -p "Remover arquivos .txt originais? (s/n): " remover
    
    if [[ "$remover" == "s" || "$remover" == "S" ]]; then
        echo ""
        info "Removendo arquivos .txt originais..."
        
        while IFS= read -r arquivo; do
            ARQUIVO_BASE=$(basename "$arquivo")
            rm -f "$arquivo"
            sucesso "Removido: $ARQUIVO_BASE"
        done <<< "$TXT_FILES"
    else
        aviso "Arquivos .txt não foram removidos"
        aviso "Remova manualmente para manter segurança: rm src/config/*.txt"
    fi
    
    echo ""
    sucesso "Criptografia concluída!"
    echo ""
    info "Próximos passos:"
    echo "   1. Guardar senha mestre em local seguro"
    echo "   2. Defina em runtime: export BRT_MASTER_PASSWORD='[senha]'"
    echo "   3. Ou use em código C com config_crypto.h"
    echo ""
    ;;

# ========================================================================================
# Opção 2: Descriptografar
# ========================================================================================
2)
    echo ""
    info "Descriptografando arquivos..."
    
    # Encontrar arquivos .enc
    ENC_FILES=$(find "$CONFIG_DIR" -maxdepth 1 -name "*.enc" -type f)
    
    if [[ -z "$ENC_FILES" ]]; then
        erro "Nenhum arquivo .enc encontrado em $CONFIG_DIR"
        exit 1
    fi
    
    # Pedir senha
    echo ""
    read -sp "🔐 Digite a senha mestre: " MASTER_PASS
    echo ""
    
    TOTAL=$(echo "$ENC_FILES" | wc -l)
    COUNT=0
    
    echo ""
    info "Processando $TOTAL arquivo(s)..."
    
    while IFS= read -r arquivo; do
        COUNT=$((COUNT + 1))
        ARQUIVO_BASE=$(basename "$arquivo")
        ARQUIVO_OUT="${arquivo%.enc}"
        
        echo ""
        info "[$COUNT/$TOTAL] Descriptografando: $ARQUIVO_BASE"
        
        # Descriptografar
        echo "$MASTER_PASS" | python3 "${PROJETO_DIR}/crypto_util.py" decrypt "$arquivo" || {
            erro "Falha ao descriptografar: $ARQUIVO_BASE (senha incorreta?)"
            exit 1
        }
        
        sucesso "Descriptografado: $ARQUIVO_BASE → $(basename "$ARQUIVO_OUT")"
    done <<< "$ENC_FILES"
    
    echo ""
    sucesso "Descriptografia concluída!"
    ;;

# ========================================================================================
# Opção 3: Ver informações
# ========================================================================================
3)
    echo ""
    info "Visualizando informações de arquivo criptografado..."
    
    # Encontrar arquivos .enc
    ENC_FILES=$(find "$CONFIG_DIR" -maxdepth 1 -name "*.enc" -type f)
    
    if [[ -z "$ENC_FILES" ]]; then
        erro "Nenhum arquivo .enc encontrado"
        exit 1
    fi
    
    echo ""
    echo "Arquivos .enc encontrados:"
    echo ""
    
    COUNT=1
    while IFS= read -r arquivo; do
        echo "  $COUNT) $(basename "$arquivo")"
        COUNT=$((COUNT + 1))
    done <<< "$ENC_FILES"
    
    echo ""
    read -p "Escolha arquivo (número): " choice
    
    SELECTED=$(echo "$ENC_FILES" | sed -n "${choice}p")
    
    if [[ -z "$SELECTED" ]]; then
        erro "Escolha inválida"
        exit 1
    fi
    
    echo ""
    python3 "${PROJETO_DIR}/crypto_util.py" show "$SELECTED"
    ;;

# ========================================================================================
# Opção 4: Criar exemplos
# ========================================================================================
4)
    echo ""
    info "Criando arquivos de exemplo..."
    
    # Criar admin.txt.example
    cat > "${CONFIG_DIR}/admin.txt.example" << 'EOF'
admin:senha_exemplo_123
root:root_senha_exemplo
EOF
    
    sucesso "Criado: ${CONFIG_DIR}/admin.txt.example"
    
    # Criar operadores.txt.example
    cat > "${CONFIG_DIR}/operadores.txt.example" << 'EOF'
op01:operador_senha_123
op02:operador_senha_456
EOF
    
    sucesso "Criado: ${CONFIG_DIR}/operadores.txt.example"
    
    echo ""
    info "Agora você pode:"
    echo "   1. Copiar os exemplos"
    echo "   2. Editar com senhas reais"
    echo "   3. Criptografar"
    echo ""
    echo "Exemplo:"
    echo "   cp src/config/admin.txt.example src/config/admin.txt"
    echo "   nano src/config/admin.txt  # Edite com senhas reais"
    echo "   python3 crypto_util.py encrypt src/config/admin.txt"
    echo ""
    ;;

# ========================================================================================
# Opção 5: Sair
# ========================================================================================
5)
    echo ""
    info "Saindo..."
    exit 0
    ;;

*)
    erro "Opção inválida"
    exit 1
    ;;

esac

echo ""
echo "╔════════════════════════════════════════════════════════════════╗"
echo "║                    Setup Concluído!                           ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""
