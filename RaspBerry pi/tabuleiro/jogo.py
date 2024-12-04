import threading
import time
import random
import requests
import chess
from tabuleiro import mostrar_tabuleiro, reiniciar_tabuleiro

# Variáveis de controle do jogo
board = chess.Board()
game_running = False
game_paused = False
jogador_atual = "Player 1"  # Controla qual jogador está jogando (alternando entre Player 1 e Player 2)
tabuleiro_url_player = "http://localhost:5003/receber_tabuleiro"  # URL do Player

# Variáveis para controlar quem joga com as brancas e quem joga com as pretas
controlar_brancas = None
controlar_pretas = None

def controlar_jogadas():
    """Solicita ao usuário quem controlará as brancas e quem controlará as pretas."""
    
    controlar_brancas = input("Quem irá controlar as peças brancas? (1 para IA, 2 para Jogada Aleatória, 3 para Usuário): ")
    controlar_pretas = input("Quem irá controlar as peças pretas? (1 para IA, 2 para Jogada Aleatória, 3 para Usuário): ")

    # Enviar as informações ao player antes de iniciar o jogo Json
    dados_configuracao = {
        "brancas": controlar_brancas,
        "pretas": controlar_pretas
    }
    try:
        response = requests.post("http://localhost:5003/controle_peças", json=dados_configuracao)
        if response.status_code == 200:
            print("Configuração enviada com sucesso para o Controle!")
        else:
            print("Erro ao enviar configuração para o Player.")
    except requests.RequestException as e:
        print(f"Erro de comunicação com o Player: {e}")

def jogar_partida():
    
    global game_running, game_paused, jogador_atual
    while game_running:
        if not game_paused:
            if board.is_game_over():
                response = requests.post('http://localhost:5003/jogoFinalizado')
                print(response.json())
                print("O jogo acabou.")
                resultado = board.result()
                if resultado == "1-0":
                    print("Vitória das brancas!")
                elif resultado == "0-1":
                    print("Vitória das pretas!")
                else:
                    print("O jogo terminou em empate.")
                game_running = False
                break

            try:
                response = requests.post(tabuleiro_url_player, json={'fen': board.fen()})
                if response.status_code == 200:
                    print(f"\nJogada enviada para {jogador_atual}")
                    mostrar_tabuleiro(board)
                    jogador_atual = "Player 2" if jogador_atual == "Player 1" else "Player 1"
                else:
                    print(f"Erro ao enviar jogada para {jogador_atual}.")
            except requests.RequestException as e:
                print(f"Erro ao comunicar com {jogador_atual}: {e}")

            time.sleep(2)  # Atraso entre as jogadas para simular o ritmo do jogo 
        else:
            time.sleep(1)  # Pausa o loop enquanto o jogo estiver pausado
            time.sleep(2)  # Atraso entre as jogadas para simular o ritmo do jogo

def iniciar_jogo():
    
    global game_running
    if not game_running:
        game_running = True
        thread = threading.Thread(target=jogar_partida)
        thread.start()
        print("Jogo iniciado!")  
    else:
        print("O jogo está em andamento!")              

def exibir_lances():
    global game_paused
    game_paused = True  # Pausa o jogo enquanto exibe os lances

    # Lista todos os lances possíveis
    lances_possiveis = [move.uci() for move in board.legal_moves]
    print("Lances possíveis:")
    for i, lance in enumerate(lances_possiveis, 1):
        print(f"{i}. {lance}", end="  ")
    print("\n")

    # Opções para o jogador
    escolha = input("Escolha uma opção:\n1 - Jogada aleatória\n2 - Jogada específica\n3 - Desistir do lance\nDigite sua escolha: ").strip()

    if escolha == '1':  # Jogada aleatória
        lance_aleatorio = random.choice(lances_possiveis)
        board.push_uci(lance_aleatorio)
        print(f"Jogada aleatória executada: {lance_aleatorio}")
        mostrar_tabuleiro(board)

    elif escolha == '2':  # Jogada específica
        lance_especifico = input("Digite a jogada específica (exemplo: e2e4): ").strip().lower()
        try:
            # Cria o movimento a partir do input do usuário
            movimento = chess.Move.from_uci(lance_especifico)
            if movimento in board.legal_moves:
                board.push(movimento)  # Executa o movimento no tabuleiro
                print(f"Jogada específica executada: {lance_especifico}")
                mostrar_tabuleiro(board)
            else:
                print("Jogada inválida! A jogada não é permitida no estado atual do tabuleiro.")
        except ValueError:
            print("Formato de jogada inválido! Por favor, digite a jogada no formato UCI (exemplo: e2e4).")

    elif escolha == '3':  # Desistir do lance
        print("Desistindo do lance. Retornando ao jogo normalmente.")
    
    else:
        print("Opção inválida. Voltando ao jogo sem realizar jogada.")

    # Retoma o jogo
    game_paused = False      


# Função para finalizar o jogo com derrota do jogador atual
def desistir():
    global game_running
    vencedor = "Player 2" if jogador_atual == "Player 1" else "Player 1"
    game_running = False  # Para o jogo
    print(f"{jogador_atual} desistiu! Vitória de {vencedor}.")          

def reiniciar_jogo():
    
    global board, game_running, game_paused, jogador_atual
    board = chess.Board() # Redefine o tabuleiro para o estado inicial
    game_running = False # Reseta o status de execução do jogo
    game_paused = False  # Garante que o jogo não esteja pausado
    jogador_atual = "Player 1"  # Define o jogador inicial como Player 1
    print("\nO jogo foi reiniciado.")  
    controlar_jogadas() # Coleta novamente as escolhas dos jogadores