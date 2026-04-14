from flask import Flask, render_template
import subprocess
import os

app = Flask(__name__)

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
ROOT_DIR = os.path.dirname(BASE_DIR)

EXE_PATH = os.path.join(ROOT_DIR, "main.exe")

@app.route("/")
def index():
    return render_template("index.html")


@app.route("/run")
def run_program():
    try:
        result = subprocess.run(
            [EXE_PATH],
            input="",              # <<< ISSO EVITA TRAVAMENTO
            capture_output=True,
            text=True,
            timeout=5
        )

        output = result.stdout + result.stderr

        if not output:
            output = "[SEM SAÍDA - POSSÍVEL TRAVA EM INPUT]"

        return f"<pre>{output}</pre>"

    except Exception as e:
        return f"<pre>Erro: {str(e)}</pre>"


if __name__ == "__main__":
    app.run(debug=True)