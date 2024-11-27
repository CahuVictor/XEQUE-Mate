from flask import jsonify, request

def configurar_partida(configurar):
    """Recebe as configurações de quem vai controlar as brancas e as pretas."""
    controlar_brancas = int(configurar.get("brancas"))
    controlar_pretas = int(configurar.get("pretas"))
    
    print(f"Configuração recebida: Brancas -> {controlar_brancas}, Pretas -> {controlar_pretas}")
    
    return controlar_brancas, controlar_pretas  
