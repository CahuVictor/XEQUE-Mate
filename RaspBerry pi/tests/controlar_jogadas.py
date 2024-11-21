def controlar_jogadas():
    """Solicita ao usuário quem controlará as brancas e quem controlará as pretas."""
    global controlar_brancas, controlar_pretas
    controlar_brancas = input("Quem irá controlar as peças brancas? (1 para IA, 2 para Jogada Aleatória, 3 para Usuário): ")
    controlar_pretas = input("Quem irá controlar as peças pretas? (1 para IA, 2 para Jogada Aleatória, 3 para Usuário): ")

    # Enviar as informações ao player antes de iniciar o jogo Json
    dados_configuracao = {
        "brancas": controlar_brancas,
        "pretas": controlar_pretas
    }
    try:
        response = requests.post("http://localhost:5001/configurar_partida", json=dados_configuracao)
        if response.status_code == 200:
            print("Configuração enviada com sucesso para o Player!")
        else:
            print("Erro ao enviar configuração para o Player.")
    except requests.RequestException as e:
        print(f"Erro de comunicação com o Player: {e}")