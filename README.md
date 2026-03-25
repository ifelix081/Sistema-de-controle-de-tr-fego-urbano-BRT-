# Sistema de Controle de Tráfego Urbano (BRT)

> **Documentação Detalhada:** [Acesse o Google Docs aqui](https://docs.google.com/document/d/1pH7ctoyV4JyCKIjl5qHV-YHrib_jUnsV4zVQYIZQsCw/edit?usp=sharing)
> **Fluxograma:** 

> **Status:** Em desenvolvimento 🚧

Sistema de cadastro e gerenciamento de ônibus BRT (Bus Rapid Transit). Permite registrar ônibus, paradas, motoristas e linhas, salvando tudo em arquivos de texto para persistência de dados.

**Autor:** Italo

---

## 💻 Sobre o Projeto

O código principal do projeto foi desenvolvido na linguagem **C** (padrão C99) e foca na organização de dados através de estruturas (structs) e manipulação de arquivos `.txt`.

### O que o programa faz:

* **Cadastrar Ônibus:** registra linha, número, placa, garagem, motorista e turno.
* **Cadastrar Parada:** registra código e endereço de uma parada.
* **Cadastrar Motorista:** registra nome, CPF, telefone e linha de atuação.
* **Cadastrar Linha:** registra uma linha e todas as paradas que ela percorre.
* **Relatório Geral:** exibe todos os dados cadastrados diretamente no console.

---

## 📂 Arquivos Gerados

O programa cria automaticamente os seguintes arquivos para armazenar as informações:

| Arquivo | Conteúdo |
| :--- | :--- |
| `relatorio_brt.txt` | Dados dos ônibus |
| `relatorio_paradas.txt` | Dados das paradas |
| `relatorio_motorista.txt` | Dados dos motoristas |
| `relatorio_linhas.txt` | Dados das linhas |

---

## 🛠 Estrutura do Código

```text
main.c
├── Structs
│   ├── BRT_onibus      → dados do onibus
│   ├── BRT_paradas     → dados da parada
│   ├── BRT_linhas      → relacao linha x parada
│   └── BRT_motorista   → dados do motorista
│
└── Funções
    ├── cadastro()           → cadastra onibus
    ├── cadastro_paradas()   → cadastra paradas
    ├── cadastro_motorista() → cadastra motoristas
    ├── cadastro_linhas()    → cadastra linhas e suas paradas
    ├── relatorio_geral()    → exibe todos os cadastros
    └── main()               → menu principal com loop do-while
