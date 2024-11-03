#include "appl_RFIDControl.h"

RFIDControl::RFIDControl() : rfidQueue(nullptr), spiMutex(nullptr), readMode(READ_DISABLED), readInterval(5000) {}

void RFIDControl::initialize() {
    gpioExpander.initialize();
    SPI.begin();
    spiMutex = xSemaphoreCreateMutex();
    if (spiMutex == nullptr) {
        LOG_ERROR(&Serial, "RFIDControl: Falha ao criar mutex SPI.");
    }

    LOG_INFO(&Serial, "RFIDControl: SPI, expansor de GPIO e mutex inicializados.");

    rfidQueue = xQueueCreate(10, sizeof(char) * 32);
    if (rfidQueue == nullptr) {
        LOG_ERROR(&Serial, "RFIDControl: Falha ao criar a fila de controle do RFID.");
    }
}

void RFIDControl::startTask() {
    xTaskCreate(taskWrapper, "RFID Control Task", 4096, this, 1, NULL);
}

void RFIDControl::setReadMode(RFIDReadMode mode) {
    readMode = mode;
    LOG_INFO(&Serial, mode == CONTINUOUS_READ ? "Modo contínuo ativado" :
                   mode == SINGLE_READ ? "Modo de leitura única ativado" :
                                         "Leitura desativada");
}

void RFIDControl::setReadInterval(uint32_t interval_ms) {
    readInterval = interval_ms;
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "Intervalo de leitura configurado para %d ms", interval_ms);
    LOG_INFO(&Serial, buffer);
}

bool RFIDControl::sendMessageToRFIDQueue(const char* msg) {
    if (rfidQueue != nullptr) {
        if (xQueueSend(rfidQueue, msg, portMAX_DELAY) == pdPASS) {
            return true;
        } else {
            LOG_WARNING(&Serial, "RFIDControl: Falha ao enviar mensagem para a fila de RFID.");
        }
    }
    return false;
}

void RFIDControl::scanAll() {
    LOG_INFO(&Serial, "RFIDControl: Iniciando escaneamento de todos os 64 RFID.");
    for (uint8_t i = 0; i < 64; i++) {
        readRFID(i);
    }
}

void RFIDControl::rfidControlTask() {
    char msg[32];
    for (;;) {
        if (readMode == CONTINUOUS_READ) {
            scanAll();
            vTaskDelay(pdMS_TO_TICKS(readInterval));
        } else if (readMode == SINGLE_READ) {
            if (xQueueReceive(rfidQueue, msg, portMAX_DELAY) == pdPASS) {
                LOG_INFO(&Serial, "RFIDControl: Escaneando RFID conforme mensagem recebida.");
                scanAll();
                readMode = READ_DISABLED;  // Após a leitura única, desativa o modo de leitura
            }
        } else {
            vTaskDelay(pdMS_TO_TICKS(100));  // Pequeno delay para evitar uso excessivo da CPU
        }
    }
}

void RFIDControl::readRFID(uint8_t rfidID) {
    gpioExpander.selectChannel(rfidID % 16);  // Seleciona o canal do RFID no multiplexador

    #ifdef USE_FAKE_FUNCTIONS
    uint8_t fakeStatus = random(0, 2);  // 0 = ausente, 1 = presente
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "RFID %d: Status fictício %s", rfidID, fakeStatus ? "presente" : "ausente");
    LOG_INFO(&Serial, buffer);
    #else
    if (xSemaphoreTake(spiMutex, portMAX_DELAY) == pdTRUE) {
        // Código real para comunicação SPI com o RFID
        // SPI.transfer(...); // Exemplo fictício de comando SPI
        xSemaphoreGive(spiMutex);
    } else {
        LOG_ERROR(&Serial, "RFIDControl: Falha ao obter o mutex SPI.");
    }
    #endif
}
