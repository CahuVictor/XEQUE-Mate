from chess_engine import ChessEngine

def main():
    engine = ChessEngine()
    try:
        engine.start_engine()
        print("Chess e Stockfish inicializados com sucesso")
        print("Tabuleiro inicial:")
        print(engine.board)
        best_move = engine.get_best_move()
        print("Melhor jogada sugerida:", best_move)
    except Exception as e:
        print("Erro:", e)
    finally:
        engine.stop_engine()

if __name__ == "__main__":
    main()
