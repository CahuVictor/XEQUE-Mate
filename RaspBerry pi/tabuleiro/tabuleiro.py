import chess

# Função para mostrar o tabuleiro
def mostrar_tabuleiro(board):
    print("\nEstado atual do tabuleiro:")
    print(board)

# Função para reiniciar o tabuleiro
def reiniciar_tabuleiro():
    global board
    board = chess.Board()
