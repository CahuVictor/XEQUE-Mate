from flask import Flask, request, jsonify
import chess
from view.view_jogar import jogar_ia, jogar_usuario, jogar_aleatorio

app = Flask(__name__)



@app.route('/jogarIa', methods=['POST'])
def jogar_jogada_ia():
    configurar_json = request.json
    
    # Recebe a FEN (estágio do tabuleiro) na requisição
    fen = configurar_json['fen']
    board = chess.Board(fen)  # Inicializa o tabuleiro com a FEN recebida
    print(board)
    # Chama a função jogar que processa a jogada, passando as variáveis de controle como parâmetros
    board = jogar_ia(fen, board)
    
    # Retorna o estado atualizado do tabuleiro
    return jsonify({
        "status": "Jogada processada.",
        "fen": board.fen()
    })

@app.route('/jogarUsuario', methods=['POST'])
def jogar_jogada_usuario():
    configurar_json = request.json
    fen = configurar_json['fen']
    board = chess.Board(fen)
    board = jogar_usuario(fen, board)
    return jsonify({
        "status": "Joagada processada",
        "fen": board.fen()
    })

@app.route('/jogarAleatoria', methods=['POST'])
def jogar_jogada_aleatoria():
    configurar_json = request.json
    fen = configurar_json['fen']
    board = chess.Board(fen)
    board = jogar_aleatorio(fen, board)
    return jsonify({
        "status": "Jogada processada",
        "fen": board.fen()
    })




if __name__ == '__main__':
    app.run(port=5001)
