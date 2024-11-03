# XEQUE-Mate
 Repositório do projeto XEQUE-Mate executado na escola ETE do Porto Digital

Este projeto tem como objetivo desenvolver um jogo de xadrez inteligente e interativo para ensinar pessoas a jogar xadrez. Utilizando impressão 3D e cortadora a laser para fabricação das peças e tabuleiro, adicionando a Sistemas Embarcados com ESP32 e Raspberry Pi 3 para monitorar/controlar LEDs, Display e outros sensores. Com capacidade de identificar a jogada realizada e fornecer feedback visual e textual sobre os movimentos realizados, ajudando os jogadores a compreender estratégias e técnicas do jogo. O projeto terá uma IA integrada na Raspberry Pi 3 para realizar avaliações, sugerir jogadas, e todas as etapas do projeto relacionadas a tomada de decisão, deixando a ESP32 incubida de executar as tarefas solicitadas pela IA, a IA terá como missão promover uma experiência educacional dinâmica e envolvente.

# Projeto ESP32 com FreeRTOS e Comunicação via Serial, WiFi e Web Server

Este projeto utiliza um ESP32 para controle de LEDs, RFID, Display LCD, Web Server e comunicação Serial. A conexão WiFi é configurável e possui suporte para modo Access Point com captive portal para configurações.

## Funcionalidades

- **Conexão WiFi**: Conecta-se automaticamente a uma rede WiFi salva ou cria um Access Point para configuração.
- **Servidor Web**: Servidor HTTP configurável na porta 8080.
- **Comunicação Serial**: Recebe comandos pela Serial, incluindo configuração de SSID e senha do WiFi.
- **Controle de Periféricos**: Controla LEDs, RFID e Display LCD com tarefas assíncronas em FreeRTOS.

## Comandos Serial Disponíveis

- `SSID:<nome>`: Define o SSID da rede WiFi.
- `PASSWORD:<senha>`: Define a senha da rede WiFi.
- `HELP`: Mostra os comandos disponíveis.

## Estrutura de Arquivos

- `appl_LedControl.h/.cpp`: Controle dos LEDs.
- `appl_RFIDControl.h/.cpp`: Controle de RFID com multiplexador.
- `appl_LCDControl.h/.cpp`: Controle de Display LCD.
- `appl_SerialCommunication.h/.cpp`: Comunicação Serial e processamento de comandos.
- `appl_WebServerControl.h/.cpp`: Controle do servidor web para monitoramento e interface.
- `WiFiManager.h/.cpp`: Gerenciamento da conexão WiFi com suporte para captive portal.
- `MyLogger.h/.cpp`: Biblioteca de logs com suporte a FreeRTOS.

## Modificações na Biblioteca AsyncTCP

Para compatibilidade com FreeRTOS, foi necessário editar o arquivo `AsyncTCP.cpp`:

1. Abra o arquivo `.pio/libdeps/esp32-s3-devkitc-1/AsyncTCP/src/AsyncTCP.cpp`.
2. Substitua todas as ocorrências de `xQueueHandle` por `QueueHandle_t`.

## Dependências

Adicione as dependências ao arquivo `platformio.ini`:

```ini
lib_deps =
    me-no-dev/ESP Async WebServer@^1.2.4
    me-no-dev/AsyncTCP@^1.1.1
