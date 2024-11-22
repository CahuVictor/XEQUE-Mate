from flask import Flask, request, jsonify
from threading import Thread
import jogo
import controle
from jogo import board

app = Flask(__name__)

debug = True

def mostrar_tabuleiro():
    print("\nEstado atual do tabuleiro:")
    print(board)

@app.route('/comando', methods=['POST'])
def comando():
    """Recebe comandos do jogador para controlar o jogo."""
    return jsonify({"status": "Comando inválido ou não implementado via API."}), 400

@app.route('/jogar', methods=['POST'])
def jogar():
    """Recebe a jogada do Player 1, faz a jogada e envia de volta o novo estado."""
    global jogador_atual

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
    jogo.controlar_jogadas()  # Coleta as escolhas dos jogadores
    Thread(target=controle.controlar_comandos, daemon=True).start()
    app.run(port=5000)
