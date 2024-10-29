import chess
import chess.engine
from chess.engine import SimpleEngine
stockfish_path = '/home/codespace/.python/current/lib/python3.12/site-packages/stockfish'

try:
    board = chess.Board()
    engine = SimpleEngine.popen_uci(stockfish_path)
    print("Chess e Stockfish inicializados com sucesso")
    print("Tabuleiro inicial:")
    print(board)
    result = engine.play(board,chess.engine.Limit(time=0.1))
    print("Melhor jogada sugerida:", result.move)
    engine.quit()
except Exception as e:
    print("Erro:", e)
