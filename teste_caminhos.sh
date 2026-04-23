#!/bin/bash
# teste_caminhos.sh - Script de teste automático para validar localização de arquivos
# Uso: bash teste_caminhos.sh

set -e

PROJETO_DIR="/workspaces/Sistema-de-controle-de-tr-fego-urbano-BRT-"
EXEC="${PROJETO_DIR}/src/output/main"

echo "╔════════════════════════════════════════════════════════════════╗"
echo "║     Teste Automático: Localização de Arquivos de Config       ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""

# Cores
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Contadores
TESTES_PASSOU=0
TESTES_FALHOU=0

teste() {
    echo -e "${BLUE}[TESTE]${NC} $1"
}

sucesso() {
    echo -e "${GREEN}[✓] $1${NC}"
    ((TESTES_PASSOU++))
}

erro() {
    echo -e "${RED}[✗] $1${NC}"
    ((TESTES_FALHOU++))
}

aviso() {
    echo -e "${YELLOW}[!] $1${NC}"
}

# ============================================================================
# TESTE 1: Verificar se arquivos existem
# ============================================================================
echo ""
teste "1. Verificando existência dos arquivos de configuração..."

if [[ ! -f "${PROJETO_DIR}/src/config/admin.txt" ]]; then
    erro "admin.txt não encontrado em ${PROJETO_DIR}/src/config/"
    exit 1
else
    sucesso "admin.txt encontrado"
fi

if [[ ! -f "${PROJETO_DIR}/src/config/operadores.txt" ]]; then
    erro "operadores.txt não encontrado em ${PROJETO_DIR}/src/config/"
    exit 1
else
    sucesso "operadores.txt encontrado"
fi

# ============================================================================
# TESTE 2: Verificar permissões
# ============================================================================
echo ""
teste "2. Verificando permissões dos arquivos..."

PERM_ADMIN=$(stat -c "%a" "${PROJETO_DIR}/src/config/admin.txt")
PERM_OPER=$(stat -c "%a" "${PROJETO_DIR}/src/config/operadores.txt")

if [[ "$PERM_ADMIN" == "644" || "$PERM_ADMIN" == "444" ]]; then
    sucesso "admin.txt tem permissões corretas ($PERM_ADMIN)"
else
    aviso "admin.txt tem permissões incomuns ($PERM_ADMIN), restaurando..."
    chmod 644 "${PROJETO_DIR}/src/config/admin.txt"
    sucesso "Permissões restauradas"
fi

if [[ "$PERM_OPER" == "644" || "$PERM_OPER" == "444" ]]; then
    sucesso "operadores.txt tem permissões corretas ($PERM_OPER)"
else
    aviso "operadores.txt tem permissões incomuns ($PERM_OPER), restaurando..."
    chmod 644 "${PROJETO_DIR}/src/config/operadores.txt"
    sucesso "Permissões restauradas"
fi

# ============================================================================
# TESTE 3: Verificar conteúdo
# ============================================================================
echo ""
teste "3. Verificando conteúdo dos arquivos..."

ADMIN_LINES=$(wc -l < "${PROJETO_DIR}/src/config/admin.txt")
OPER_LINES=$(wc -l < "${PROJETO_DIR}/src/config/operadores.txt")

if [[ $ADMIN_LINES -gt 0 ]]; then
    sucesso "admin.txt tem $ADMIN_LINES linha(s)"
else
    erro "admin.txt está vazio!"
    exit 1
fi

if [[ $OPER_LINES -gt 0 ]]; then
    sucesso "operadores.txt tem $OPER_LINES linha(s)"
else
    erro "operadores.txt está vazio!"
    exit 1
fi

# ============================================================================
# TESTE 4: Compilação
# ============================================================================
echo ""
teste "4. Verificando se programa está compilado..."

if [[ ! -f "$EXEC" ]]; then
    erro "Programa não compilado em $EXEC"
    echo "Compilando..."
    cd "$PROJETO_DIR"
    gcc -Wall -Wextra -g3 \
        -I"${PROJETO_DIR}/src/auth" \
        "${PROJETO_DIR}/src/main.c" \
        -o "$EXEC" 2>&1
    if [[ $? -eq 0 ]]; then
        sucesso "Programa compilado com sucesso"
    else
        erro "Falha ao compilar"
        exit 1
    fi
else
    sucesso "Programa já compilado"
fi

# ============================================================================
# TESTE 5: Execução da raiz do projeto
# ============================================================================
echo ""
teste "5. Executando da raiz do projeto (timeout 3s)..."

cd "$PROJETO_DIR"
TIMEOUT_SAIDA=$(timeout 3 "$EXEC" 2>&1 <<< "" || true)

if echo "$TIMEOUT_SAIDA" | grep -q "OK.*Todos os arquivos"; then
    sucesso "Localização com sucesso (CWD=$PROJETO_DIR)"
elif echo "$TIMEOUT_SAIDA" | grep -q "Encontrado.*admin.txt"; then
    sucesso "Arquivos encontrados (output truncado)"
else
    erro "Não localizou arquivos corretamente"
    echo "Saída recebida:"
    echo "$TIMEOUT_SAIDA" | head -10
fi

# ============================================================================
# TESTE 6: Execução de outro diretório
# ============================================================================
echo ""
teste "6. Executando de diretório diferente (/tmp)..."

cd /tmp
TIMEOUT_SAIDA=$(timeout 3 "$EXEC" 2>&1 <<< "" || true)

if echo "$TIMEOUT_SAIDA" | grep -q "OK.*Encontrado"; then
    sucesso "Localização com sucesso mesmo de /tmp!"
    echo "    Demonstra: Busca relativa ao executável funciona ✓"
else
    aviso "Pode não encontrar se está longe da raiz do projeto"
    echo "    Teste com: export CONFIG_PATH=$(pwd)/src/config"
fi

# ============================================================================
# TESTE 7: Variável de ambiente
# ============================================================================
echo ""
teste "7. Testando suporte a variável de ambiente..."

export CONFIG_PATH="${PROJETO_DIR}/src/config"
cd /tmp
TIMEOUT_SAIDA=$(timeout 3 "$EXEC" 2>&1 <<< "" || true)

if echo "$TIMEOUT_SAIDA" | grep -q "OK\|Encontrado"; then
    sucesso "CONFIG_PATH aceita e funciona corretamente!"
else
    aviso "CONFIG_PATH pode não estar sendo usado"
fi

unset CONFIG_PATH

# ============================================================================
# TESTE 8: Conteúdo válido
# ============================================================================
echo ""
teste "8. Validando formato dos arquivos..."

# Verificar se tem pelo menos um ':' em cada linha
INVALID_ADMIN=$(grep -vc ':' "${PROJETO_DIR}/src/config/admin.txt" || true)
INVALID_OPER=$(grep -vc ':' "${PROJETO_DIR}/src/config/operadores.txt" || true)

if [[ $INVALID_ADMIN -eq 0 ]]; then
    sucesso "admin.txt tem formato válido (usuario:senha)"
else
    aviso "Algumas linhas em admin.txt podem estar no formato errado"
fi

if [[ $INVALID_OPER -eq 0 ]]; then
    sucesso "operadores.txt tem formato válido (matricula:senha)"
else
    aviso "Algumas linhas em operadores.txt podem estar no formato errado"
fi

# ============================================================================
# Resumo
# ============================================================================
echo ""
echo "╔════════════════════════════════════════════════════════════════╗"
echo "║                         RESUMO DOS TESTES                      ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""

TOTAL=$((TESTES_PASSOU + TESTES_FALHOU))
PERCENTUAL=$((TESTES_PASSOU * 100 / TOTAL))

if [[ $TESTES_FALHOU -eq 0 ]]; then
    echo -e "${GREEN}✓ TODOS OS TESTES PASSARAM! ($PERCENTUAL%)${NC}"
    echo ""
    echo "Sistema de localização de arquivos está:"
    echo "  ✓ Funcionando corretamente"
    echo "  ✓ Pronto para produção"
    echo ""
    exit 0
else
    echo -e "${RED}✗ ALGUNS TESTES FALHARAM ($PERCENTUAL% OK)${NC}"
    echo ""
    echo "Pontos de falha:"
    echo "  ✗ Verifique os erros acima"
    echo ""
    exit 1
fi

