from flask import Flask, request, jsonify
import chess


app = Flask(__name__)

player_url_jogada_usuario =
player_url_jogada_ia =
player_url_jogada_aleatoria =
tabuleiro_url = "http://localhost:5000/"



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

partida = Partida()
@app.route("/controle_peças", methods=['POST'])
def controle_peças():
    configurar_json = request.json
    brancas = int(configurar_json.get("brancas"))
    pretas = int(configurar_json.get("pretas"))
    print(f"Configuração recebida: Brancas -> {brancas}, Pretas -> {pretas}")
    partida.configurar(brancas, pretas)
    return jsonify({
        "controlar_brancas": brancas,
        "controlar_pretas": pretas,
        "status": "Configurar recebida com sucesso."
    })

@app.route('/receber_tabuleiro', methods=['POST'])
def controle_do_jogo():
    configurar_json = request.json
    fen = configurar_json['fen']
    board = chess.Board(fen)
    brancas, pretas = partida.obter_configuracao()
    if board.turn == chess.WHITE:
        if brancas == 1:
            
        
        elif brancas == 2:
            
        
        elif brancas == 3:
            
        
    elif board.turn == chess.BLACK:
        if pretas == 1:
        

        elif pretas == 2:
        

        elif pretas == 3:
    












if __name__ == '__main__':
    app.run(port=5003)
