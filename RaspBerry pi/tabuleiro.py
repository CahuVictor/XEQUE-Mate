from flask import Flask, request, jsonify
import chess
import time
import threading
import requests
import random

app = Flask(__name__)

debug = True

# Variáveis de controle do jogo
board = chess.Board()
print(board)

game_running = False
game_paused = False

jogador_atual = "Player 1"  # Controla qual jogador está jogando (alternando entre Player 1 e Player 2)
tabuleiro_url_player = "http://localhost:5001/jogar"  # URL do Player, que agora fará todas as jogadas

# Variáveis para controlar quem joga com as brancas e quem joga com as pretas
controlar_brancas = None
controlar_pretas = None

def controlar_jogadas():
    """Solicita ao usuário quem controlará as brancas e quem controlará as pretas."""
    global controlar_brancas, controlar_pretas
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

# Função para mostrar o tabuleiro
def mostrar_tabuleiro():
    print("\nEstado atual do tabuleiro:")
    print(board)

# Função para gerenciar a partida
def jogar_partida():
    global game_running, game_paused, jogador_atual
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

            # Envia o estado do tabuleiro para o Player 1 para ele fazer a jogada
            try:
                # Envia o estado do tabuleiro para o Player 1 para ele fazer a jogada
                response = requests.post(tabuleiro_url_player, json={'fen': board.fen()})
                if response.status_code == 200:
                    print(f"\nJogada enviada para {jogador_atual}")
                    mostrar_tabuleiro()
                    # Alterna para o próximo jogador
                    jogador_atual = "Player 2" if jogador_atual == "Player 1" else "Player 1"
                else:
                    print(f"Erro ao enviar jogada para {jogador_atual}.")
            except requests.RequestException as e:
                print(f"Erro ao comunicar com {jogador_atual}: {e}")

            time.sleep(2)  # Atraso entre as jogadas para simular o ritmo do jogo    
        else:
            time.sleep(1)  # Pausa o loop enquanto o jogo estiver pausado
            time.sleep(2)  # Atraso entre as jogadas para simular o ritmo do jogo

# Função para iniciar o jogo
def iniciar_jogo():
    global game_running
    if not game_running:
        game_running = True
        thread = threading.Thread(target=jogar_partida)
        thread.start()
        print("Jogo iniciado!")
    else:
        print("O jogo já está em andamento!")

# Função para exibir lances possíveis e permitir escolhas
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
        mostrar_tabuleiro()

    elif escolha == '2':  # Jogada específica
        lance_especifico = input("Digite a jogada específica (exemplo: e2e4): ").strip().lower()
        try:
            # Cria o movimento a partir do input do usuário
            movimento = chess.Move.from_uci(lance_especifico)
            if movimento in board.legal_moves:
                board.push(movimento)  # Executa o movimento no tabuleiro
                print(f"Jogada específica executada: {lance_especifico}")
                mostrar_tabuleiro()
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

# Função para controlar os comandos do console
def controlar_comandos():
    global game_running, game_paused
    while True:
        comando = input("\nDigite um comando (start, pause, resume, stop, lances, desistir, reiniciar): ").strip().lower()
        
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
        elif comando == 'lances' and game_running:
            exibir_lances()
        elif comando == 'desistir' and game_running:
            desistir()   
        elif comando == 'reiniciar' and not game_running:
            reiniciar_jogo() # Reinicia o jogo, se ele já foi finalizado    
        else:
            print("Comando inválido ou jogo já no estado desejado.")

@app.route('/comando', methods=['POST'])
def comando():
    """Recebe comandos do jogador para controlar o jogo."""
    return jsonify({"status": "Comando inválido ou não implementado via API."}), 400

@app.route('/jogar', methods=['POST'])
def jogar():
    """Recebe a jogada do Player 1, faz a jogada e envia de volta o novo estado."""
    fen = request.json['fen']
    board.set_fen(fen)  # Atualiza o tabuleiro com o FEN recebido
    print(f"\n{jogador_atual} recebeu o FEN: {fen}")

    if debug:
        mostrar_tabuleiro()

    # O Player 1 realiza a jogada
    jogada = request.json.get('jogada', None)
    if jogada:
        try:
            board.push_uci(jogada)  # Executa a jogada recebida
            print(f"Jogada feita por {jogador_atual}: {jogada}")
            mostrar_tabuleiro()

            # Retorna o novo estado do tabuleiro após a jogada
            return jsonify({'fen': board.fen(), 'status': 'jogada realizada'})
        except ValueError as e:
            print(f"Erro ao realizar jogada: {e}")
            return jsonify({"status": "Jogada inválida."}), 400
    else:
        return jsonify({"status": "Jogada não fornecida."}), 400


if __name__ == '__main__':
    # Inicia o servidor Flask para o tabuleiro
    controlar_jogadas()  # Coleta as escolhas dos jogadores
    threading.Thread(target=controlar_comandos, daemon=True).start()
    app.run(port=5000)
