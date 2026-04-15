🚌 Sistema BRT Integrado
📌 Descrição do Projeto

O Sistema BRT Integrado é uma aplicação desenvolvida em linguagem C com o objetivo de simular e gerenciar operações de transporte urbano no modelo BRT (Bus Rapid Transit). O sistema contempla diferentes níveis de acesso e funcionalidades, permitindo o controle operacional de ônibus, motoristas, linhas, paradas e comunicação em tempo real entre motoristas e a central de controle.

O projeto foi concebido com foco em organização de dados, rastreabilidade de eventos e suporte à tomada de decisão operacional.

🎯 Objetivo Geral

Desenvolver um sistema modular para gerenciamento de operações de transporte público, proporcionando:

Controle de cadastro de recursos (ônibus, motoristas, linhas e paradas)
Comunicação estruturada entre motoristas e central
Monitoramento de jornadas
Registro e acompanhamento de incidentes
🧠 Objetivos Específicos
Implementar autenticação por níveis de acesso
Estruturar persistência de dados via arquivos .csv
Garantir integridade básica dos dados com validações
Simular um ambiente real de operação de transporte urbano
Permitir rastreamento completo de ocorrências via protocolo
👥 Perfis de Usuário
🔐 Administrador

Responsável pela gestão estrutural do sistema:

Cadastro de ônibus
Cadastro de motoristas
Cadastro de paradas
Cadastro de linhas
Emissão de relatórios gerais
🚍 Motorista

Responsável pela operação diária:

Início de jornada
Envio de avisos (incidentes)
Consulta de respostas da central
Visualização do histórico da jornada
Encerramento da jornada
🎧 Controle Central

Responsável pela supervisão operacional:

Visualização de avisos pendentes
Resposta a incidentes
Monitoramento de motoristas ativos
Consulta de histórico completo
Busca por protocolos
⚙️ Funcionalidades Principais
📋 Cadastro de Dados
Ônibus (dados_onibus.csv)
Motoristas (dados_motoristas.csv)
Paradas (dados_paradas.csv)
Linhas (dados_linhas.csv)
🚨 Sistema de Avisos
Registro de ocorrências com:
Tipo
Gravidade
Localização
Descrição
Geração automática de protocolo único
Persistência em avisos.csv
📡 Comunicação com a Central
Respostas registradas em respostas.csv
Associação via protocolo
Status de acompanhamento:
Pendente
Respondido
⏱️ Controle de Sessão
Registro de jornadas em sessoes.csv
Controle de início e fim
Identificação de sessões ativas
📊 Relatórios
Leitura consolidada de todos os dados
Exibição direta no terminal
Visão geral do sistema
🗂️ Estrutura de Dados

O sistema utiliza estruturas (struct) para modelagem:

Onibus → Dados de veículos
Motorista → Informações pessoais e operacionais
Linha → Relação entre linhas e paradas
Parada → Localizações
Aviso → Ocorrências geradas
Resposta → Ações da central
Sessao → Controle de jornada
💾 Persistência de Dados

O sistema utiliza arquivos .csv como armazenamento:

Arquivo	Conteúdo
dados_onibus.csv	Cadastro de ônibus
dados_motoristas.csv	Cadastro de motoristas
dados_paradas.csv	Cadastro de paradas
dados_linhas.csv	Estrutura das linhas
avisos.csv	Ocorrências registradas
respostas.csv	Respostas da central
sessoes.csv	Jornadas dos motoristas
🔐 Autenticação
Administrador
Usuário: admin
Senha: 1234
Operador
Usuário: oper01 / sup01
Senhas: brt2024 / sup2024
🧪 Validações Implementadas
CPF com 11 dígitos numéricos
Placa de veículo (mínimo de caracteres)
Campos obrigatórios não vazios
Controle de tentativas de entrada
Sanitização de strings (trim)
🎨 Interface
Interface via terminal (CLI)
Uso de cores ANSI para destaque:
Verde → Sucesso
Vermelho → Erro/Crítico
Amarelo → Atenção
Ciano → Informativo
🔄 Fluxo Geral do Sistema
Usuário seleciona tipo de acesso
Realiza autenticação
Acessa menu específico
Executa operações
Dados são persistidos em arquivos
Sistema pode integrar com banco externo via script Python
🔗 Integração Externa

Ao encerrar o sistema:

python src/output/db_import.py

Essa etapa sugere integração com banco de dados (ex: SQLite), permitindo evolução do sistema para um modelo mais robusto.

🛠️ Tecnologias Utilizadas
Linguagem C (estrutura procedural)
Manipulação de arquivos (stdio.h)
Strings (string.h)
Tempo (time.h)
Validações (ctype.h)
Integração com Python (extensão futura)
