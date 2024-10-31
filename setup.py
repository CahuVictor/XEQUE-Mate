import chess
import chess.engine
from chess.engine import SimpleEngine
import os
import shutil

try:
    board = chess.Board()

    # Diretório do script
    script_dir = os.path.dirname(os.path.abspath(__file__))
    
    # Tenta encontrar o executável do Stockfish no mesmo diretório do script
    stockfish_dir = os.path.join(script_dir, 'Stockfish')
    engine_name = 'stockfish-windows-x86-64-sse41-popcnt.exe'  # Nome exato do executável
    local_engine_path = os.path.join(stockfish_dir, engine_name)

    if os.path.exists(local_engine_path):
        engine_path = local_engine_path
    else:
        # Tenta encontrar o Stockfish no PATH do sistema
        engine_path = shutil.which('stockfish')
        if engine_path is None:
            raise FileNotFoundError("Executável do Stockfish não encontrado.")

    engine = SimpleEngine.popen_uci(engine_path)
    print("Chess e Stockfish inicializados com sucesso")
    print("Tabuleiro inicial:")
    print(board)
    result = engine.play(board,chess.engine.Limit(time=0.1))
    print("Melhor jogada sugerida:", result.move)
    engine.quit()
except Exception as e:
    print("Erro:", e)
