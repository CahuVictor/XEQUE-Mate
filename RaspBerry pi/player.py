from flask import Flask, request, jsonify
import chess
import chess.engine
import os
import requests
import random

app = Flask(__name__)
board = chess.Board()
tabuleiro_url = "http://localhost:5000/jogar"  # Endereço do Jogador 1
jogador = 1
controlar_brancas = None  # Inicializa a variável globalmente
controlar_pretas = None

@app.route('/configurar_partida', methods=['POST'])
def configurar_partida():
    """Recebe as configurações de quem vai controlar as brancas e as pretas."""
    global controlar_brancas, controlar_pretas
    configurar = request.json
    controlar_brancas = int(configurar.get("brancas"))  # Converte para int se necessário
    controlar_pretas = int(configurar.get("pretas"))
    
    print(f"Configuração recebida: Brancas -> {controlar_brancas}, Pretas -> {controlar_pretas}")
    
    return jsonify({"status": "Configuração recebida com sucesso."})

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

def realizar_jogada_aleatoria(fen):
    """Realiza uma jogada aleatória e a aplica no tabuleiro."""
    board = chess.Board(fen)
    legal_moves = list(board.legal_moves)
    jogada_aleatoria = random.choice(legal_moves)
    return jogada_aleatoria  # Retorna a jogada aleatória

@app.route('/jogar', methods=['POST'])
def jogar():
    """Recebe a jogada do oponente, faz sua jogada e envia de volta."""
    global jogador, controlar_brancas, controlar_pretas
    fen = request.json['fen']
    board.set_fen(fen)
    print(f"\nJogador recebeu o tabuleiro FEN. Tabuleiro FEN: {fen}")
    print(board)
    print(f"Valor de controlar_brancas: {controlar_brancas} ,Tipo: {type(controlar_brancas)}")
    print(f"Valor de controlar_pretas: {controlar_pretas} ,Tipo: {type(controlar_pretas)}")
    print(f"Valor de jogador: {jogador} ,Tipo: {type(jogador)}")

    if not board.is_game_over():
        # Confirma se jogador e controlar_brancas são do tipo int
        if jogador == 1 and controlar_brancas == 1:
            jogada = realizar_jogada(board.fen())
            move = chess.Move.from_uci(jogada)
            board.push(move)
            print("Jogada da IA:", jogada)
            jogador = 2
        elif jogador == 1 and controlar_brancas == 2:
            jogada = realizar_jogada_aleatoria(board.fen())
            move = jogada
            board.push(move)
            print("Jogada aleatoria:", jogada)
            jogador = 2
        elif jogador == 1 and controlar_brancas == 3:
            print("Entrou na condição if com controlar_brancas igual a 3 e jogador igual a 1.")
            legal_moves = list(board.legal_moves)
            print("Jogadas válidas:", [move.uci() for move in legal_moves])
            jogada_usuario = input("Escolha sua jogada válida no formato UCI: ")
            move = chess.Move.from_uci(jogada_usuario)
            if move in legal_moves:
                board.push(move) 
                print("Jogada do usuário aplicada ao tabuleiro.")
                jogador = 2
        elif jogador == 2 and controlar_pretas == 1:
            jogada = realizar_jogada(board.fen())
            move = chess.Move.from_uci(jogada)
            board.push(move)
            print("Jogada da IA:", jogada)
            jogador = 1
        elif jogador == 2 and controlar_pretas == 2:
            jogada = realizar_jogada_aleatoria(board.fen())
            move = jogada
            board.push(move)
            print("Jogada aleatoria:", jogada)
            jogador = 1
        elif jogador == 2 and controlar_pretas == 3:
            print("Entrou na condição if com controlar_brancas igual a 3 e jogador igual a 1.")
            legal_moves = list(board.legal_moves)
            print("Jogadas válidas:", [move.uci() for move in legal_moves])
            jogada_usuario = input("Escolha sua jogada válida no formato UCI: ")
            move = chess.Move.from_uci(jogada_usuario)
            if move in legal_moves:
                board.push(move) 
                print("Jogada do usuário aplicada ao tabuleiro.")
                jogador = 1
        # Enviar a jogada para o jogador 1
        try:
            response = requests.post(tabuleiro_url, json={'fen': board.fen()}, timeout=5)
            if response.status_code == 200:
                print("\nJogada enviada para o tabuleiro.")
            else:
                print("")
        except requests.RequestException as e:
            print(f"Erro: {e}")
    else:
        print("O jogo acabou.")
        
    return jsonify({"status": "Jogada processada."})

if __name__ == '__main__':
    # Inicia o servidor Flask para aguardar as jogadas do jogador 1
    app.run(port=5001)