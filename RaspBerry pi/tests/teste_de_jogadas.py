import chess
import chess.engine
from chess.engine import SimpleEngine
import os
import shutil

def evaluate_move(score):
    #Gera um feedback textual baseado na avaliação da jogada.
    if score > 0:
        return "A jogada foi boa! Você melhorou a posição."
    elif score < 0:
        return "A jogada não foi boa. Você pode ter perdido material ou exposto seu rei."
    else:
        return "A jogada é neutra. A posição não mudou muito."

def main():
    try:
        board = chess.Board()

        # Diretório do script
        script_dir = os.path.dirname(os.path.abspath(__file__))
        stockfish_dir = os.path.join(script_dir, 'Stockfish')
        engine_name = 'stockfish-windows-x86-64-sse41-popcnt.exe'
        local_engine_path = os.path.join(stockfish_dir, engine_name)
    except Exception as e:
        print("Erro:", e)

    if os.path.exists(local_engine_path):
        engine_path = local_engine_path
    else:
        engine_path = shutil.which('stockfish')
        if engine_path is None:
            raise FileNotFoundError("Executável do Stockfish não encontrado.")

    with SimpleEngine.popen_uci(engine_path) as engine:
        while not board.is_game_over():
            print(board)
            print("\nDigite seu movimento (ex: e2e4):")
            move = input()

            if chess.Move.from_uci(move) in board.legal_moves:
                board.push(chess.Move.from_uci(move))

                # Avaliação da posição após o movimento do jogador
                evaluation = engine.analyse(board, chess.engine.Limit(time=1))  # Corrigido aqui
                score = evaluation['score'].relative.score()

                if score is not None:
                    feedback = evaluate_move(score)
                    print(feedback)

                if not board.is_game_over():
                    result = engine.play(board, chess.engine.Limit(time=2))
                    board.push(result.move)
                    print(f"Stockfish move: {result.move}")

                    # Avaliação após o movimento do Stockfish
                    evaluation = engine.analyse(board, chess.engine.Limit(time=1))  # Corrigido aqui
                    score = evaluation['score'].relative.score()
                    print(f"A avaliação da jogada do Stockfish: {score}")
                    

        print("Jogo terminado!")
        print("Resultado:", board.result())

if __name__ == "__main__":
    main()
