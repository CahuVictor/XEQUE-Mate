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
    SerialCommunication(Stream* serialInterface);  // Construtor com a interface serial como parâmetro

    void initialize();                        // Inicializa a fila e o buffer circular
    void setQueue(QueueHandle_t* queue);     // Define a fila da comunicação serial
    void startTask();                        // Inicia as tarefas FreeRTOS para recepção e transmissão
    bool sendMessage(const char* message);    // Envia uma mensagem para a fila de transmissão
    int readData(char* buffer, size_t length); // Lê dados do buffer circular para outro bloco

    //void setWiFiTaskHandle(TaskHandle_t handle);

private:
    Stream* serial;                           // Interface serial para comunicação
    QueueHandle_t* __queue__;                // Fila para dados de saída serial
    SemaphoreHandle_t bufferMutex;            // Mutex para acesso ao buffer circular
    char rxBuffer[BUFFER_SIZE];               // Buffer circular para dados recebidos
    size_t writeIndex;                        // Índice de escrita no buffer
    size_t readIndex;                         // Índice de leitura no buffer

    void serialReceiveTask();                 // Tarefa para monitorar dados recebidos na serial
    void serialTransmitTask();                // Tarefa para monitorar dados de transmissão na fila
    void writeToBuffer(char c);               // Escreve no buffer circular
    int readFromBuffer(char* buffer, size_t length); // Lê do buffer circular
    void processCommand(const char* command); // Nova função para processar comandos

    static void receiveTaskWrapper(void* param) {
        static_cast<SerialCommunication*>(param)->serialReceiveTask();
    }

    static void transmitTaskWrapper(void* param) {
        static_cast<SerialCommunication*>(param)->serialTransmitTask();
    }

    //TaskHandle_t wifiTaskHandle;  // Handle para notificar a WiFiManager
};

#endif
