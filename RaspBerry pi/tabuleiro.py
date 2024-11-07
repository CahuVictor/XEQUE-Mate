from flask import Flask, request, jsonify
import chess
import time
import requests
import threading

app = Flask(__name__)

debug = False

# Variáveis de controle do jogo
board = chess.Board()
game_running = False
game_paused = False
tabuleiro_url_player1 = "http://localhost:5001/jogar"  # URL do Jogador 1
tabuleiro_url_player2 = "http://localhost:5002/jogar"  # URL do Jogador 2

# Função para mostrar o tabuleiro
def mostrar_tabuleiro():
    print("\nEstado atual do tabuleiro:")
    print(board)

# Função para gerenciar a partida
def jogar_partida():
    global game_running, game_paused
    while game_running:
        if not game_paused:
            # Verifica se o jogo acabou antes de tentar enviar jogadas
            if board.is_game_over():
                print("O jogo acabou.")
                resultado = board.result()
                if resultado == "1-0":
                    print("Vitória das brancas!")
                elif resultado == "0-1":
                    print("Vitória das pretas!")
                else:
                    print("O jogo terminou em empate.")
                game_running = False  # Interrompe o jogo quando ele acabar
                break  # Sai do loop

            # Envia a jogada para o Player 1
            try:
                response = requests.post(tabuleiro_url_player1, json={'fen': board.fen()})
                if response.status_code == 200:
                    print("\nJogada enviada para o Player 1")
                    mostrar_tabuleiro()
                else:
                    print("Erro ao enviar jogada para o Player 1.")
            except requests.RequestException as e:
                print(f"Erro ao comunicar com Player 1: {e}")

            time.sleep(2)  # Atraso entre as jogadas para simular o ritmo do jogo

            # Espera pela jogada do Player 2, processa e envia para o Player 2
            try:
                response = requests.post(tabuleiro_url_player2, json={'fen': board.fen()})
                if response.status_code == 200:
                    print("\nJogada enviada para o Player 2")
                    mostrar_tabuleiro()
                else:
                    print("Erro ao enviar jogada para o Player 2.")
            except requests.RequestException as e:
                print(f"Erro ao comunicar com Player 2: {e}")
            
            time.sleep(2)  # Atraso entre as jogadas para simular o ritmo do jogo
        else:
            time.sleep(1)  # Pausa o loop enquanto o jogo estiver pausado

# Função para iniciar o jogo
def iniciar_jogo():
    global game_running
    game_running = True
    thread = threading.Thread(target=jogar_partida)
    thread.start()

# Função para controlar os comandos do console
def controlar_comandos():
    global game_running, game_paused
    while True:
        comando = input("\nDigite um comando (start, pause, resume, stop): ").strip().lower()
        
        if comando == 'start' and not game_running:
            print("Iniciando o jogo...")
            iniciar_jogo()
        elif comando == 'pause' and game_running:
            game_paused = True
            print("Jogo pausado.")
        elif comando == 'resume' and game_paused:
            game_paused = False
            print("Jogo retomado.")
        elif comando == 'stop' and game_running:
            game_running = False
            print("Jogo finalizado.")
            break
        else:
            print("Comando inválido ou jogo já no estado desejado.")

@app.route('/comando', methods=['POST'])
def comando():
    """Recebe comandos do jogador para controlar o jogo."""
    return jsonify({"status": "Comando inválido ou não implementado via API."}), 400

@app.route('/jogar', methods=['POST'])
def jogar():
    """Recebe a jogada do oponente, faz a jogada e envia de volta."""
    fen = request.json['fen']
    board.set_fen(fen)  # Atualiza o tabuleiro com o FEN recebido
    print(f"\nJogador 1 ou 2 recebeu a jogada. Tabuleiro FEN: {fen}")
    if debug:
        mostrar_tabuleiro()

    # Retorna a jogada para o próximo jogador
    if not board.is_game_over():
        jogada = request.json.get('jogada', None)
        if jogada:
            if "Player 1" in request.json['fen']:
                response = requests.post(tabuleiro_url_player2, json={'fen': board.fen()})
            else:
                response = requests.post(tabuleiro_url_player1, json={'fen': board.fen()})
            
            if response.status_code == 200:
                print("\nJogada enviada para o próximo jogador.")
            else:
                print("Erro ao enviar jogada para o próximo jogador.")
    else:
        print("O jogo acabou.")
        resultado = board.result()
        if resultado == "1-0":
            print("Vitória das brancas!")
        elif resultado == "0-1":
            print("Vitória das pretas!")
        else:
            print("O jogo terminou em empate.")
    return jsonify({'fen': board.fen(), 'status': 'jogada realizada'})

if __name__ == '__main__':
    # Inicia o servidor Flask para o tabuleiro
    threading.Thread(target=controlar_comandos, daemon=True).start()
    app.run(port=5000)
