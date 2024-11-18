#include "appl_ButtonControl.h"

const char text_buttoncontrol[] = "ButtonControl::";

ButtonControl::ButtonControl(uint8_t buttonPins[BUTTON_COUNT]) 
    : SendQueue(nullptr), ReceiveQueue(nullptr) {
    for (int i = 0; i < BUTTON_COUNT; i++) {
        this->buttonPins[i] = buttonPins[i];
        this->buttonPressed[i] = false;
    }
}

void ButtonControl::initialize() {
    for (int i = 0; i < BUTTON_COUNT; i++) {
        pinMode(buttonPins[i], INPUT_PULLUP); // Configura os pinos dos botões com pull-up interno
        attachInterruptArg(digitalPinToInterrupt(buttonPins[i]), isrButtonPress, (void*)this, FALLING); // Configura interrupção
        LOG_DEBUG(&Serial, (String(text_buttoncontrol) + String("Botão inicializado no pino ") + String(buttonPins[i])).c_str());
    }
}

void ButtonControl::setQueue(QueueHandle_t SendQueue, QueueHandle_t ReceiveQueue) {
    this->SendQueue = SendQueue;
    this->ReceiveQueue = ReceiveQueue;
}

bool ButtonControl::sendMessageToQueue(const char* message) {
    if (this->SendQueue != nullptr) {
        if (xQueueSend(this->SendQueue, message, portMAX_DELAY) == pdPASS) {
            return true;
        } else {
            LOG_WARNING(&Serial, (String(text_buttoncontrol) + String("Falha ao enviar mensagem para a fila.")).c_str());
        }
    }
    return false;
}

void ButtonControl::startTask() {
    xTaskCreate(this->taskWrapper, "Button Task", 2048, this, 1, NULL);
}

void ButtonControl::monitorTask() {
    for (;;) {
        for (int i = 0; i < BUTTON_COUNT; i++) {
            if (buttonPressed[i]) {
                buttonPressed[i] = false;  // Reseta o estado do botão
                LOG_DEBUG(&Serial, (String(text_buttoncontrol) + String("Botão ") + String(i + 1) + String(" pressionado!")).c_str());
                char message[QUEUE_MESSAGE_SIZE];
                snprintf(message, sizeof(message), "Botão %d pressionado", i + 1);
                this->sendMessageToQueue(message);
            }
        }

        this->processCommand();
        vTaskDelay(pdMS_TO_TICKS(100)); // Pequeno delay para liberar a CPU
    }
}

void ButtonControl::processCommand() {
    if (this->ReceiveQueue != nullptr) {
        char command[QUEUE_MESSAGE_SIZE];
        if (xQueueReceive(this->ReceiveQueue, &command, pdMS_TO_TICKS(100)) == pdPASS) {
            LOG_DEBUG(&Serial, (String(text_buttoncontrol) + String("Comando recebido: ") + String(command)).c_str());
            
            // Processa o comando (exemplo)
            if (strcmp(command, "RESET") == 0) {
                LOG_INFO(&Serial, (String(text_buttoncontrol) + String("Comando RESET recebido")).c_str());
                // Adicione lógica para tratar o comando RESET
            }
        }
    }
}

void ButtonControl::handleButtonPress(int buttonIndex) {
    if (buttonIndex >= 0 && buttonIndex < BUTTON_COUNT) {
        buttonPressed[buttonIndex] = true;  // Define o estado do botão como pressionado
    }
}

void IRAM_ATTR ButtonControl::isrButtonPress(void* arg) {
    ButtonControl* buttonControl = static_cast<ButtonControl*>(arg);
    for (int i = 0; i < BUTTON_COUNT; i++) {
        if (digitalRead(buttonControl->buttonPins[i]) == LOW) {
            buttonControl->handleButtonPress(i);
        }
    }
}
