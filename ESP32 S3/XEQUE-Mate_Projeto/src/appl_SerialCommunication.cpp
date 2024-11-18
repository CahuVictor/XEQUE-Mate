#include "appl_SerialCommunication.h"

SerialCommunication::SerialCommunication(Stream* serialInterface) 
    : serial(serialInterface), SendQueue(nullptr), ReceiveQueue(nullptr),/* bufferMutex(nullptr),*/ writeIndex(0), readIndex(0) {}

void SerialCommunication::initialize() {
    LOG_INFO(serial, "SerialCommunication: Inicializado com fila e buffer.");
}

/*
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
}*/
/*
int SerialCommunication::readFromBuffer(char* buffer, size_t length) {
    int bytesRead = 0;
    while (bytesRead < length && readIndex != writeIndex) {
        buffer[bytesRead++] = rxBuffer[readIndex];
        readIndex = (readIndex + 1) % BUFFER_SIZE;
    }
    return bytesRead;
}*/

void SerialCommunication::setQueue(QueueHandle_t SendQueue, QueueHandle_t ReceiveQueue) {
    this->SendQueue = SendQueue;
    this->ReceiveQueue = ReceiveQueue;
}

void SerialCommunication::startTask() {
    xTaskCreate(taskWrapper, "Serial Receive Task",              // A name just for humans
                2048,                                                   // The stack size can be checked by calling `uxHighWaterMark = 
                                                                        // uxTaskGetStackHighWaterMark(NULL);`
                this,                                                   // Task parameter which can modify the task behavior. This must 
                                                                        // be passed as pointer to void.
                1,                                                      // Priority
                NULL);                                                  // Task handle is not used here - simply pass NULL
}

void SerialCommunication::monitorTask() {
    static char commandBuffer[64];
    static int commandIndex = 0;
    
    for (;;) {
        if (serial->available() > 0) {
            char receivedChar = serial->read();
            //writeToBuffer(receivedChar);

            // Acumula caracteres no `commandBuffer` até detectar um comando completo (finalizado com '\n')
            if (receivedChar != '\n' && commandIndex < (sizeof(commandBuffer) - 1))
            {
                commandBuffer[commandIndex] = receivedChar;
                commandIndex++;
                commandBuffer[commandIndex] = '\0';
                Serial.println(commandBuffer);
            } else {
                // Finaliza o comando com um terminador nulo e chama `processCommand`
                commandBuffer[commandIndex] = '\0';
                LOG_DEBUG(serial, ("serialCommunication: Comando recebido. Comando: " + String(commandBuffer)).c_str());
                processCommand(commandBuffer);
                commandIndex = 0; // Reseta para o próximo comando
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void SerialCommunication::processCommand(const char* command) {
    // Verifica se "HELP" está presente em qualquer parte da string
    if (strstr(command, "HELP") != nullptr) {
        LOG_INFO(serial, "Comandos disponíveis:\nSSID:<nome>\nPASSWORD:<senha>\nHELP\nGET IP\n");
        LOG_INFO(serial, "    SSID:<nome>");
        LOG_INFO(serial, "    PASSWORD:<senha>");
        LOG_INFO(serial, "    GET IP");
        LOG_INFO(serial, "    GET PORT");
        LOG_INFO(serial, "    GET URL");
        LOG_INFO(serial, "    READ RFID");
        LOG_INFO(serial, "    SEND LCD:<msg>");
        LOG_INFO(serial, "    TEST BUTTON");
    } else if ( strstr(command, "GET IP") != nullptr || 
                strstr(command, "SSID:") != nullptr || 
                strstr(command, "PASSWORD:") != nullptr || 
                strstr(command, "GET PORT") != nullptr  || 
                strstr(command, "GET URL") != nullptr || 
                strstr(command, "READ RFID") != nullptr || 
                strstr(command, "SEND LCD:") != nullptr || 
                strstr(command, "TEST BUTTON") != nullptr) {
        // Comandos para WiFiManager
        if ( this->SendQueue != nullptr) {
            xQueueSend( this->SendQueue , command, portMAX_DELAY);  // Envia o comando para a Queue de envio
            LOG_DEBUG(serial, ("serialCommunication: Comando enviado para a fila SendQueue. Comando: " + String(command)).c_str());
        }
        else 
        {
            LOG_DEBUG(serial, "serialCommunication: SendQueue não inicializada");
        }
    } else {
        // Comando não reconhecido - para futuras implementações, podemos adicionar mais verificações
        LOG_INFO(serial, "serialCommunication: Comando desconhecido. Digite 'HELP' para ver os comandos disponíveis.");
    }
}