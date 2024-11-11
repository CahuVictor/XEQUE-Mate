from flask import Flask, request, jsonify
import chess
import chess.engine
import os
import requests
import time
import random

app = Flask(__name__)
board = chess.Board()
tabuleiro_url = "http://localhost:5000/jogar"  # Endereço do Jogador 1

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

@app.route('/jogar', methods=['POST'])
def jogar():
    """Recebe a jogada do oponente, faz sua jogada e envia de volta."""
    fen = request.json['fen']
    board.set_fen(fen)
    print(f"\nJogador 2 recebeu a jogada do oponente. Tabuleiro FEN: {fen}")
    print(board)

    if not board.is_game_over():
        jogada = realizar_jogada(board.fen())  # Obter a jogada calculada
        move = chess.Move.from_uci(jogada)  # Converte a string para um objeto Move
        board.push(move)  # Aplica a jogada no tabuleiro
        print("\nJogador 2 move:", jogada)
        print("Estado atual do tabuleiro:")
        print(board)
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
    # Inicia o servidor Flask para aguardar as jogadas do jogador 1
    app.run(port=5001)