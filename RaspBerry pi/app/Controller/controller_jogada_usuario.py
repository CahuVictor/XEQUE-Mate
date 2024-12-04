import chess

def realizar_jogada_usuario(fen):
    """Recebe a FEN, e solicita ao jogador uma jogada válida."""
    board = chess.Board(fen)
    legal_moves = list(board.legal_moves)
    print(legal_moves)
    jogada_usuario = input("Faça sua jogada válida: ")
    return jogada_usuario
