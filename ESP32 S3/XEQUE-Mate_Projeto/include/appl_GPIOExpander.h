#ifndef __appl_GPIOExpander_h__
#define __appl_GPIOExpander_h__

#include "Arduino.h"
#include "appl_MyLogger.h"

// Define os pinos usados para controle do multiplexador
#define MUX1_S0  4
#define MUX1_S1  5
#define MUX2_S0  6
#define MUX2_S1  7
#define MUX2_S2  8
#define MUX2_S3  9

class GPIOExpander {
public:
    GPIOExpander();                      // Construtor
    void initialize();                   // Inicializa os multiplexadores
    void selectChannel(uint8_t channel); // Seleciona o canal desejado de 0 a 63

private:
    void selectMUX1(uint8_t mux);        // Seleciona qual MUX2-MUX5 será usado
    void selectMUX2to5(uint8_t channel); // Seleciona o canal no MUX2-MUX5
};

#endif
