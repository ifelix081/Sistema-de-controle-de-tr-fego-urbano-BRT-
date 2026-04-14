from flask import Flask, render_template, request
import sqlite3
import os


# =========================
# CONFIGURAÇÕES DO PROJETO
# =========================
app = Flask(__name__)

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
ROOT_DIR = os.path.dirname(BASE_DIR)

DB_PATH = os.path.join(ROOT_DIR, "src", "db", "brt.db")


# =========================
# BANCO DE DADOS
# =========================
def conectar_banco():
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    return conn


def buscar_onibus_por_linha(linha):
    conn = conectar_banco()
    cursor = conn.cursor()

    query = "SELECT * FROM onibus WHERE linha = ?"
    cursor.execute(query, (linha,))

    resultados = cursor.fetchall()
    conn.close()

    return resultados


# =========================
# ROTAS
# =========================
@app.route("/")
def index():
    return render_template("index.html")


@app.route("/consultar_onibus")
def consultar_onibus():
    linha = request.args.get("linha")

    resultados = buscar_onibus_por_linha(linha)

    return render_template(
        "resultado.html",
        resultados=resultados,
        linha=linha
    )


# =========================
# EXECUÇÃO
# =========================
if __name__ == "__main__":
    app.run(debug=True)