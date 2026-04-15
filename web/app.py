from flask import Flask, render_template, request
import sqlite3
import os


# CONFIGURAÇÕES DO PROJETO

app = Flask(
    __name__,
    static_folder="static",
    static_url_path="/static",
    template_folder="templates",
)

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
ROOT_DIR = os.path.dirname(BASE_DIR)

DB_PATH = os.path.join(ROOT_DIR, "src", "db", "brt.db")


# BANCO DE DADOS

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


def buscar_parada_por_id(id_estacao):
    conn = conectar_banco()
    cursor = conn.cursor()

    query = "SELECT * FROM paradas WHERE id = ?"
    cursor.execute(query, (id_estacao,))

    parada = cursor.fetchone()
    conn.close()
    return parada


# ROTAS

@app.route("/")
def index():
    return render_template("index.html")


@app.route("/consultar_onibus")
def consultar_onibus():
    linha = request.args.get("linha", type=int)

    if linha is None:
        return render_template(
            "index.html",
            erro="Por favor, informe o número da linha antes de consultar.",
        )

    resultados = buscar_onibus_por_linha(linha)
    return render_template(
        "resultado.html",
        resultados=resultados,
        linha=linha,
    )


@app.route("/buscar_estacao")
def buscar_estacao():
    id_estacao = request.args.get("id_estacao", type=int)

    if id_estacao is None:
        return render_template(
            "index.html",
            erro="Por favor, informe o ID da estação antes de consultar.",
        )

    parada = buscar_parada_por_id(id_estacao)
    return render_template(
        "resultado_parada.html",
        parada=parada,
        id_estacao=id_estacao,
    )


# EXECUÇÃO

if __name__ == "__main__":
    app.run(debug=True)