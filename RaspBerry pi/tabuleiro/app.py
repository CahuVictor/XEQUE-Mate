from flask import Flask
from threading import Thread
import jogo
import comunicacao
import controle

app = Flask(__name__)

@app.route('/comando', methods=['POST'])
def comando():
    """Recebe comandos do jogador para controlar o jogo."""
    return comunicacao.comando()

@app.route('/jogar', methods=['POST'])
def jogar():
    """Recebe a jogada do Player 1, faz a jogada e envia de volta o novo estado."""
    return comunicacao.jogar()

if __name__ == '__main__':
    # Inicia o servidor Flask para o tabuleiro
    jogo.controlar_jogadas()  # Coleta as escolhas dos jogadores
    Thread(target=controle.controlar_comandos, daemon=True).start()
    app.run(port=5000)
