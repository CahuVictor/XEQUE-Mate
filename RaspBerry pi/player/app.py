from flask import Flask, request, jsonify
import chess
from configurar_partida import configurar_partida  # A função configurar_partida, caso você precise
from jogar import jogar_ia, jogar_usuario, jogar_aleatorio

app = Flask(__name__)

# Definindo a classe Partida diretamente no mesmo script
class Partida:
    def __init__(self):
        self.controlar_brancas = None
        self.controlar_pretas = None

    def configurar(self, brancas, pretas):
        """Configura quem controla as brancas e quem controla as pretas."""
        self.controlar_brancas = brancas
        self.controlar_pretas = pretas

    def obter_configuracao(self):
        """Retorna as configurações de quem vai controlar as brancas e as pretas."""
        return self.controlar_brancas, self.controlar_pretas


# Instância global da classe Partida
partida = Partida()

@app.route('/configurar_partida', methods=['POST'])
def configurar():
    """Configura as variáveis de quem vai controlar as brancas e as pretas."""
    configurar_json = request.json
    brancas = int(configurar_json.get("brancas"))
    pretas = int(configurar_json.get("pretas"))
    
    # Configura as variáveis no objeto da partida
    partida.configurar(brancas, pretas)
    
    # Retorna a configuração como um JSON
    return jsonify({
        "controlar_brancas": brancas,
        "controlar_pretas": pretas,
        "status": "Configuração recebida com sucesso."
    })

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
