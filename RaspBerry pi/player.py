from flask import Flask, request, jsonify
import chess
import chess.engine
import os
import requests
import time
import random

<<<<<<< Updated upstream
app = Flask(__name__)
board = chess.Board()
tabuleiro_url = "http://localhost:5000/jogar"  # Endereço do Jogador 1
=======
jogador = 1  # Define que o jogador 1 começa (brancas)

# Variáveis para controle das escolhas
controlar_brancas = None
controlar_pretas = None

app = Flask(__name__)
board = chess.Board()
tabuleiro_url = "http://localhost:5000/jogar"  # Endereço do Tabuleiro

def realizar_jogada_usuario(fen):
    board = chess.Board(fen)
    legal_moves = list(board.legal.moves)
    print(legal_moves)
    jogada_usuario = input("Escolha sua jogada valida: ")
    return jogada_usuario

def realizar_jogada_aleatoria(fen):
    """Realiza uma jogada aleatória e a aplica no tabuleiro."""
    board = chess.Board(fen)
    legal_moves = list(board.legal_moves)
    jogada_aleatoria = random.choice(legal_moves)
    return jogada_aleatoria  # Retorna a jogada aleatória
>>>>>>> Stashed changes

def obter_caminho_engine():
    """Retorna o caminho do executável do Stockfish."""
    engine_name = 'stockfish-windows-x86-64-sse41-popcnt.exe'
    script_dir = os.path.dirname(os.path.abspath(__file__))
    stockfish_dir = os.path.join(script_dir, 'Stockfish')
    return os.path.join(stockfish_dir, engine_name)

def realizar_jogada(fen):
    """Recebe um FEN do tabuleiro, calcula a melhor jogada e retorna."""
    engine_path = obter_caminho_engine()
    board = chess.Board(fen)
    with chess.engine.SimpleEngine.popen_uci(engine_path) as engine:
        result = engine.play(board, chess.engine.Limit(time=2))
        return result.move.uci()  # Retorna a jogada no formato UCI

<<<<<<< Updated upstream
=======
@app.route('/configurar_partida', methods=['POST'])
def configurar_partida():
    """Recebe as configurações de quem vai controlar as brancas e as pretas."""
    global controlar_brancas, controlar_pretas
    configurar = request.json
    controlar_brancas = configurar.get("brancas")
    controlar_pretas = configurar.get("pretas")
    
    print(f"Configuração recebida: Brancas -> {controlar_brancas}, Pretas -> {controlar_pretas}")
    
    return jsonify({"status": "Configuração recebida com sucesso."})

>>>>>>> Stashed changes
@app.route('/jogar', methods=['POST'])
def jogar():
    """Recebe a jogada do oponente, faz sua jogada e envia de volta."""
    fen = request.json['fen']
    board.set_fen(fen)
    print(f"\nJogador 2 recebeu a jogada do oponente. Tabuleiro FEN: {fen}")
    print(board)

    if not board.is_game_over():
<<<<<<< Updated upstream
        jogada = realizar_jogada(board.fen())  # Obter a jogada calculada
=======
        global jogador
        if jogador == 1:
            if controlar_brancas == 1:
                jogada = realizar_jogada(board.fen())  # Obter a jogada calculada
            elif controlar_brancas == 2:
                jogada = realizar_jogada_aleatoria(board.fen())
            elif controlar_brancas == 3:
                jogada = realizar_jogada_usuario(board.fen())
            print("Jogada das brancas.")
            jogador = 2
        elif jogador == 2:
            if controlar_pretas == 1:
                jogada = realizar_jogada(board.fen())  # Obter a jogada calculada
            elif controlar_pretas == 2:
                jogada = realizar_jogada_aleatoria(board.fen())
            elif controlar_pretas == 3:
                jogada = realizar_jogada_usuario(board.fen())
            print("Jogada das pretas.")
            jogador = 1

        #jogada = realizar_jogada(board.fen())  # Obter a jogada calculada
>>>>>>> Stashed changes
        move = chess.Move.from_uci(jogada)  # Converte a string para um objeto Move
        board.push(move)  # Aplica a jogada no tabuleiro
        print("\nJogador 2 move:", jogada)
        print("Estado atual do tabuleiro:")
        print(board)
<<<<<<< Updated upstream

=======
>>>>>>> Stashed changes
        # Envia a jogada para o jogador 1
        try:
            response = requests.post(tabuleiro_url, json={'fen': board.fen()}, timeout=5)
            if response.status_code == 200:
                print("\nJogada enviada para o tabuleiro.")
            else:
                print("Erro ao enviar jogada para o tabuleiro.")
        except requests.RequestException as e:
            print(f"Erro: {e}")

    else:
        print("O jogo acabou.")
        
    return jsonify(move=str(jogada))  # Retorna a jogada como uma string no formato UCI

if __name__ == '__main__':
<<<<<<< Updated upstream
    # Inicia o servidor Flask para aguardar as jogadas do jogador 1
=======
>>>>>>> Stashed changes
    app.run(port=5001)
