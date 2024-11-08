#ifndef __appl_LedControl_h__
#define __appl_LedControl_h__

#include "Arduino.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "appl_MyLogger.h"

// Define `USE_FAKE_FUNCTIONS` para usar valores fictícios nas funções.
#define USE_FAKE_FUNCTIONS

class LedControl {
public:
    LedControl();                                  // Construtor
    void initialize();                             // Inicializa o controle de LEDs
    void setBrightness(uint8_t level);             // Define o brilho dos LEDs
    void updateLEDs(const uint32_t* colorData, size_t length); // Atualiza os LEDs
    void startTask();                              // Inicia a tarefa FreeRTOS de monitoramento
    void sendMessage(const char* msg);             // Envia mensagem para a fila de controle de LEDs

private:
    QueueHandle_t ledQueue;                        // Fila para mensagens de controle de LEDs
    void ledControlTask();                         // Função FreeRTOS contínua para controle de LEDs
    void updateFakeLEDs();                         // Função fictícia para simular LEDs

    static void taskWrapper(void* _this) {
        static_cast<LedControl*>(_this)->ledControlTask();
    }
};

#endif
