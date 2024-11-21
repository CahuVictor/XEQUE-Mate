from flask import Flask, request, jsonify
import chess
from configurar_partida import configurar_partida  # A função configurar_partida, caso você precise
from jogar import jogar

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

@app.route('/jogar', methods=['POST'])
def jogar_jogo():
    """Recebe a FEN, inicializa o tabuleiro e faz a jogada."""
    # Recebe a FEN (estágio do tabuleiro) e as variáveis de controle de brancas e pretas da requisição
    configurar_json = request.json
    
    # Obtém as variáveis de controle da partida através da instância global
    controlar_brancas, controlar_pretas = partida.obter_configuracao()

    # Verifica se as variáveis de controle foram configuradas
    if controlar_brancas is None or controlar_pretas is None:
        return jsonify({"error": "As variáveis de controle de brancas e pretas não foram configuradas."}), 400

    jogador = 1
    
    # Recebe a FEN (estágio do tabuleiro) na requisição
    fen = configurar_json['fen']
    board = chess.Board(fen)  # Inicializa o tabuleiro com a FEN recebida
    
    # Chama a função jogar que processa a jogada, passando as variáveis de controle como parâmetros
    board, jogador = jogar(fen, board, jogador, controlar_brancas, controlar_pretas)
    
    # Retorna o estado atualizado do tabuleiro
    return jsonify({
        "status": "Jogada processada.",
        "fen": board.fen()
    })

if __name__ == '__main__':
    app.run(port=5001)
