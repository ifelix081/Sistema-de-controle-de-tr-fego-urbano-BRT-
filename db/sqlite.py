import sqlite3

# Conectar ao banco de dados
conn = sqlite3.connect('sisbus.db')

cursor = conn.cursor()

# Criar tabelas baseadas nas structs do sistema BRT

# Tabela de ônibus
cursor.execute('''
CREATE TABLE IF NOT EXISTS onibus (
    linha INTEGER,
    numero_onibus INTEGER PRIMARY KEY,
    placa TEXT,
    garagem TEXT,
    motorista TEXT,
    turno TEXT
)
''')

# Tabela de paradas
cursor.execute('''
CREATE TABLE IF NOT EXISTS paradas (
    codigo_parada INTEGER PRIMARY KEY,
    endereco TEXT
)
''')

# Tabela de linhas (relacionamento linha-parada)
cursor.execute('''
CREATE TABLE IF NOT EXISTS linhas (
    codigo_linha INTEGER,
    codigo_parada INTEGER,
    PRIMARY KEY (codigo_linha, codigo_parada),
    FOREIGN KEY (codigo_parada) REFERENCES paradas(codigo_parada)
)
''')

# Tabela de motoristas
cursor.execute('''
CREATE TABLE IF NOT EXISTS motoristas (
    nome TEXT,
    cpf TEXT PRIMARY KEY,
    telefone TEXT,
    linha_atuacao INTEGER
)
''')

print("Tabelas criadas com sucesso!")
conn.commit()

# Função para importar dados dos arquivos .txt
def importar_dados():
    import os
    
    # Caminho para os arquivos de relatório
    output_path = os.path.join('..', 'scr', 'output')
    
    # Importar ônibus
    try:
        with open(os.path.join(output_path, 'relatorio_brt.txt'), 'r', encoding='utf-8') as f:
            for linha in f:
                linha = linha.strip()
                if linha:
                    partes = linha.split()
                    if len(partes) >= 6:
                        cursor.execute('''
                            INSERT OR REPLACE INTO onibus 
                            (linha, numero_onibus, placa, garagem, motorista, turno)
                            VALUES (?, ?, ?, ?, ?, ?)
                        ''', (int(partes[0]), int(partes[1]), partes[2], partes[3], partes[4], partes[5]))
        print("Dados de ônibus importados com sucesso!")
    except FileNotFoundError:
        print("Arquivo relatorio_brt.txt não encontrado")
    except Exception as e:
        print(f"Erro ao importar ônibus: {e}")
    
    # Importar motoristas
    try:
        with open(os.path.join(output_path, 'relatorio_motorista.txt'), 'r', encoding='utf-8') as f:
            for linha in f:
                linha = linha.strip()
                if linha:
                    partes = linha.split()
                    if len(partes) >= 4:
                        cursor.execute('''
                            INSERT OR REPLACE INTO motoristas 
                            (nome, cpf, telefone, linha_atuacao)
                            VALUES (?, ?, ?, ?)
                        ''', (partes[0], partes[1], partes[2], int(partes[3])))
        print("Dados de motoristas importados com sucesso!")
    except FileNotFoundError:
        print("Arquivo relatorio_motorista.txt não encontrado")
    except Exception as e:
        print(f"Erro ao importar motoristas: {e}")
    
    # Importar paradas
    try:
        with open(os.path.join(output_path, 'relatorio_paradas.txt'), 'r', encoding='utf-8') as f:
            for linha in f:
                linha = linha.strip()
                if linha:
                    partes = linha.split(' ', 1)
                    if len(partes) >= 2:
                        cursor.execute('''
                            INSERT OR REPLACE INTO paradas 
                            (codigo_parada, endereco)
                            VALUES (?, ?)
                        ''', (int(partes[0]), partes[1]))
        print("Dados de paradas importados com sucesso!")
    except FileNotFoundError:
        print("Arquivo relatorio_paradas.txt não encontrado")
    except Exception as e:
        print(f"Erro ao importar paradas: {e}")
    
    # Importar linhas
    try:
        with open(os.path.join(output_path, 'relatorio_linhas.txt'), 'r', encoding='utf-8') as f:
            for linha in f:
                linha = linha.strip()
                if linha:
                    partes = linha.split()
                    if len(partes) >= 2:
                        cursor.execute('''
                            INSERT OR REPLACE INTO linhas 
                            (codigo_linha, codigo_parada)
                            VALUES (?, ?)
                        ''', (int(partes[0]), int(partes[1])))
        print("Dados de linhas importados com sucesso!")
    except FileNotFoundError:
        print("Arquivo relatorio_linhas.txt não encontrado")
    except Exception as e:
        print(f"Erro ao importar linhas: {e}")
    
    conn.commit()

# Importar os dados existentes
importar_dados()

# Fechar conexão
conn.close()
print("Importação concluída!")

