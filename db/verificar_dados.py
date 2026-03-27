import sqlite3

# Conectar ao banco de dados
conn = sqlite3.connect('sisbus.db')
cursor = conn.cursor()

print("=== VERIFICACAO DE DADOS IMPORTADOS ===\n")

# Verificar ônibus
print("ONIBUS:")
cursor.execute("SELECT * FROM onibus")
onibus = cursor.fetchall()
for bus in onibus:
    print(f"Linha: {bus[0]}, Numero: {bus[1]}, Placa: {bus[2]}, Garagem: {bus[3]}, Motorista: {bus[4]}, Turno: {bus[5]}")
print(f"Total de onibus: {len(onibus)}\n")

# Verificar motoristas
print("MOTORISTAS:")
cursor.execute("SELECT * FROM motoristas")
motoristas = cursor.fetchall()
for motorista in motoristas:
    print(f"Nome: {motorista[0]}, CPF: {motorista[1]}, Telefone: {motorista[2]}, Linha: {motorista[3]}")
print(f"Total de motoristas: {len(motoristas)}\n")

# Verificar paradas
print("PARADAS:")
cursor.execute("SELECT * FROM paradas")
paradas = cursor.fetchall()
for parada in paradas:
    print(f"Codigo: {parada[0]}, Endereco: {parada[1]}")
print(f"Total de paradas: {len(paradas)}\n")

# Verificar linhas
print("LINHAS:")
cursor.execute("SELECT * FROM linhas")
linhas = cursor.fetchall()
for linha in linhas:
    print(f"Codigo Linha: {linha[0]}, Codigo Parada: {linha[1]}")
print(f"Total de relacoes linha-parada: {len(linhas)}\n")

conn.close()
print("Verificação concluída!")
