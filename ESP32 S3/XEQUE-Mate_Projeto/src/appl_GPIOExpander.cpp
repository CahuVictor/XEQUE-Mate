#include "appl_GPIOExpander.h"

GPIOExpander::GPIOExpander() {}

void GPIOExpander::initialize() {
    // Configuração dos pinos de controle dos multiplexadores
    pinMode(MUX1_S0, OUTPUT);
    pinMode(MUX1_S1, OUTPUT);
    pinMode(MUX2_S0, OUTPUT);
    pinMode(MUX2_S1, OUTPUT);
    pinMode(MUX2_S2, OUTPUT);
    pinMode(MUX2_S3, OUTPUT);
    LOG_INFO(&Serial, "GPIOExpander: Multiplexadores inicializados.");
}

void GPIOExpander::selectChannel(uint8_t channel) {
    if (channel > 63) {
        LOG_ERROR(&Serial, "GPIOExpander: Canal fora do alcance.");
        return;
    }

    uint8_t mux1 = channel / 16;    // Define qual dos 4 multiplexadores será usado (MUX2 a MUX5)
    uint8_t mux2to5 = channel % 16; // Define o canal específico dentro do multiplexador selecionado

    selectMUX1(mux1);
    selectMUX2to5(mux2to5);

    char buffer[50];
    snprintf(buffer, sizeof(buffer), "GPIOExpander: Canal %d selecionado.", channel);
    LOG_INFO(&Serial, buffer);
}

void GPIOExpander::selectMUX1(uint8_t mux) {
    // Define os pinos MUX1_S0 e MUX1_S1 para selecionar o multiplexador desejado
    digitalWrite(MUX1_S0, mux & 0x01);
    digitalWrite(MUX1_S1, (mux >> 1) & 0x01);
}

void GPIOExpander::selectMUX2to5(uint8_t channel) {
    // Define os pinos MUX2_S0 a MUX2_S3 para selecionar o canal desejado no multiplexador ativo
    digitalWrite(MUX2_S0, channel & 0x01);
    digitalWrite(MUX2_S1, (channel >> 1) & 0x01);
    digitalWrite(MUX2_S2, (channel >> 2) & 0x01);
    digitalWrite(MUX2_S3, (channel >> 3) & 0x01);
}
