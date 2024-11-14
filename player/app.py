from flask import Flask, request, jsonify
from configurar_partida import configurar_partida
from jogar import jogar
import chess
from config import inicializar_jogo

app = Flask(__name__)

@app.route('/configurar_partida', methods=['POST'])
def configurar():
    """Configura as variáveis de quem vai controlar as brancas e as pretas."""
    configurar_json = request.json
    controlar_brancas, controlar_pretas = configurar_partida(configurar_json)
    return jsonify({"status": "Configuração recebida com sucesso."})

@app.route('/jogar', methods=['POST'])
def jogar_jogo():
    """Recebe a FEN, inicializa o tabuleiro e faz a jogada."""
    # Inicializa as variáveis e o tabuleiro
    jogador, controlar_brancas,controlar_pretas = inicializar_jogo()
    # Recebe a FEN (estágio do tabuleiro) na requisição
    fen = request.json['fen']
    board = chess.Board(fen)  # Inicializa o tabuleiro com a FEN recebida
    
    # Chama a função jogar que processa a jogada
    board, jogador = jogar(fen, board, jogador, controlar_brancas, controlar_pretas)  # Chama a função jogar do módulo jogar
    
    # Retorna o estado atualizado
    return jsonify({"status": "Jogada processada.", "fen": board.fen()})

if __name__ == '__main__':
    app.run(port=5001)
