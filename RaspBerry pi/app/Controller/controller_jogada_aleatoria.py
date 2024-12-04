import chess
import random

def realizar_jogada_aleatoria(board):
    # Gera uma lista de movimentos válidos
    movimentos_validos = list(board.legal_moves)

    # Escolhe um movimento aleatório
    jogada = random.choice(movimentos_validos)

    return jogada
