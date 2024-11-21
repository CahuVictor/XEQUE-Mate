import requests
from realizar_jogada_engine import realizar_jogada_engine
from realizar_jogada_aleatoria import realizar_jogada_aleatoria
from realizar_jogada_usuario import realizar_jogada_usuario
import chess

tabuleiro_url = "http://localhost:5000/jogar"  # Endereço do Jogador 1

def jogar(fen, board, jogador, controlar_brancas, controlar_pretas):
    """Recebe a jogada do oponente, faz sua jogada e envia de volta."""
    board.set_fen(fen)
    print(f"\nJogador recebeu o tabuleiro FEN. Tabuleiro FEN: {fen}")
    print(board)
    
    if not board.is_game_over():
        print('jogador =', jogador)
        if jogador == 1:
            print(controlar_brancas)
            if controlar_brancas == 1:
                jogada = realizar_jogada_engine(board.fen())
            elif controlar_brancas == 2:
                jogada = realizar_jogada_aleatoria(board.fen())
            elif controlar_brancas == 3: 
                jogada = realizar_jogada_usuario(board.fen())
            board.push(chess.Move.from_uci(jogada))
            jogador = 2
        elif jogador == 2:
            if controlar_pretas == 1:
                jogada = realizar_jogada_engine(board.fen())
            elif controlar_pretas == 2:
                jogada = realizar_jogada_aleatoria(board.fen())
            elif controlar_pretas == 3: 
                jogada = realizar_jogada_usuario(board.fen())
            board.push(chess.Move.from_uci(jogada))
            jogador = 1
        
        # Enviar a jogada para o jogador 1
        try:
            response = requests.post(tabuleiro_url, json={'fen': board.fen()}, timeout=5)
            if response.status_code == 200:
                print("\nJogada enviada para o tabuleiro.")
        except requests.RequestException as e:
            print(f"Erro: {e}")
    else:
        print("O jogo acabou.")
    
    return board, jogador  # Retorna o novo estado do tabuleiro e o jogador atualizado
