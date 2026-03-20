# Sistema de Controle de Trafego Urbano (BRT)

> **Status:** Em desenvolvimento

Sistema de cadastro e gerenciamento de onibus BRT (Bus Rapid Transit). Permite registrar onibus, paradas, motoristas e linhas, salvando tudo em arquivos de texto.

**Autor:** Italo

## Codigo Principal

O codigo principal do projeto foi desenvolvido na linguagem **C**.

## O que o programa faz

- **Cadastrar Onibus** — registra linha, numero, placa, garagem, motorista e turno
- **Cadastrar Parada** — registra codigo e endereco de uma parada
- **Cadastrar Motorista** — registra nome, CPF, telefone e linha de atuacao
- **Cadastrar Linha** — registra uma linha e todas as paradas que ela percorre
- **Relatorio Geral** — exibe todos os dados cadastrados na tela

## Arquivos gerados

O programa cria automaticamente os seguintes arquivos de texto para guardar os dados:

| Arquivo                   | Conteudo                  |
|---------------------------|---------------------------|
| `relatorio_brt.txt`       | Dados dos onibus          |
| `relatorio_paradas.txt`   | Dados das paradas         |
| `relatorio_motorista.txt` | Dados dos motoristas      |
| `relatorio_linhas.txt`    | Dados das linhas          |

## Estrutura do codigo

```
main.c
├── Structs
│   ├── BRT_onibus      → dados do onibus
│   ├── BRT_paradas     → dados da parada
│   ├── BRT_linhas      → relacao linha x parada
│   └── BRT_motorista   → dados do motorista
│
└── Funcoes
    ├── cadastro()           → cadastra onibus
    ├── cadastro_paradas()   → cadastra paradas
    ├── cadastro_motorista() → cadastra motoristas
    ├── cadastro_linhas()    → cadastra linhas e suas paradas
    ├── relatorio_geral()    → exibe todos os cadastros
    └── main()               → menu principal com loop do-while
```

## Tecnologias

- Linguagem C (padrão C99)
- Arquivos de texto para persistencia de dados
- Bibliotecas: `stdio.h`, `stdlib.h`, `string.h`

====================================================================================================================================
