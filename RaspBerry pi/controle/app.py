from flask import Flask, request, jsonify
import logging
import chess
import requests
import json  # Para manipulação de JSON
import os

app = Flask(__name__)

player_url_jogada_usuario = "http://localhost:5001/jogarUsuario"
player_url_jogada_ia = "http://localhost:5001/jogarIa"
player_url_jogada_aleatoria = "http://localhost:5001/jogarAleatoria"
tabuleiro_url = "http://localhost:5000/jogar"

# Nome do arquivo de log
LOG_FILE = 'json_requests.log'

# Configuração do logger para salvar as requisições em um arquivo de log no formato JSON
logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)

# Função personalizada para salvar o log em formato JSON
def json_log_handler(log_data):
    with open(LOG_FILE, 'a') as log_file:
        log_file.write(json.dumps(log_data) + "\n")

# Middleware para registrar todas as requisições POST que contenham JSON
@app.before_request
def log_request_info():
    if request.method == 'POST' and request.is_json:
        # Captura os dados da requisição em formato JSON
        json_data = request.get_json()
        
        # Prepara os dados para salvar no formato JSON
        log_data = {
            "method": request.method,
            "url": request.url,
            "json_data": json_data
        }
        
        # Registra os dados JSON no arquivo de log
        json_log_handler(log_data)

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
        "status": "Configuração recebida com sucesso."
    })

@app.route('/receber_tabuleiro', methods=['POST'])
def controle_do_jogo():
    configurar_json = request.json
    fen = configurar_json['fen']
    board = chess.Board(fen)
    brancas, pretas = partida.obter_configuracao()
    print(board)
    
    jogadas = {
        1: player_url_jogada_ia,
        2: player_url_jogada_aleatoria,
        3: player_url_jogada_usuario
    }
    quem_joga = brancas if board.turn == chess.WHITE else pretas
    url_jogada = jogadas.get(quem_joga)
    if url_jogada:
        try:
            response = requests.post(url_jogada, json={'fen': board.fen()})
            if response.status_code == 200:
                print("\nTabuleiro enviado para o player")
        except requests.RequestException as e:
            print(f"Erro: {e}")
    return jsonify({"status": "Tabuleiro recebido e processado."})

@app.route('/receber_jogada', methods=['POST'])
def receber_jogada():
    configurar_json = request.json
    fen = configurar_json['fen']
    board = chess.Board(fen)
    print("Tabuleiro recebido com jogada do player \n", board)
    try:
        response = requests.post(tabuleiro_url, json={'fen': board.fen()})
        if response.status_code == 200:
            print("\nJogada enviada para o tabuleiro")
    except requests.RequestException as e:
        print(f"Erro: {e}")
    return jsonify({"status": "Jogada recebida e processada."})

@app.route('/jogoFinalizado', methods=['POST'])
def jogo_finalizado():
    if os.path.exists(LOG_FILE):
        os.remove(LOG_FILE)
        print(f"Arquivo de log {LOG_FILE} apagado.")
    
    # Finaliza o jogo ou zera o estado conforme necessário
    partida.controlar_brancas = None
    partida.controlar_pretas = None
    return jsonify({"status": "Jogo finalizado e arquivo de log apagado."})


if __name__ == '__main__':
    app.run(port=5003)
