# XEQUE-Mate
 Repositório do projeto XEQUE-Mate executado na escola ETE do Porto Digital

Este projeto tem como objetivo desenvolver um jogo de xadrez inteligente e interativo para ensinar pessoas a jogar xadrez. Utilizando impressão 3D e cortadora a laser para fabricação das peças e tabuleiro, adicionando a Sistemas Embarcados com ESP32 e Raspberry Pi 3 para monitorar/controlar LEDs, Display e outros sensores. Com capacidade de identificar a jogada realizada e fornecer feedback visual e textual sobre os movimentos realizados, ajudando os jogadores a compreender estratégias e técnicas do jogo. O projeto terá uma IA integrada na Raspberry Pi 3 para realizar avaliações, sugerir jogadas, e todas as etapas do projeto relacionadas a tomada de decisão, deixando a ESP32 incubida de executar as tarefas solicitadas pela IA, a IA terá como missão promover uma experiência educacional dinâmica e envolvente.

## Objetivos do Projeto
* Criar um tabuleiro com feedback visual e textual, assistido por IA.
* Desenvolver habilidades técnicas em impressão 3D, corte a laser e programação.
* Fomentar o aprendizado de xadrez de maneira didática e interativa.
* Capacitações e Workshops
* O projeto inclui workshops sobre a utilização de tecnologias como ESP32, Raspberry Pi, LEDs e displays para alunos e professores, permitindo a compreensão prática das ferramentas e habilidades aplicadas.

## Melhorias Futuras
* Aplicativo complementar para análise de jogos e tutoriais.
* Parcerias com clubes e instituições para ampliar o impacto do projeto.
* Comunidade online para promover a interação entre os usuários.
* Resultados Esperados
* Tabuleiro de xadrez inteligente funcional.
* Documentação técnica detalhada.
* Relatórios de capacitação e divulgação.

## Padronização dos commits
* feat: uma nova feature (recurso) que você está adicionando a uma aplicação específica ;
* fix: a resolução de um bug;
* style: recurso e atualizações relacionadas à estilização;
* refactor: refatoração de uma seção específica da base de código;
* test: tudo o que for relacionado a testes;
* docs: tudo o que for relacionado à documentação;
* chore: manutenção regular do código.
<<<<<<< HEAD
=======

## Estrutura de arquivos
XEQUE-Mate/

├── src/

│   ├── __init__.py

│   ├── main.py

│   ├── chess_engine.py

│   └── ... (outros módulos)

├── tests/

│   ├── __init__.py

│   ├── test_chess_engine.py

│   └── ... (outros arquivos de teste)

├── stockfish/

│   └── stockfish-windows-x86-64-sse41-popcnt.exe (ou o executável apropriado para o seu sistema)

├── data/

│   └── ... (arquivos de dados, se houver)

├── docs/

│   └── LICENSE

│   └── ... (documentação do projeto)

├── .gitignore

├── README.md

├── requirements.txt

└── setup.py (se você planeja empacotar seu projeto)


src/:
    
    Contém o código-fonte principal do seu aplicativo. Seguir as convenções do PEP 8 para escrever um código Python legível e consistente. Manter funções e métodos curtos e com responsabilidade única.
    
tests/:
    
    Inclui todos os testes automatizados do seu projeto. Verificar o uso de frameworks de teste como unittest (integrado ao Python) ou pytest para facilitar a escrita e execução dos testes.
    
stockfish/:

    Armazena o executável do Stockfish.
    
data/:

    Contém arquivos de dados usados pelo seu aplicativo, como arquivos PGN, configurações ou recursos adicionais.
    
docs/:

    Armazena a documentação do seu projeto. Considerar usar ferramentas como Sphinx para gerar documentação automática.
    

Arquivos de Configuração e Documentação:

    .gitignore: Lista de arquivos e pastas que o Git deve ignorar (por exemplo, arquivos temporários, ambientes virtuais).
    
    README.md: Descrição geral do projeto, instruções de instalação e uso.
    
    requirements.txt: Lista de dependências Python que podem ser instaladas com pip install -r requirements.txt. Sempre que adicionar uma nova dependência, atualizar o requirements.txt usando pip freeze > requirements.txt.
    
    setup.py: Arquivo de configuração para empacotar e distribuir seu projeto (opcional).
    
>>>>>>> stockfish

##