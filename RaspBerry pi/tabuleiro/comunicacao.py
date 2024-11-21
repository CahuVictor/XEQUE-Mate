import requests
from flask import jsonify
import jogo

tabuleiro_url_player = "http://localhost:5001/jogar"

def comando():
    """Recebe comandos do jogador para controlar o jogo."""
    return jsonify({"status": "Comando inválido ou não implementado via API."}), 400

def jogar():
    """Recebe a jogada do Player 1, faz a jogada e envia de volta o novo estado."""
    fen = requests.json['fen']
    jogo.board.set_fen(fen)

    if 'jogada' in requests.json:
        jogada = requests.json['jogada']
        try:
            jogo.board.push_uci(jogada)
            return jsonify({'fen': jogo.board.fen(), 'status': 'jogada realizada'})
        except ValueError:
            return jsonify({"status": "Jogada inválida."}), 400
    return jsonify({"status": "Jogada não fornecida."}), 400
