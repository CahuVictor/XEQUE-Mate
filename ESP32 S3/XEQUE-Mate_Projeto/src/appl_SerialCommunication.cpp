#include "appl_SerialCommunication.h"

SerialCommunication::SerialCommunication(Stream* serialInterface) 
    : serial(serialInterface), __queue__(nullptr), bufferMutex(nullptr), writeIndex(0), readIndex(0) {}

void SerialCommunication::initialize() {
    LOG_INFO(serial, "SerialCommunication: Inicializado com fila e buffer.");

    // Inicializa a fila para dados de saída serial
    /*serialQueue = xQueueCreate(10, sizeof(char) * 64);
    if (serialQueue == nullptr) {
        LOG_ERROR(serial, "SerialCommunication: Falha ao criar a fila de transmissão serial.");
    }*/

    // Inicializa o mutex para o buffer
    bufferMutex = xSemaphoreCreateMutex();
    if (bufferMutex == nullptr) {
        LOG_ERROR(serial, "SerialCommunication: Falha ao criar mutex para buffer.");
    }
}

void SerialCommunication::setQueue(QueueHandle_t* queue) {
    __queue__ = queue;
}

void SerialCommunication::startTask() {
    xTaskCreate(receiveTaskWrapper, "Serial Receive Task",              // A name just for humans
                2048,                                                   // The stack size can be checked by calling `uxHighWaterMark = 
                                                                        // uxTaskGetStackHighWaterMark(NULL);`
                this,                                                   // Task parameter which can modify the task behavior. This must 
                                                                        // be passed as pointer to void.
                1,                                                      // Priority
                NULL);                                                  // Task handle is not used here - simply pass NULL
    xTaskCreate(transmitTaskWrapper, "Serial Transmit Task", 
                2048,                                                   // The stack size can be checked by calling `uxHighWaterMark = 
                                                                        // uxTaskGetStackHighWaterMark(NULL);`
                this,                                           // Task parameter which can modify the task behavior. This must 
                                                                        // be passed as pointer to void.
                1,                                                      // Priority
                NULL);                                                  // Task handle is not used here - simply pass NULL
}

bool SerialCommunication::sendMessage(const char* message) {
    if (__queue__ != nullptr && *__queue__ != nullptr) {
        if (xQueueSend(*__queue__, message, portMAX_DELAY) == pdPASS)
        {
            delay(1000);
            char checkMessage[64];  // Buffer para armazenar a mensagem da fila
            xQueueReceive(*__queue__, &checkMessage, pdMS_TO_TICKS(100));
            Serial.printf("SerialCommunication: Mensagem salva na fila: %s\n", String(checkMessage));
            
            return true;
        } else {
            LOG_WARNING(serial, "SerialCommunication: Falha ao enviar mensagem para a fila serial.");
        }
    }
    return false;
}

int SerialCommunication::readData(char* buffer, size_t length) {
    int bytesRead = 0;
    if (xSemaphoreTake(bufferMutex, portMAX_DELAY) == pdTRUE) {
        bytesRead = readFromBuffer(buffer, length);
        xSemaphoreGive(bufferMutex);
    }
    return bytesRead;
}

void SerialCommunication::serialReceiveTask() {
    char commandBuffer[64];
    int commandIndex = 0;
    
    for (;;) {
        if (serial->available() > 0) {
            char receivedChar = serial->read();
            if (xSemaphoreTake(bufferMutex, portMAX_DELAY) == pdTRUE) {
                writeToBuffer(receivedChar);
                xSemaphoreGive(bufferMutex);
            }

        // Acumula caracteres no `commandBuffer` até detectar um comando completo (finalizado com '\n')
            if (receivedChar != '\n' && commandIndex < (sizeof(commandBuffer) - 1)) {
                commandBuffer[commandIndex++] = receivedChar;
            } else {
                // Finaliza o comando com um terminador nulo e chama `processCommand`
                commandBuffer[commandIndex] = '\0';
                processCommand(commandBuffer);
                commandIndex = 0; // Reseta para o próximo comando
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void SerialCommunication::serialTransmitTask() {
    char message[64];
    for (;;) {
        if (xQueueReceive(*__queue__, &message, portMAX_DELAY) == pdPASS) {
            LOG_INFO(serial, message);
        }
    }
}

void SerialCommunication::writeToBuffer(char c) {
    rxBuffer[writeIndex] = c;
    writeIndex = (writeIndex + 1) % BUFFER_SIZE;

    // Log de depuração para monitorar a escrita no buffer
    char logBuffer[64];
    snprintf(logBuffer, sizeof(logBuffer), "serialCommunication: Dado recebido '%c' adicionado ao buffer. writeIndex=%d", c, writeIndex);
    LOG_DEBUG(serial, logBuffer);

    // Se o buffer estiver cheio, sobrescreve o dado mais antigo
    if (writeIndex == readIndex) {
        readIndex = (readIndex + 1) % BUFFER_SIZE;
        LOG_WARNING(serial, "serialCommunication: Buffer circular cheio. Sobrescrevendo dados antigos.");
    }
}

int SerialCommunication::readFromBuffer(char* buffer, size_t length) {
    int bytesRead = 0;
    while (bytesRead < length && readIndex != writeIndex) {
        buffer[bytesRead++] = rxBuffer[readIndex];
        readIndex = (readIndex + 1) % BUFFER_SIZE;
    }
    return bytesRead;
}

void SerialCommunication::processCommand(const char* command) {
    // Verifica se "HELP" está presente em qualquer parte da string
    if (strstr(command, "HELP") != nullptr) {
        sendMessage("Comandos disponíveis:\nSSID:<nome>\nPASSWORD:<senha>\nHELP\nGET IP\n");
    } else if (strstr(command, "GET IP") != nullptr || strstr(command, "SSID:") != nullptr || strstr(command, "PASSWORD:") != nullptr) {
        // Comandos para WiFiManager
        if ( __queue__ != nullptr) {
            xQueueSend( *__queue__ , command, portMAX_DELAY);  // Envia o comando para WiFiManager
            LOG_DEBUG(serial, ("serialCommunication: Comando enviado para a fila __queue__. Comando: " + String(command)).c_str());

            // Envia notificação para a WiFiManager processar o comando
            /*if (wifiTaskHandle != nullptr) {
                xTaskNotify(wifiTaskHandle, 0, eNoAction);
            }*/
        }
    } else {
        // Comando não reconhecido - para futuras implementações, podemos adicionar mais verificações
        sendMessage("serialCommunication: Comando desconhecido. Digite 'HELP' para ver os comandos disponíveis.");
    }
}

/*void SerialCommunication::setWiFiTaskHandle(TaskHandle_t handle) {
    wifiTaskHandle = handle;  // Handle para notificar a WiFiManager
}*/