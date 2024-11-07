from flask import Flask, request, jsonify
import chess
import chess.engine
import os
import time
import requests
import threading
import signal
import sys
import random

app = Flask(__name__)
board = chess.Board()
opponent_url = "http://localhost:5001/jogar"  # Endereço do Jogador 2

# Estado do jogo
game_running = False
game_paused = False  # Variável para controlar o estado de pausa
last_move = None  # Variável para armazenar a última jogada do jogador

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
        board.set_fen(fen)  # Usa o tabuleiro global e atualiza o FEN
        result = engine.play(board, chess.engine.Limit(time=2))
        return result.move.uci()

@app.route('/jogar', methods=['POST'])
def jogar():
    """Recebe a jogada do oponente, faz sua jogada e envia de volta."""
    global game_running, game_paused, board, last_move
    if not game_running or game_paused:  # Verifica se o jogo está pausado
        return jsonify(message="O jogo não está em execução ou está pausado.")

    fen = request.json['fen']
    board.set_fen(fen)
    print(f"\nJogador 1 recebeu a jogada do oponente. Tabuleiro FEN: {fen}")
    print(board)

    if not board.is_game_over():
        jogada = realizar_jogada(board.fen())
        board.push_uci(jogada)
        last_move = jogada  # Armazena a jogada feita
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
        print(board)
        resultado = board.result()
        if resultado == "1-0":
            print("Vitória das brancas!")
        elif resultado == "0-1":
            print("Vitória das pretas!")
        else:
            print("O jogo terminou em empate.")
        exit()
        
    return jsonify(move=jogada)

def controle_jogo():
    """Função para controlar o estado do jogo (start/stop) via entrada do usuário."""
    """Função para controlar o estado do jogo (start/stop/pause/retomar) via entrada do usuário."""
    while True:
        comando = input("Digite 'start' para iniciar o jogo, 'stop' para parar e sair do jogo, 'pause' para pausar e exibir os movimentos, ou 'retomar' para retomar o jogo: ").strip().lower()
        
        if comando == "start":
            if game_running:
                print("O jogo já está em andamento!")
            else:
                iniciar_jogo()
        
        elif comando == "stop":
            sair_jogo()
            break  # Sai do loop para encerrar a aplicação
        
        elif comando == "pause":
            if game_running and not game_paused:
                pausar_jogo()
            elif not game_running:
                print("O jogo ainda não começou. Use 'start' para iniciar.")
            else:
                print("O jogo já está pausado.")
        
        elif comando == "retomar":
            if game_paused:
                retomar_jogo()
            elif not game_running:
                print("O jogo ainda não começou. Use 'start' para iniciar.")
            else:
                print("O jogo já está em andamento, não é necessário retomar.")

def iniciar_jogo():
    """Função para iniciar o jogo."""
    global game_running, game_paused, board
    game_running = True
    game_paused = False  # Inicia o jogo e garante que não está pausado
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

def sair_jogo():
    """Função para sair do jogo, alterando o estado e encerrando o servidor."""
    global game_running
    if game_running:
        game_running = False
        print("Jogo encerrado! Saindo...")
        # Encerra o servidor Flask de maneira suave, sem precisar de request context
        print("Serviço encerrado.")
        os._exit(0)  # Força a saída sem exceções

def pausar_jogo():
    """Função para pausar o jogo e permitir ao jogador escolher uma jogada possível ou aleatória."""
    global game_paused, last_move
    game_paused = True  # Altera o estado para "pausado"
    
    # Exibe os movimentos válidos
    valid_moves = [move.uci() for move in board.legal_moves]
    print("Jogo pausado. Movimentos disponíveis no tabuleiro:")
    print(" ".join(valid_moves))  # Exibe todos os movimentos na mesma linha, separados por espaços
    
    # Solicita ao jogador para escolher uma jogada
    escolha = input("\nDigite '1' para escolher uma jogada ou '2' para jogar aleatoriamente: ").strip()

    if escolha == '1':
        jogada_escolhida = input("Digite a jogada escolhida (exemplo: e2e4): ").strip().lower()

        if jogada_escolhida in valid_moves:
            board.push_uci(jogada_escolhida)  # Executa a jogada escolhida
            last_move = jogada_escolhida  # Armazena a jogada feita
            print(f"Jogada executada: {jogada_escolhida}")
            print("Estado atual do tabuleiro:")
            print(board)

            # Envia a jogada para o Jogador 2
            try:
                response = requests.post(opponent_url, json={'fen': board.fen()}, timeout=5)
                if response.status_code == 200:
                    print("\nJogada enviada para o Jogador 2.")
            except requests.RequestException as e:
                print(f"Erro ao enviar jogada para o Jogador 2: {e}")
        else:
            print("Jogada inválida! Tente novamente.")
    
    elif escolha == '2':
        # Realiza uma jogada aleatória
        jogada_aleatoria = random.choice(valid_moves)
        board.push_uci(jogada_aleatoria)  # Executa a jogada aleatória
        last_move = jogada_aleatoria  # Armazena a jogada feita
        print(f"Jogada aleatória executada: {jogada_aleatoria}")
        print("Estado atual do tabuleiro:")
        print(board)

        # Envia a jogada para o Jogador 2
        try:
            response = requests.post(opponent_url, json={'fen': board.fen()}, timeout=5)
            if response.status_code == 200:
                print("\nJogada enviada para o Jogador 2.")
        except requests.RequestException as e:
            print(f"Erro ao enviar jogada para o Jogador 2: {e}")
    else:
        print("Opção inválida. Por favor, escolha '1' para uma jogada específica ou '2' para jogada aleatória.")

def retomar_jogo():
    """Função para retomar o jogo e permitir que o jogo continue a partir do estado atual."""
    global game_running, game_paused, board, last_move
    if game_paused:  # Verifica se o jogo está pausado antes de retomar
        game_paused = False  # Altera o estado para "não pausado"
        game_running = True  # Continua o jogo
        print("Jogo retomado! O jogo continuará a partir de onde parou.")

        # Verifica se a última jogada foi feita e envia ao jogador 2
        if last_move:
            print(f"Última jogada feita: {last_move}")
            print("Estado atual do tabuleiro após a última jogada:")
            print(board)

            # Envia o estado atualizado para o Jogador 2
            try:
                response = requests.post(opponent_url, json={'fen': board.fen()}, timeout=5)
                if response.status_code == 200:
                    print("\nEstado atualizado enviado para o Jogador 2.")
            except requests.RequestException as e:
                print(f"Erro ao enviar estado para o Jogador 2: {e}")
    else:
        print("O jogo não está pausado, não é necessário retomar.")

if __name__ == "__main__":
    # Inicia o servidor Flask em um thread separado
    threading.Thread(target=app.run, daemon=True, kwargs={'host': '0.0.0.0', 'port': 5000, 'debug': False}).start()
    
    # Inicia a função de controle de jogo
    controle_jogo()  