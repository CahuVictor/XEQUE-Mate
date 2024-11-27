import requests
from realizar_jogada_engine import realizar_jogada_engine
from realizar_jogada_aleatoria import realizar_jogada_aleatoria
from realizar_jogada_usuario import realizar_jogada_usuario
import chess

tabuleiro_url_ia = "http://localhost:5000/jogada_ia"
tabuleir_url_usuario = "http://localhost:5000/jogada_usuario"
tabuleiro_url_aleatorio = "http://localhost:5000/jogada_aleatoria"

def jogar_ia(fen, board):
    """Recebe a jogada do oponente, faz sua jogada e envia de volta."""
    board.set_fen(fen)
    print(f"\nJogador recebeu o tabuleiro FEN. Tabuleiro FEN: {fen}")
    print(board)
    jogada = realizar_jogada_engine(board.fen())
    board.push(chess.Move.from_uci(jogada))
   
        
        # Enviar a jogada para o jogador 1
    try:
        response = requests.post(tabuleiro_url_usuario, json={'fen': board.fen()}, timeout=5)
        if response.status_code == 200:
            print("\nJogada enviada para o tabuleiro.")
    except requests.RequestException as e:
        print(f"Erro: {e}")
    return board  # Retorna o novo estado do tabuleiro e o jogador atualizado

def jogar_usuario(fen, board):
    """Recebe a jogada do oponente, faz sua jogada e envia de volta."""
    board.set_fen(fen)
    print(f"\nJogador recebeu o tabuleiro FEN. Tabuleiro FEN: {fen}")
    print(board)
    jogada = realizar_jogada_usuario(board.fen())
    board.push(chess.Move.from_uci(jogada))
   
        
        # Enviar a jogada para o jogador 1
    try:
        response = requests.post(tabuleiro_url_ia, json={'fen': board.fen()}, timeout=5)
        if response.status_code == 200:
            print("\nJogada enviada para o tabuleiro.")
    except requests.RequestException as e:
        print(f"Erro: {e}")
    
    return board  # Retorna o novo estado do tabuleiro e o jogador atualizado

def jogar_aleatorio(fen, board):
    board.set_fen(fen)
    print(f"\nJogador recebeu o tabuleiro FEN. Tabuleiro FEN: {fen}")
    print(board)
    jogada = realizar_jogada_aleatoria
    board.push(chess.Move.from_uci(jogada))
    try:
        response = requests.post(tabuleiro_url_aleatorio, json={'fen': board.fen()}, timeout=5)
        if response.status_code == 200:
            print(f"\n Jogada enviada para o tabuleiro.")
    except requests.RequestException as e:
        print(f"Erro: {e}")