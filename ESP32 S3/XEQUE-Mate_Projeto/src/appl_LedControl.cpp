#include "appl_LedControl.h"

const char text_ledcontrol[] =  "LedControl::";

LedControl::LedControl() : SendQueue(nullptr), ReceiveQueue(nullptr) {}

void LedControl::initialize() {
    #ifdef __LED_USE_FAKE_FUNCTIONS__
        LOG_DEBUG(&Serial, (String(text_ledcontrol) + String("Inicialização fictícia dos LEDs concluída.")).c_str());
    #else
    // Código real de inicialização aqui
    #endif
}

void LedControl::setBrightness(uint8_t level) {
    #ifdef __LED_USE_FAKE_FUNCTIONS__
        LOG_DEBUG(&Serial, (String(text_ledcontrol) + String("Definindo brilho fictício para ") + String(level)).c_str());
    #else
    // Código real para definir o brilho dos LEDs
    #endif
}

void LedControl::updateLEDs(const uint32_t* colorData, size_t length) {
    #ifdef __LED_USE_FAKE_FUNCTIONS__
        LOG_DEBUG(&Serial, (String(text_ledcontrol) + String("Atualizando ") +
                            String(length) + String(" LEDs com dados fictícios.")).c_str());
        for (size_t i = 0; i < length; i++) {
            LOG_DEBUG(&Serial, (String(text_ledcontrol) + String("LED ") +
                                String(i) + String(": Cor fictícia ") + String(random(0xFFFFFF))).c_str());
        }
    #else
    // Código real para atualizar LEDs
    #endif
}

void LedControl::setQueue(QueueHandle_t SendQueue, QueueHandle_t ReceiveQueue) {
    this->SendQueue = SendQueue;
    this->ReceiveQueue = ReceiveQueue;
}

void LedControl::startTask() {
    xTaskCreate(taskWrapper, "RFID Control Task", 4096, this, 1, NULL);
}

bool LedControl::sendMessageToLedControlQueue(const char* msg) {
    if (this->SendQueue != nullptr) {
        if (xQueueSend(this->SendQueue, msg, portMAX_DELAY) == pdPASS)
        {
            LOG_DEBUG(&Serial, (String(text_ledcontrol) + String("Sucesso ao enviar mensagem para a fila de LedControl.")).c_str());
            return true;
        } else {
            LOG_WARNING(&Serial, (String(text_ledcontrol) + String("Falha ao enviar mensagem para a fila de LedControl.")).c_str());
        }
    }
    return false;
}

void LedControl::monitorTask() {
    for (;;) {
        this->processCommand();

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void LedControl::processCommand() {
    char msg[32];
    if (xQueueReceive(ReceiveQueue, msg, portMAX_DELAY) == pdPASS) {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "LedControl: Processando mensagem \"%s\"", msg);
        LOG_INFO(&Serial, buffer);
        LOG_DEBUG(&Serial, (String(text_ledcontrol) + String("Sucesso ao enviar mensagem para a fila de LedControl.")).c_str());

        // Ação fictícia baseada na mensagem recebida
        updateFakeLEDs();
    }
}





#ifdef __LED_USE_FAKE_FUNCTIONS__
    void LedControl::updateFakeLEDs() {
        LOG_DEBUG(&Serial, (String(text_ledcontrol) + String("Executando sequência fictícia de LEDs.")).c_str());
        for (int i = 0; i < 256; i++) {
            LOG_DEBUG(&Serial, (String(text_ledcontrol) + String("LED ") +
                                String(i) + String(": Cor fictícia ") + String(random(0xFFFFFF))).c_str());
            delay(10);  // Simula o tempo de atualização do LED
        }
    }
#endif
