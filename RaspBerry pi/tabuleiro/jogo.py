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
tabuleiro_url_player = "http://localhost:5001/jogar"  # URL do Player

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
        response = requests.post("http://localhost:5001/configurar_partida", json=dados_configuracao)
        if response.status_code == 200:
            print("Configuração enviada com sucesso para o Player!")
        else:
            print("Erro ao enviar configuração para o Player.")
    except requests.RequestException as e:
        print(f"Erro de comunicação com o Player: {e}")

def iniciar_jogo():
    
    if not game_running:
        game_running = True
        thread = threading.Thread(target=jogar_partida)
        thread.start()
        print("Jogo iniciado!")

def jogar_partida():
    
    while game_running:
        if not game_paused:
            if board.is_game_over():
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

            time.sleep(2) 
        else:
            time.sleep(1)

def reiniciar_jogo():
    
    reiniciar_tabuleiro()
    game_running = False
    game_paused = False
    jogador_atual = "Player 1"
    print("\nO jogo foi reiniciado.")
    controlar_jogadas()
