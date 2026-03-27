import sqlite3
import csv
import os

# Conectar ao banco de dados
conn = sqlite3.connect('sisbus.db')
cursor = conn.cursor()

# Criar pasta para exportar CSV se não existir
csv_folder = 'export_csv'
if not os.path.exists(csv_folder):
    os.makedirs(csv_folder)

print("Exportando dados para arquivos CSV...")

# Exportar Ônibus
cursor.execute("SELECT linha, numero_onibus, placa, garagem, motorista, turno FROM onibus ORDER BY linha, numero_onibus")
onibus = cursor.fetchall()

with open(os.path.join(csv_folder, 'onibus.csv'), 'w', newline='', encoding='utf-8') as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(['Linha', 'Numero', 'Placa', 'Garagem', 'Motorista', 'Turno'])
    writer.writerows(onibus)
print(f"[OK] onibus.csv - {len(onibus)} registros")

# Exportar Motoristas
cursor.execute("SELECT nome, cpf, telefone, linha_atuacao FROM motoristas ORDER BY nome")
motoristas = cursor.fetchall()

with open(os.path.join(csv_folder, 'motoristas.csv'), 'w', newline='', encoding='utf-8') as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(['Nome', 'CPF', 'Telefone', 'Linha Atuacao'])
    writer.writerows(motoristas)
print(f"[OK] motoristas.csv - {len(motoristas)} registros")

# Exportar Paradas
cursor.execute("SELECT codigo_parada, endereco FROM paradas ORDER BY codigo_parada")
paradas = cursor.fetchall()

with open(os.path.join(csv_folder, 'paradas.csv'), 'w', newline='', encoding='utf-8') as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(['Codigo', 'Endereco'])
    writer.writerows(paradas)
print(f"[OK] paradas.csv - {len(paradas)} registros")

# Exportar Linhas com detalhes
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

with open(os.path.join(csv_folder, 'linhas_paradas.csv'), 'w', newline='', encoding='utf-8') as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(['Codigo Linha', 'Codigo Parada', 'Endereco da Parada'])
    writer.writerows(linhas_detalhes)
print(f"[OK] linhas_paradas.csv - {len(linhas_detalhes)} registros")

# Criar arquivo de resumo
resumo_dados = [
    ['Categoria', 'Quantidade'],
    ['Total de Onibus', len(onibus)],
    ['Total de Motoristas', len(motoristas)],
    ['Total de Paradas', len(paradas)],
    ['Total de Linhas', len(set([linha[0] for linha in linhas_detalhes]))]
]

with open(os.path.join(csv_folder, 'resumo_geral.csv'), 'w', newline='', encoding='utf-8') as csvfile:
    writer = csv.writer(csvfile)
    writer.writerows(resumo_dados)
print(f"[OK] resumo_geral.csv - Estatisticas gerais")

conn.close()

print(f"\nTodos os arquivos foram exportados para a pasta '{csv_folder}/'")
print("Voce pode abrir estes arquivos no Excel, Google Sheets ou qualquer editor de planilhas.")
print("\nArquivos criados:")
for file in os.listdir(csv_folder):
    print(f"  - {file}")
