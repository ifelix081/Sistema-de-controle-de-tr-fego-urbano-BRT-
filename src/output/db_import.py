import sqlite3
import os

BASE_DIR = os.path.dirname(os.path.abspath(__file__))  
ROOT_DIR = os.path.dirname(BASE_DIR)                   

DB_DIR = os.path.join(ROOT_DIR, "db")
DB_NAME = os.path.join(DB_DIR, "brt.db")

if not os.path.exists(DB_DIR):
    os.makedirs(DB_DIR)


def conectar():
    return sqlite3.connect(DB_NAME)



def criar_tabelas():
    conn = conectar()
    cursor = conn.cursor()

    cursor.execute("""
    CREATE TABLE IF NOT EXISTS onibus (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        linha INTEGER,
        numero INTEGER,
        placa TEXT UNIQUE,
        garagem TEXT,
        motorista TEXT,
        turno TEXT
    )
    """)

    cursor.execute("""
    CREATE TABLE IF NOT EXISTS paradas (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        codigo INTEGER UNIQUE,
        endereco TEXT
    )
    """)

    cursor.execute("""
    CREATE TABLE IF NOT EXISTS motoristas (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        cpf TEXT UNIQUE,
        nome TEXT,
        telefone TEXT,
        linha INTEGER
    )
    """)

    cursor.execute("""
    CREATE TABLE IF NOT EXISTS linhas (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        codigo_linha INTEGER,
        codigo_parada INTEGER
    )
    """)

    cursor.execute("""
    CREATE TABLE IF NOT EXISTS avisos (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        protocolo TEXT UNIQUE,
        datahora TEXT,
        motorista TEXT,
        linha TEXT,
        tipo TEXT,
        gravidade TEXT,
        local TEXT,
        descricao TEXT
    )
    """)

    cursor.execute("""
    CREATE TABLE IF NOT EXISTS respostas (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        protocolo TEXT,
        datahora TEXT,
        operador TEXT,
        acao TEXT
    )
    """)

    cursor.execute("""
    CREATE TABLE IF NOT EXISTS sessoes (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        matricula TEXT,
        nome TEXT,
        linha TEXT,
        veiculo TEXT,
        inicio TEXT,
        fim TEXT,
        ativo INTEGER
    )
    """)

    conn.commit()
    conn.close()



def importar_csv(arquivo, query, transform=None):
    if not os.path.exists(arquivo):
        return

    conn = conectar()
    cursor = conn.cursor()

    with open(arquivo, "r", encoding="utf-8") as f:
        for linha in f:
            linha = linha.strip()
            if not linha:
                continue

            dados = linha.split(";")

            if transform:
                dados = transform(dados)

            try:
                cursor.execute(query, dados)
            except:
                pass  # ignora duplicados

    conn.commit()
    conn.close()



def importar_onibus():
    importar_csv(
        "dados_onibus.csv",
        """INSERT OR IGNORE INTO onibus 
        (linha, numero, placa, garagem, motorista, turno)
        VALUES (?, ?, ?, ?, ?, ?)""",
        lambda d: (int(d[0]), int(d[1]), d[2], d[3], d[4], d[5])
    )


def importar_paradas():
    importar_csv(
        "dados_paradas.csv",
        """INSERT OR IGNORE INTO paradas 
        (codigo, endereco)
        VALUES (?, ?)""",
        lambda d: (int(d[0]), d[1])
    )


def importar_motoristas():
    importar_csv(
        "dados_motoristas.csv",
        """INSERT OR IGNORE INTO motoristas 
        (nome, cpf, telefone, linha)
        VALUES (?, ?, ?, ?)""",
        lambda d: (d[0], d[1], d[2], int(d[3]))
    )


def importar_linhas():
    if not os.path.exists("dados_linhas.csv"):
        return

    conn = conectar()
    cursor = conn.cursor()

    with open("dados_linhas.csv", "r", encoding="utf-8") as f:
        for linha in f:
            dados = linha.strip().split(";")
            cod = int(dados[0])

            for parada in dados[1:]:
                try:
                    cursor.execute(
                        "INSERT INTO linhas (codigo_linha, codigo_parada) VALUES (?, ?)",
                        (cod, int(parada))
                    )
                except:
                    pass

    conn.commit()
    conn.close()


def importar_avisos():
    importar_csv(
        "avisos.csv",
        """INSERT OR IGNORE INTO avisos 
        (protocolo, datahora, motorista, linha, tipo, gravidade, local, descricao)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?)"""
    )


def importar_respostas():
    importar_csv(
        "respostas.csv",
        """INSERT INTO respostas 
        (protocolo, datahora, operador, acao)
        VALUES (?, ?, ?, ?)"""
    )


def importar_sessoes():
    importar_csv(
        "sessoes.csv",
        """INSERT INTO sessoes 
        (matricula, nome, linha, veiculo, inicio, fim, ativo)
        VALUES (?, ?, ?, ?, ?, ?, ?)""",
        lambda d: (
            d[0], d[1], d[2], d[3], d[4],
            d[5] if d[5] else None,
            1 if d[6] == "ativo" else 0
        )
    )


def limpar_csv():
    arquivos = [
        "dados_onibus.csv",
        "dados_paradas.csv",
        "dados_motoristas.csv",
        "dados_linhas.csv",
        "avisos.csv",
        "respostas.csv",
        "sessoes.csv"
    ]

    for arq in arquivos:
        if os.path.exists(arq):
            os.remove(arq)


def executar():
    print("\n[DB] Atualizando banco SQLite...\n")

    criar_tabelas()

    importar_onibus()
    importar_paradas()
    importar_motoristas()
    importar_linhas()
    importar_avisos()
    importar_respostas()
    importar_sessoes()


    print("[DB] Atualização concluída.\n")


if __name__ == "__main__":
    executar()