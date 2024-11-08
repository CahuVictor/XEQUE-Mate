#ifndef __appl_LCDControl_h__
#define __appl_LCDControl_h__

#include "Arduino.h"
#include <Wire.h>
//#include <LiquidCrystal_I2C.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include "appl_MyLogger.h"

// Endereços I2C dos displays LCD
#define LCD1_ADDR 0x27
#define LCD2_ADDR 0x26

// Dimensões do display LCD
#define LCD_COLUMNS 16
#define LCD_ROWS 2

class LCDControl {
public:
    LCDControl();                            // Construtor
    void initialize();                       // Inicializa os displays LCD
    void startTask();                        // Inicia a tarefa FreeRTOS para controle do LCD
    bool sendMessageToLCD(uint8_t lcdNum, const char* message, uint8_t row = 0); // Envia uma mensagem para o LCD

    void clearDisplay(uint8_t lcdNum);       // Limpa o LCD especificado
    void setBacklight(uint8_t lcdNum, bool state); // Liga ou desliga o backlight do LCD especificado
    void displayStatus();                    // Exibe informações de status nos LCDs

private:
    //LiquidCrystal_I2C lcd1;                  // Instância do primeiro LCD
    //LiquidCrystal_I2C lcd2;                  // Instância do segundo LCD
    QueueHandle_t lcdQueue;                  // Fila para mensagens do LCD

    void lcdControlTask();                   // Tarefa FreeRTOS para controle do LCD
    void updateLCD(/*LiquidCrystal_I2C &lcd,*/ const char* message, uint8_t row); // Função auxiliar para atualizar mensagem no LCD
    static void taskWrapper(void* _this) {
        static_cast<LCDControl*>(_this)->lcdControlTask();
    }

    struct LCDMessage {
        uint8_t lcdNum;                      // Número do LCD (1 ou 2)
        char message[LCD_COLUMNS + 1];       // Mensagem a ser exibida
        uint8_t row;                         // Linha onde a mensagem será exibida
    };
};

#endif
