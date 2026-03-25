# 🚍 Sistema de Controle de Tráfego Urbano (BRT)

Sistema de gerenciamento e controle de operações de ônibus BRT (Bus Rapid Transit), com foco em organização de dados, simulação de operações e base para evolução em tempo real.

📄 **Documentação Detalhada:** (https://docs.google.com/document/d/1pH7ctoyV4JyCKIjl5qHV-YHrib_jUnsV4zVQYIZQsCw/edit?tab=t.0)  
🚧 **Status:** Em desenvolvimento  

👨‍💻 **Autores:** Breno Matheus, Italo Vinicius, Murilo Pussa, Yasmim Lais  

---

## 💡 Visão Geral

Este projeto simula um Sistema de Controle de Tráfego Urbano (BRT) inspirado no corredor **Via Livre Norte/Sul – Recife/PE**, com foco em:

- Cadastro e gerenciamento de entidades do sistema  
- Organização de dados com `structs` em C  
- Persistência em arquivos `.txt`  
- Base para evolução futura com GPS, CCO e tempo real  

---

## 🚀 Funcionalidades

### 📌 Cadastros

#### 🚌 Ônibus
- Linha, número, placa, garagem, motorista e turno  

#### 📍 Paradas
- Código e endereço  

#### 👨‍✈️ Motoristas
- Nome, CPF, telefone e linha de atuação  

#### 🛣 Linhas
- Associação entre linhas e suas paradas  

### 📊 Relatórios
- Exibição geral de todos os dados cadastrados via console  
- Geração automática de arquivos `.txt`  

---

## 📂 Persistência de Dados

O sistema salva automaticamente os dados nos seguintes arquivos:

| Arquivo                    | Conteúdo                  |
|---------------------------|---------------------------|
| relatorio_brt.txt         | Dados dos ônibus          |
| relatorio_paradas.txt     | Dados das paradas         |
| relatorio_motorista.txt   | Dados dos motoristas      |
| relatorio_linhas.txt      | Dados das linhas          |

---

## 🧠 Estrutura do Código

```

main.c
├── Structs
│   ├── BRT_onibus
│   ├── BRT_paradas
│   ├── BRT_linhas
│   └── BRT_motorista
│
└── Funções
├── cadastro()
├── cadastro_paradas()
├── cadastro_motorista()
├── cadastro_linhas()
├── relatorio_geral()
└── main()

````

---

## 🧱 Modelagem de Dados (C)

```c
struct BRT_onibus {
   int linha;
   int numero_onibus;
   char placa[8];
   char garagem[50];
   char motorista[50];
   char turno[20];
};

struct BRT_paradas {
   int codigo_parada;
   char endereco[100];
};

struct BRT_linhas {
   int codigo_linha;
   int codigo_parada;
};

struct BRT_motorista {
   char nome[50];
   char cpf[14];
   char telefone[15];
   int linha_atuacao;
};
````

---

## 🏙️ Arquitetura do Sistema

O sistema foi pensado com base em um modelo real de operação BRT:

### 🧭 Componentes principais

* 🚌 Ônibus (30 total)

  * 20 ativos
  * 10 reserva
* 👨‍✈️ Motoristas: 60
* 🛣 Rotas: 5
* 📍 Paradas: 75
* 🏢 Terminais: 2 (Norte e Sul)

---

## 🧠 Centro de Controle Operacional (CCO)

Módulos planejados para evolução do sistema:

* 📡 Monitoramento em tempo real (GPS)
* ⚠️ Alertas automáticos (atrasos, incidentes)
* ⏱ Controle de headway (intervalo entre ônibus)
* 🔀 Gestão de desvios de rota
* 📝 Logs operacionais

---

## 🔄 Fluxo do Sistema

```
Ônibus → envia dados (GPS/status)
   ↓
CCO → processa informações
   ↓
Instrui motorista (avisos)
   ↓
Estação / Usuário recebem dados
   ↓
Registro em log_operacao.txt
```

---

## 🧩 Arquitetura em Camadas

### 🟢 Camada 1 — Estado

* Estruturas (`structs`)
* Dados atuais do sistema

### 🟡 Camada 2 — Eventos

* Logs
* Saídas, atrasos, incidentes

### 🔴 Camada 3 — Regras

* Validações
* Alertas automáticos
* Controle de intervalos (headway)

---

## 🏃 Metodologia (Scrum)

O projeto segue uma divisão por Sprints:

### 🟢 Sprint 1 — Base Operacional

* Cadastro de ônibus, motoristas e paradas
* Controle de status

### 🟡 Sprint 2 — Motor de Controle

* Controle de horários
* Headway (intervalo entre ônibus)
* Painel do CCO

### 🔴 Sprint 3 — Inteligência e Gestão

* Alertas automáticos
* Desvio de rotas
* Relatórios avançados

---

## 🔮 Futuras Implementações

* 🌐 Versão Web
* 📱 Integração com aplicativo (usuário)
* 📍 Simulação de GPS em tempo real
* 🔔 Sistema de notificações
* 📊 Dashboard interativo

---

## 📌 Tecnologias Utilizadas

* Linguagem C (C99)
* Manipulação de arquivos `.txt`
* Estruturas de dados (`struct`)
* Lógica procedural

```
