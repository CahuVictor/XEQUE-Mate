#ifndef __appl_SerialCommunication_h__
#define __appl_SerialCommunication_h__

#include "Arduino.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include "appl_MyLogger.h"

#define BUFFER_SIZE 256  // Tamanho do buffer circular

class SerialCommunication {
public:
    SerialCommunication(Stream* serialInterface);           // Construtor com a interface serial como parâmetro

    void initialize();                                      // Inicializa a fila e o buffer circular
    void setQueue(QueueHandle_t SendQueue, QueueHandle_t ReceiveQueue);     // Define a fila de envio e recebimento de dados
    void startTask();                                       // Inicia as tarefas FreeRTOS para recepção e transmissão
    int readData(char* buffer, size_t length);              // Lê dados do buffer circular para outro bloco

    //void setWiFiTaskHandle(TaskHandle_t handle);

private:
    Stream* serial;                                         // Interface serial para comunicação
    QueueHandle_t SendQueue;                                // Ponteiro para a fila de envio de dados
    QueueHandle_t ReceiveQueue;                             // Ponteiro para a fila de recebimento de dados
    char rxBuffer[BUFFER_SIZE];                             // Buffer circular para dados recebidos
    size_t writeIndex;                                      // Índice de escrita no buffer
    size_t readIndex;                                       // Índice de leitura no buffer

    void serialReceiveTask();                               // Tarefa para monitorar dados recebidos na serial
    void writeToBuffer(char c);                             // Escreve no buffer circular
    int readFromBuffer(char* buffer, size_t length);        // Lê do buffer circular
    void processCommand(const char* command);               // Nova função para processar comandos

    static void receiveTaskWrapper(void* param) {
        static_cast<SerialCommunication*>(param)->serialReceiveTask();
    }
};

#endif
