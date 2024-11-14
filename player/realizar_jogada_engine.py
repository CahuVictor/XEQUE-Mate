import chess.engine
from obter_caminho_engine import obter_caminho_engine

def realizar_jogada_engine(fen):
    """Recebe um FEN do tabuleiro, calcula a melhor jogada e retorna no formato UCI."""
    engine_path = obter_caminho_engine()
    board = chess.Board(fen)
    with chess.engine.SimpleEngine.popen_uci(engine_path) as engine:
        result = engine.play(board, chess.engine.Limit(time=2))
        return result.move.uci()  
