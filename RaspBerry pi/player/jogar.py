import requests
from realizar_jogada_engine import realizar_jogada_engine
from realizar_jogada_aleatoria import realizar_jogada_aleatoria
from realizar_jogada_usuario import realizar_jogada_usuario
import chess


tabuleiro_url = "http://localhost:5003/receber_jogada"

def jogar_ia(fen, board):
    """Recebe a jogada do oponente, faz sua jogada e envia de volta."""
    board.set_fen(fen)
    print(f"\nJogador recebeu o tabuleiro FEN. Tabuleiro FEN: {fen}")
    print(board)
    jogada = realizar_jogada_engine(board.fen())
    board.push(chess.Move.from_uci(jogada))
   
        
        # Enviar a jogada para o jogador 1
    try:
        response = requests.post(tabuleiro_url, json={'fen': board.fen()}, timeout=5)
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
        response = requests.post(tabuleiro_url, json={'fen': board.fen()}, timeout=5)
        if response.status_code == 200:
            print("\nJogada enviada para o tabuleiro.")
    except requests.RequestException as e:
        print(f"Erro: {e}")
    
    return board  # Retorna o novo estado do tabuleiro e o jogador atualizado

def jogar_aleatorio(fen, board):
    board.set_fen(fen)
    print(f"\nJogador recebeu o tabuleiro FEN. Tabuleiro FEN: {fen}")
    print(board)

    # Supondo que 'realizar_jogada_aleatoria' seja uma função que retorna uma jogada válida
    jogada = realizar_jogada_aleatoria(board)  # Aqui você deve passar o board como argumento

    # Agora, você tem a jogada válida para ser executada
    board.push(jogada)

    try:
        response = requests.post(tabuleiro_url, json={'fen': board.fen()}, timeout=5)
        if response.status_code == 200:
            print(f"\n Jogada enviada para o tabuleiro.")
    except requests.RequestException as e:
        print(f"Erro: {e}")

    return board