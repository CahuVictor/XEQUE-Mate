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
#define __RFID_USE_FAKE_FUNCTIONS__

#define PEAO_BRANCO                                     1
#define CAVALO_BRANCO                                   2
#define BISPO_BRANCO                                    3
#define TORRE_BRANCO                                    4
#define RAINHA_BRANCO                                   5
#define REI_BRANCO                                      6
#define PEAO_PRETO                                     -1
#define CAVALO_PRETO                                   -2
#define BISPO_PRETO                                    -3
#define TORRE_PRETO                                    -4
#define RAINHA_PRETO                                   -5
#define REI_PRETO                                      -6

#define QUEUE_MESSAGE_SIZE 64

enum RFIDReadMode {
    SINGLE_READ,
    CONTINUOUS_READ,
    READ_DISABLED  // Renomeado para evitar conflito
};

class RFIDControl {
public:
    RFIDControl();                                              // Construtor
    void initialize();                                          // Inicializa RFID e configura SPI

    void scanAll();                                             // Escaneia todos os RFID uma vez
    void setReadMode(RFIDReadMode mode);                        // Define o modo de leitura
    void setReadInterval(uint32_t interval_ms);                 // Define o intervalo entre leituras contínuas

    void setQueue(QueueHandle_t SendQueue, QueueHandle_t ReceiveQueue);     // Define a fila de envio e recebimento de dados
    void startTask();                                           // Inicia a tarefa FreeRTOS para leitura contínua dos RFID

private:
    GPIOExpander gpioExpander;                                  // Instância do expansor de GPIO
    RFIDReadMode readMode;                                      // Modo atual de leitura
    uint32_t readInterval;                                      // Intervalo entre leituras em modo contínuo

    void readRFID(uint8_t rfidID);                              // Função para ler um RFID específico

    int matrizPos[8][8];
    
    QueueHandle_t SendQueue;                                    // Ponteiro para a fila de envio de dados
    QueueHandle_t ReceiveQueue;                                 // Ponteiro para a fila de recebimento de dados
    SemaphoreHandle_t spiMutex;                                 // Mutex para controle do barramento SPI

    bool sendMessageToRFIDQueue(const char* msg);               // Envia mensagens para a fila do RFID
    void monitorTask();                                     // Função FreeRTOS contínua para controle de RFID
    void processCommand();                                      // Processa comandos recebidos na fila

    static void taskWrapper(void* _this) {
        static_cast<RFIDControl*>(_this)->monitorTask();
    }

    #ifdef __RFID_USE_FAKE_FUNCTIONS__
        String getPiece(int statusPos);
        
        void updateMatrizFake();
    #endif
};

#endif
