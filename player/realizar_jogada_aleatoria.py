import chess
import random

def realizar_jogada_aleatoria(fen):
    """Realiza uma jogada aleatória e a aplica no tabuleiro."""
    board = chess.Board(fen)
    legal_moves = list(board.legal_moves)
    jogada_aleatoria = random.choice(legal_moves)
    return jogada_aleatoria.uci()  
