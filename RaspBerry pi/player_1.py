from flask import Flask, request, jsonify
import chess
import chess.engine
import os
import time
import requests
import threading

app = Flask(__name__)
board = chess.Board()
opponent_url = "http://localhost:5001/jogar"  # Endereço do Jogador 2

# Estado do jogo
game_running = False

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
        return result.move.uci()

@app.route('/jogar', methods=['POST'])
def jogar():
    """Recebe a jogada do oponente, faz sua jogada e envia de volta."""
    global game_running
    if not game_running:
        return jsonify(message="O jogo não está em execução.")

    fen = request.json['fen']
    board.set_fen(fen)
    print(f"\nJogador 1 recebeu a jogada do oponente. Tabuleiro FEN: {fen}")
    print(board)

    if not board.is_game_over():
        jogada = realizar_jogada(board.fen())
        board.push_uci(jogada)
        print("\nJogador 1 move:", jogada)
        print("Estado atual do tabuleiro:")
        print(board)

        # Envia a jogada para o jogador 2 com timeout e tratamento de erro
        try:
            response = requests.post(opponent_url, json={'fen': board.fen()}, timeout=5)
            if response.status_code == 200:
                print("\nJogada enviada para o Jogador 2.")
            else:
                print("Erro ao enviar jogada para o Jogador 2.")
        except requests.RequestException as e:
            print(f"Erro: {e}")

    else:
        print("O jogo acabou.")
        
    return jsonify(move=jogada)

def controle_jogo():
    """Função para controlar o estado do jogo (start/stop) via entrada do usuário."""
    global game_running
    while True:
        comando = input("Digite 'start' para iniciar o jogo, 'stop' para parar ou 'exit' para sair: ").strip().lower()
        if comando == "start" and not game_running:
            game_running = True
            print("Jogo iniciado!")
            # Faz a primeira jogada e envia para o jogador 2
            jogada = realizar_jogada(board.fen())
            board.push_uci(jogada)
            print("Jogador 1 inicia o jogo:", jogada)
            print("Estado inicial do tabuleiro:")
            print(board)

            # Envia a primeira jogada ao jogador 2
            try:
                response = requests.post(opponent_url, json={'fen': board.fen()}, timeout=5)
                if response.status_code == 200:
                    print("\nPrimeira jogada enviada para o Jogador 2.")
            except requests.RequestException as e:
                print(f"Erro na primeira jogada: {e}")
        
        elif comando == "stop" and game_running:
            parar_jogo()  # Chama a função que para o jogo
        elif comando == "exit":
            print("Saindo...")
            break  # Encerra o loop e sai


if __name__ == '__main__':
    # Inicia o thread para controle do jogo
    threading.Thread(target=controle_jogo, daemon=True).start()

    # Inicia o servidor Flask para aguardar as respostas do jogador 2
    app.run(port=5000)
