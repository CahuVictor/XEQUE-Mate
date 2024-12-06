import chess
import chess.engine
from setup import obter_caminho_engine

# Caminho para o executável do Stockfish
STOCKFISH_PATH = obter_caminho_engine()

def avaliar_posicao(fen):
    with chess.engine.SimpleEngine.popen_uci(STOCKFISH_PATH) as engine:
        board = chess.Board(fen)
        result = engine.analyse(board, chess.engine.Limit(time=1))  # Análise por 1 segundo
        return result['score'].relative.score() / 100  # Retorna o score em peões (em unidades de peões)


def calcular_score(fen_before, fen_after):
    # Avalia a posição antes e depois da jogada
    score_before = avaliar_posicao(fen_before)
    score_after = avaliar_posicao(fen_after)
    
    # Calcula a diferença de score
    delta_score = score_after - score_before
    return delta_score

