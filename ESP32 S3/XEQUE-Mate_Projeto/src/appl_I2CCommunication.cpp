#include "appl_I2CCommunication.h"

const char text_i2c[] = "I2CCommunication::";

I2CCommunication::I2CCommunication(uint8_t address)
    : i2cAddress(address), SendQueue(nullptr), ReceiveQueue(nullptr) {}

void I2CCommunication::initialize() {
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    LOG_INFO(&Serial, (String(text_i2c) + "I2C inicializado.").c_str());
}

void I2CCommunication::setQueue(QueueHandle_t SendQueue, QueueHandle_t ReceiveQueue) {
    this->SendQueue = SendQueue;
    this->ReceiveQueue = ReceiveQueue;
}

bool I2CCommunication::sendMessageToQueue(const char* message) {
    if (this->SendQueue != nullptr) {
        if (xQueueSend(this->SendQueue, message, portMAX_DELAY) == pdPASS) {
            return true;
        } else {
            LOG_WARNING(&Serial, (String(text_i2c) + "Falha ao enviar mensagem para a fila.").c_str());
        }
    }
    return false;
}

void I2CCommunication::startTask() {
    xTaskCreate(this->taskWrapper, "I2CCommunication Task", 4096, this, 1, NULL);
}

void I2CCommunication::monitorTask() {
    for (;;) {
        this->processCommand();

        vTaskDelay(pdMS_TO_TICKS(100));  // Delay para liberar a CPU
    }
}

void I2CCommunication::processCommand() {
    if (this->ReceiveQueue != nullptr) {
        char command[QUEUE_MESSAGE_SIZE];
        if (xQueueReceive(this->ReceiveQueue, &command, pdMS_TO_TICKS(100)) == pdPASS) {
            LOG_INFO(&Serial, (String(text_i2c) + String("Comando recebido: ") + String(command)).c_str());
            if (strncmp(command, "WRITE:", 6) == 0) {
                this->handleI2CWrite(command + 6);
            } else if (strcmp(command, "READ") == 0) {
                this->handleI2CRead();
            }
        }
    }
}

void I2CCommunication::handleI2CWrite(const char* data) {
    //Wire.beginTransmission(this->i2cAddress);
    //Wire.write(data);
    //Wire.endTransmission();
    LOG_INFO(&Serial, (String(text_i2c) + String("Dados enviados via I2C: ") + data).c_str());
}

void I2CCommunication::handleI2CRead() {
    Wire.requestFrom(this->i2cAddress, QUEUE_MESSAGE_SIZE);
    char receivedData[QUEUE_MESSAGE_SIZE];
    int index = 0;

    while (Wire.available() && index < QUEUE_MESSAGE_SIZE - 1) {
        receivedData[index++] = Wire.read();
    }
    receivedData[index] = '\0';

    LOG_INFO(&Serial, (String(text_i2c) + String("Dados recebidos via I2C: ") + String(receivedData)).c_str());

    if (this->SendQueue != nullptr) {
        xQueueSend(this->SendQueue, receivedData, portMAX_DELAY);
    }
}
