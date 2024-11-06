import chess
import chess.engine
import os

class ChessEngine:
    def __init__(self):
        self.board = chess.Board()
        self.engine = None
        self.engine_path = self._find_engine()

    def _find_engine(self):
        #engine_name = 'stockfish.exe'  # Nome exato do executável
        engine_name = 'stockfish-windows-x86-64-sse41-popcnt.exe'  # Nome exato do executável

        # Diretório atual (onde este arquivo está localizado)
        current_dir = os.path.dirname(os.path.abspath(__file__))
        # Caminho para o executável do Stockfish
        engine_path = os.path.join(current_dir, '..', 'stockfish', engine_name)

        if not os.path.exists(engine_path):
            print(f"Executável do Stockfish não encontrado em {engine_path}")
        
            import glob
            # Diretório onde o Stockfish está localizado
            stockfish_dir = os.path.join(current_dir, '..', 'Stockfish')
            # Padrão para arquivos executáveis do Stockfish
            pattern = os.path.join(stockfish_dir, 'stockfish*')
            # Procura por arquivos que correspondem ao padrão
            executables = glob.glob(pattern)
            for exec_path in executables:
                if os.path.isfile(exec_path) and exec_path.endswith('.exe'):
                    print(f"Encontrado executável do Stockfish: {exec_path}")
                    return exec_path
            raise FileNotFoundError(f"Executável do Stockfish não encontrado em {stockfish_dir}")
        else:
            return engine_path

    def start_engine(self):
        self.engine = chess.engine.SimpleEngine.popen_uci(self.engine_path)

    def get_best_move(self, time_limit=0.1):
        result = self.engine.play(self.board, chess.engine.Limit(time=time_limit))
        return result.move

    def make_move(self, move):
        self.board.push(move)

    def stop_engine(self):
        if self.engine:
            self.engine.quit()
