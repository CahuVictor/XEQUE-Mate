#ifndef __appl_ButtonControl_h__
#define __appl_ButtonControl_h__

#include "Arduino.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "appl_MyLogger.h"

#define QUEUE_MESSAGE_SIZE 64  // Tamanho máximo de uma mensagem na fila
#define BUTTON_COUNT 4         // Número de botões

class ButtonControl {
public:
    ButtonControl(uint8_t buttonPins[BUTTON_COUNT]); // Construtor para inicializar com os pinos dos botões
    void initialize();                               // Inicializa os pinos dos botões e configura as interrupções
    void setQueue(QueueHandle_t SendQueue, QueueHandle_t ReceiveQueue); // Define as filas de envio e recebimento
    void startTask();                                // Inicia a tarefa FreeRTOS para monitorar os botões

private:
    uint8_t buttonPins[BUTTON_COUNT];                // Array de pinos dos botões
    volatile bool buttonPressed[BUTTON_COUNT];       // Array de estados dos botões (pressionados ou não)

    QueueHandle_t SendQueue;                         // Fila para enviar mensagens
    QueueHandle_t ReceiveQueue;                      // Fila para receber mensagens

    void handleButtonPress(int buttonIndex);         // Função chamada na interrupção para detectar o pressionamento
    static void IRAM_ATTR isrButtonPress(void* arg); // Interrupção de rotina de serviço para os botões

    bool sendMessageToQueue(const char* message);    // Envia mensagem para a fila
    void monitorTask();                              // Monitora o estado dos botões e processa comandos
    void processCommand();                           // Processa comandos da fila de recebimento

    static void taskWrapper(void* _this) {
        static_cast<ButtonControl*>(_this)->monitorTask();
    }
};

#endif
