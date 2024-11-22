import requests
from flask import jsonify
import jogo

tabuleiro_url_player = "http://localhost:5001/jogar"

def comando():
    """Recebe comandos do jogador para controlar o jogo."""
    return jsonify({"status": "Comando inválido ou não implementado via API."}), 400

def jogar():
    """Recebe a jogada do Player 1, faz a jogada e envia de volta o novo estado."""
    try:
        data = requests.get_json()  # Use o método correto para pegar o JSON do corpo da requisição
        fen = data['fen']
        jogo.board.set_fen(fen)

        if 'jogada' in data:
            jogada = data['jogada']
            try:
                jogo.board.push_uci(jogada)
                return jsonify({'fen': jogo.board.fen(), 'status': 'jogada realizada'})
            except ValueError:
                return jsonify({"status": "Jogada inválida."}), 400
        return jsonify({"status": "Jogada não fornecida."}), 400
    except Exception as e:
        return jsonify({"error": str(e)}), 400
