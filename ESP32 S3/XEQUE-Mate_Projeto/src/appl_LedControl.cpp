#include "appl_LedControl.h"

LedControl::LedControl() : ledQueue(nullptr) {}

void LedControl::initialize() {
    #ifdef USE_FAKE_FUNCTIONS
    LOG_INFO(&Serial, "LedControl: Inicialização fictícia dos LEDs concluída.");
    #else
    // Código real de inicialização aqui
    #endif

    // Cria a fila para mensagens de controle
    ledQueue = xQueueCreate(10, sizeof(char) * 32);  // Fila com 10 mensagens de até 32 caracteres
    if (ledQueue == nullptr) {
        LOG_ERROR(&Serial, "LedControl: Falha ao criar a fila de controle de LEDs.");
    }
}

void LedControl::setBrightness(uint8_t level) {
    #ifdef USE_FAKE_FUNCTIONS
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "LedControl: Definindo brilho fictício para %d.", level);
    LOG_INFO(&Serial, buffer);
    #else
    // Código real para definir o brilho dos LEDs
    #endif
}

void LedControl::updateLEDs(const uint32_t* colorData, size_t length) {
    #ifdef USE_FAKE_FUNCTIONS
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "LedControl: Atualizando %d LEDs com dados fictícios.", length);
    LOG_INFO(&Serial, buffer);
    for (size_t i = 0; i < length; i++) {
        snprintf(buffer, sizeof(buffer), "LED %d: Cor fictícia %06X", i, random(0xFFFFFF));
        LOG_DEBUG(&Serial, buffer);
    }
    #else
    // Código real para atualizar LEDs
    #endif
}

void LedControl::startTask() {
    xTaskCreate(taskWrapper, "LED Control Task", 2048, this, 1, NULL);
}

void LedControl::sendMessage(const char* msg) {
    if (ledQueue != nullptr) {
        if (xQueueSend(ledQueue, msg, portMAX_DELAY) != pdPASS) {
            LOG_WARNING(&Serial, "LedControl: Falha ao enviar mensagem para a fila de LEDs.");
        }
    }
}

void LedControl::ledControlTask() {
    char msg[32];
    for (;;) {
        if (xQueueReceive(ledQueue, msg, portMAX_DELAY) == pdPASS) {
            char buffer[64];
            snprintf(buffer, sizeof(buffer), "LedControl: Processando mensagem \"%s\"", msg);
            LOG_INFO(&Serial, buffer);

            // Ação fictícia baseada na mensagem recebida
            updateFakeLEDs();
        }
    }
}

void LedControl::updateFakeLEDs() {
    LOG_INFO(&Serial, "LedControl: Executando sequência fictícia de LEDs.");
    for (int i = 0; i < 256; i++) {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "LED %d: Cor fictícia %06X", i, random(0xFFFFFF));
        LOG_DEBUG(&Serial, buffer);
        delay(10);  // Simula o tempo de atualização do LED
    }
}
