import sqlite3
from tabulate import tabulate

# Conectar ao banco de dados
conn = sqlite3.connect('sisbus.db')
cursor = conn.cursor()

print("=" * 80)
print("SISTEMA BRT - RELATORIO GERAL DE DADOS")
print("=" * 80)

# Tabela de Ônibus
print("\nONIBUS")
print("-" * 80)
cursor.execute("SELECT linha, numero_onibus, placa, garagem, motorista, turno FROM onibus ORDER BY linha, numero_onibus")
onibus = cursor.fetchall()

headers_onibus = ["Linha", "Numero", "Placa", "Garagem", "Motorista", "Turno"]
if onibus:
    print(tabulate(onibus, headers=headers_onibus, tablefmt="grid"))
else:
    print("Nenhum onibus cadastrado.")

# Tabela de Motoristas
print("\nMOTORISTAS")
print("-" * 80)
cursor.execute("SELECT nome, cpf, telefone, linha_atuacao FROM motoristas ORDER BY nome")
motoristas = cursor.fetchall()

headers_motoristas = ["Nome", "CPF", "Telefone", "Linha Atuacao"]
if motoristas:
    print(tabulate(motoristas, headers=headers_motoristas, tablefmt="grid"))
else:
    print("Nenhum motorista cadastrado.")

# Tabela de Paradas
print("\nPARADAS")
print("-" * 80)
cursor.execute("SELECT codigo_parada, endereco FROM paradas ORDER BY codigo_parada")
paradas = cursor.fetchall()

headers_paradas = ["Codigo", "Endereco"]
if paradas:
    print(tabulate(paradas, headers=headers_paradas, tablefmt="grid"))
else:
    print("Nenhuma parada cadastrada.")

# Tabela de Linhas (com detalhes)
print("\nLINHAS E PARADAS")
print("-" * 80)
cursor.execute('''
    SELECT 
        l.codigo_linha, 
        l.codigo_parada, 
        p.endereco
    FROM linhas l
    LEFT JOIN paradas p ON l.codigo_parada = p.codigo_parada
    ORDER BY l.codigo_linha, l.codigo_parada
''')
linhas_detalhes = cursor.fetchall()

headers_linhas = ["Codigo Linha", "Codigo Parada", "Endereco da Parada"]
if linhas_detalhes:
    print(tabulate(linhas_detalhes, headers=headers_linhas, tablefmt="grid"))
else:
    print("Nenhuma linha cadastrada.")

# Estatísticas gerais
print("\nESTATISTICAS GERAIS")
print("-" * 80)

cursor.execute("SELECT COUNT(*) FROM onibus")
total_onibus = cursor.fetchone()[0]

cursor.execute("SELECT COUNT(*) FROM motoristas")
total_motoristas = cursor.fetchone()[0]

cursor.execute("SELECT COUNT(*) FROM paradas")
total_paradas = cursor.fetchone()[0]

cursor.execute("SELECT COUNT(DISTINCT codigo_linha) FROM linhas")
total_linhas = cursor.fetchone()[0]

estatisticas = [
    ["Total de Onibus", total_onibus],
    ["Total de Motoristas", total_motoristas],
    ["Total de Paradas", total_paradas],
    ["Total de Linhas", total_linhas]
]

print(tabulate(estatisticas, headers=["Categoria", "Quantidade"], tablefmt="grid"))

conn.close()
print("\n" + "=" * 80)
print("Relatorio gerado com sucesso!")
print("=" * 80)
