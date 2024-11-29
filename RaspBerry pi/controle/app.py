from flask import Flask, request, jsonify
import chess


app = Flask(__name__)

player_url_jogada_usuario = "http://localhost:5001/jogadaUsuario"
player_url_jogada_ia = "http://localhost:5001/jogadaIa"
player_url_jogada_aleatoria = "http://localhost:5001/jogadaAleatoria"
tabuleiro_url = "http://localhost:5000/jogar"



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
            try:
                response = request.post(player_url_jogada_ia, json={'fen': board.fen()})
                if response.status_code == 200:
                  print("\nTabuleiro enviado para o player")
            except request.RequestException as e:
                print(f"Erro: {e}")
        elif brancas == 2:
            try:
                response = request.post(player_url_jogada_aleatoria, json={'fen': board.fen()})
                if response.status_code == 200:
                   print("\nTabuleiro enviado para o player")
            except request.RequestException as e:
                print(f"Erro: {e}")
        elif brancas == 3:
            try:
                response = request.post(player_url_jogada_usuario, json={'fen': board.fen()})
                if response.status_code == 200:
                    print("\nTabuleiro enviado para o player")
            except request.RequestException as e:
                print(f"Erro: {e}")
    elif board.turn == chess.BLACK:
        if pretas == 1:
            try:
                response = request.post(player_url_jogada_ia, json={'fen': board.fen()})
                if response.status_code == 200:
                    print("\nTabuleiro enviado para o player")
            except request.RequestException as e:
                print(f"Erro: {e}")
        elif pretas == 2:
            try:
                response = request.post(player_url_jogada_aleatoria, json={'fen': board.fen()})
                if response.status_code == 200:
                    print("\nTabuleiro enviado para o player")
            except request.RequestException as e:
                print(f"Erro: {e}")
        elif pretas == 3:
            try:
                response = request.post(player_url_jogada_usuario, json={'fen': board.fen()})
                if response.status_code == 200:
                    print("\nTabuleiro enviado para o player")
            except request.RequestException as e:
                print(f"Erro: {e}")



@app.route('/receber_jogada', methods=['POST'])
def receber_jogada():
    configurar_json = request.json
    fen = configurar_json['fen']
    board = chess.Board(fen)
    print("Tabuleiro recebido com jogada do player \n" ,board)
    try:
        response = request.post(tabuleiro_url, json={'fen': board.fen()})
        if response.status_code == 200:
            print("\n Jogada enviada para o tableiro")
    except request.RequestException as e:
        print(f"Erro:{e}")














if __name__ == '__main__':
    app.run(port=5003)
