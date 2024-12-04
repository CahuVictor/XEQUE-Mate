import jogo
import requests

def comando_iniciar():
    """Inicia o jogo se ele não estiver em andamento."""
    if not jogo.game_running:
        print("Iniciando o jogo...")
        jogo.iniciar_jogo()
    else:
        print("O jogo já está em andamento.")

def comando_pause():
    """Pausa o jogo se ele estiver em andamento."""
    if jogo.game_running and not jogo.game_paused:
        jogo.game_paused = True
        print("Jogo pausado.")
    else:
        print("O jogo não está em andamento ou já está pausado.")

def comando_resume():
    """Retoma o jogo se ele estiver pausado."""
    if jogo.game_paused:
        jogo.game_paused = False
        print("Jogo retomado.")
    else:
        print("O jogo não está pausado.")

def comando_stop():
    """Finaliza o jogo se ele estiver em andamento."""
    if jogo.game_running:
        jogo.game_running = False
        print("Jogo finalizado.")
    else:
        print("O jogo não está em andamento.")

def comando_lances():
    """Exibe os lances possíveis se o jogo estiver em andamento."""
    if jogo.game_running:
        jogo.exibir_lances()
    else:
        print("O jogo não está em andamento.")

def comando_desistir():
    """Desiste do jogo e declara um vencedor."""
    if jogo.game_running:
        jogo.desistir()
    else:
        print("O jogo não está em andamento.")

def comando_reiniciar():
    """Reinicia o jogo se ele não estiver em andamento."""
    if not jogo.game_running:
        response = requests.post('http://localhost:5003/jogoFinalizado')
        print(response.json())
        jogo.reiniciar_jogo()
    else:
        print("O jogo ainda está em andamento. Não é possível reiniciar.")

def controlar_comandos():
    """Controla os comandos inseridos pelo usuário para gerenciar o jogo."""
    while True:
        comando = input("\nDigite um comando (start, pause, resume, stop, lances, desistir, reiniciar): ").strip().lower()

        if comando == 'start':
            comando_iniciar()
        elif comando == 'pause':
            comando_pause()
        elif comando == 'resume':
            comando_resume()
        elif comando == 'stop':
            comando_stop()
        elif comando == 'lances':
            comando_lances()
        elif comando == 'desistir':
            comando_desistir()
        elif comando == 'reiniciar':
            comando_reiniciar()
        else:
            print("Comando inválido ou jogo já no estado desejado.")
