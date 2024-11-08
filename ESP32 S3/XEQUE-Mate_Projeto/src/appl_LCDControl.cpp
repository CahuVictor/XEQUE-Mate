#include "appl_LCDControl.h"

LCDControl::LCDControl() 
    : //lcd1(LCD1_ADDR, LCD_COLUMNS, LCD_ROWS), 
      //lcd2(LCD2_ADDR, LCD_COLUMNS, LCD_ROWS),
      lcdQueue(nullptr) {}

void LCDControl::initialize() {
    /*lcd1.begin(LCD_COLUMNS, LCD_ROWS);
    lcd1.backlight();
    lcd1.clear();*/
    LOG_INFO(&Serial, "LCDControl: LCD1 inicializado.");

    /*lcd2.begin(LCD_COLUMNS, LCD_ROWS);
    lcd2.backlight();
    lcd2.clear();*/
    LOG_INFO(&Serial, "LCDControl: LCD2 inicializado.");

    // Cria a fila para mensagens do LCD
    lcdQueue = xQueueCreate(10, sizeof(LCDMessage));
    if (lcdQueue == nullptr) {
        LOG_ERROR(&Serial, "LCDControl: Falha ao criar a fila de controle do LCD.");
    }
}

void LCDControl::startTask() {
    xTaskCreate(taskWrapper, "LCD Control Task", 4096, this, 1, NULL);
}

bool LCDControl::sendMessageToLCD(uint8_t lcdNum, const char* message, uint8_t row) {
    if (lcdQueue != nullptr) {
        LCDMessage msg;
        msg.lcdNum = lcdNum;
        strncpy(msg.message, message, LCD_COLUMNS);  // Limita a mensagem ao número de colunas
        msg.message[LCD_COLUMNS] = '\0';  // Garante que a mensagem está terminada em null
        msg.row = row;

        if (xQueueSend(lcdQueue, &msg, portMAX_DELAY) == pdPASS) {
            return true;
        } else {
            LOG_WARNING(&Serial, "LCDControl: Falha ao enviar mensagem para a fila do LCD.");
        }
    }
    return false;
}

void LCDControl::clearDisplay(uint8_t lcdNum) {
    if (lcdNum == 1) {
        //lcd1.clear();
        LOG_INFO(&Serial, "LCDControl: LCD1 limpo.");
    } else if (lcdNum == 2) {
        //lcd2.clear();
        LOG_INFO(&Serial, "LCDControl: LCD2 limpo.");
    } else {
        LOG_ERROR(&Serial, "LCDControl: Número do LCD inválido.");
    }
}

void LCDControl::setBacklight(uint8_t lcdNum, bool state) {
    if (lcdNum == 1) {
        //state ? lcd1.backlight() : lcd1.noBacklight();
        LOG_INFO(&Serial, state ? "LCDControl: Backlight LCD1 ligado." : "LCDControl: Backlight LCD1 desligado.");
    } else if (lcdNum == 2) {
        //state ? lcd2.backlight() : lcd2.noBacklight();
        LOG_INFO(&Serial, state ? "LCDControl: Backlight LCD2 ligado." : "LCDControl: Backlight LCD2 desligado.");
    } else {
        LOG_ERROR(&Serial, "LCDControl: Número do LCD inválido.");
    }
}

void LCDControl::displayStatus() {
    sendMessageToLCD(1, "Status OK", 0);
    sendMessageToLCD(2, "Status OK", 0);
}

void LCDControl::lcdControlTask() {
    LCDMessage msg;
    for (;;) {
        if (xQueueReceive(lcdQueue, &msg, portMAX_DELAY) == pdPASS) {
            LOG_INFO(&Serial, "LCDControl: Atualizando LCD com nova mensagem.");

            /*if (msg.lcdNum == 1) {
                updateLCD(lcd1, msg.message, msg.row);
            } else if (msg.lcdNum == 2) {
                updateLCD(lcd2, msg.message, msg.row);
            } else {
                LOG_ERROR(&Serial, "LCDControl: Número do LCD inválido na mensagem.");
            }*/
        }
    }
}

void LCDControl::updateLCD(/*LiquidCrystal_I2C &lcd,*/ const char* message, uint8_t row) {
    //lcd.setCursor(0, row);
    //lcd.print("                "); // Limpa a linha (16 espaços)
    //lcd.setCursor(0, row);
    //lcd.print(message);
}
