#ifndef __appl_LedControl_h__
#define __appl_LedControl_h__

#include "Arduino.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "appl_MyLogger.h"

// Define `USE_FAKE_FUNCTIONS` para usar valores fictícios nas funções.
#define __LED_USE_FAKE_FUNCTIONS__

class LedControl {
public:
    LedControl();                                  // Construtor
    void initialize();                             // Inicializa o controle de LEDs

    void setBrightness(uint8_t level);             // Define o brilho dos LEDs
    void updateLEDs(const uint32_t* colorData, size_t length); // Atualiza os LEDs

    void setQueue(QueueHandle_t SendQueue, QueueHandle_t ReceiveQueue);     // Define a fila de envio e recebimento de dados
    void startTask();                    // Inicia a tarefa FreeRTOS para gerenciar WiFi

private:
    QueueHandle_t SendQueue;                    // Ponteiro para a fila de envio de dados
    QueueHandle_t ReceiveQueue;                 // Ponteiro para a fila de recebimento de dados

    bool sendMessageToLedControlQueue(const char* message); // Envia mensagem para a fila do Led Control
    void monitorTask();
    void processCommand();                     // Processa comandos recebidos na fila

    static void taskWrapper(void* param) {
        static_cast<LedControl*>(param)->monitorTask();
    }

    #ifdef __LED_USE_FAKE_FUNCTIONS__
        void updateFakeLEDs();                         // Função fictícia para simular LEDs
    #endif
};

#endif
