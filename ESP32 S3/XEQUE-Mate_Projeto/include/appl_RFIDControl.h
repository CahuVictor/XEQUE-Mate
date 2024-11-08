#ifndef __appl_RFIDControl_h__
#define __appl_RFIDControl_h__

#include "Arduino.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <SPI.h>
#include "appl_MyLogger.h"
#include "appl_GPIOExpander.h"

// Define `USE_FAKE_FUNCTIONS` para usar valores fictícios nas funções
#define USE_FAKE_FUNCTIONS

enum RFIDReadMode {
    SINGLE_READ,
    CONTINUOUS_READ,
    READ_DISABLED  // Renomeado para evitar conflito
};

class RFIDControl {
public:
    RFIDControl();                              // Construtor
    void initialize();                          // Inicializa RFID e configura SPI
    void startTask();                           // Inicia a tarefa FreeRTOS para leitura contínua dos RFID
    void scanAll();                             // Escaneia todos os RFID uma vez
    void setReadMode(RFIDReadMode mode);        // Define o modo de leitura
    void setReadInterval(uint32_t interval_ms); // Define o intervalo entre leituras contínuas

    bool sendMessageToRFIDQueue(const char* msg); // Envia mensagens para a fila do RFID

private:
    GPIOExpander gpioExpander;            // Instância do expansor de GPIO
    QueueHandle_t rfidQueue;              // Fila para mensagens de controle do RFID
    SemaphoreHandle_t spiMutex;           // Mutex para controle do barramento SPI
    RFIDReadMode readMode;                // Modo atual de leitura
    uint32_t readInterval;                // Intervalo entre leituras em modo contínuo
    void rfidControlTask();               // Função FreeRTOS contínua para controle de RFID
    void readRFID(uint8_t rfidID);        // Função para ler um RFID específico

    static void taskWrapper(void* _this) {
        static_cast<RFIDControl*>(_this)->rfidControlTask();
    }
};

#endif
