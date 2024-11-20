#ifndef __I2CCommunication_h__
#define __I2CCommunication_h__

#include <Arduino.h>
#include <Wire.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "appl_MyLogger.h"

#define QUEUE_MESSAGE_SIZE 64  // Tamanho máximo de uma mensagem na fila
#define I2C_SCL_PIN 22         // Defina o pino SCL
#define I2C_SDA_PIN 21         // Defina o pino SDA
#define I2C_ADDRESS 0x42       // Endereço I2C do ESP32 para comunicação com o Raspberry Pi

class I2CCommunication {
public:
    I2CCommunication(uint8_t address = I2C_ADDRESS);  // Construtor para inicializar com o endereço I2C
    void initialize();                                // Inicializa o barramento I2C
    void setQueue(QueueHandle_t SendQueue, QueueHandle_t ReceiveQueue); // Define filas para troca de dados
    void startTask();                                 // Inicia a tarefa FreeRTOS para monitorar o I2C

private:
    uint8_t i2cAddress;                               // Endereço I2C do ESP32
    QueueHandle_t SendQueue;                          // Fila de envio de dados
    QueueHandle_t ReceiveQueue;                       // Fila de recebimento de dados

    void monitorTask();                               // Monitora dados no I2C
    void processCommand();                            // Processa comandos recebidos pela fila
    bool sendMessageToQueue(const char* message);     // Envia mensagens para a fila
    void handleI2CWrite(const char* data);            // Lida com escrita no I2C
    void handleI2CRead();                             // Lida com leitura do I2C

    static void taskWrapper(void* _this) {
        static_cast<I2CCommunication*>(_this)->monitorTask();
    }
};

#endif
