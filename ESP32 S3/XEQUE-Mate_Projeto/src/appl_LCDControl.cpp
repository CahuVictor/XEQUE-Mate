#include "appl_LCDControl.h"

const char text_lcdcontrol[] =  "LCDControl::";

LCDControl::LCDControl() 
    : //lcd1(LCD1_ADDR, LCD_COLUMNS, LCD_ROWS), 
      //lcd2(LCD2_ADDR, LCD_COLUMNS, LCD_ROWS),
      SendQueue(nullptr), 
      ReceiveQueue(nullptr) {}

void LCDControl::initialize() {
    /*lcd1.begin(LCD_COLUMNS, LCD_ROWS);
    lcd1.backlight();
    lcd1.clear();*/
    LOG_DEBUG(&Serial, (String(text_lcdcontrol) + String("LCD1 inicializado.")).c_str());

    /*lcd2.begin(LCD_COLUMNS, LCD_ROWS);
    lcd2.backlight();
    lcd2.clear();*/
    LOG_DEBUG(&Serial, (String(text_lcdcontrol) + String("LCD2 inicializado.")).c_str());
}

void LCDControl::clearDisplay(uint8_t lcdNum) {
    if (lcdNum == 1) {
        //lcd1.clear();
        LOG_DEBUG(&Serial, (String(text_lcdcontrol) + String("LCD1 limpo.")).c_str());
    } else if (lcdNum == 2) {
        //lcd2.clear();
        LOG_DEBUG(&Serial, (String(text_lcdcontrol) + String("LCD2 limpo.")).c_str());
    } else {
        LOG_ERROR(&Serial, (String(text_lcdcontrol) + String("Número do LCD inválido.")).c_str());
    }
}

void LCDControl::setBacklight(uint8_t lcdNum, bool state) {
    if (lcdNum == 1) {
        //state ? lcd1.backlight() : lcd1.noBacklight();
        //LOG_INFO(&Serial, state ? "LCDControl: Backlight LCD1 ligado." : "LCDControl: Backlight LCD1 desligado.");
        LOG_DEBUG(&Serial, (String(text_lcdcontrol) + String(state ? "Backlight LCD1 ligado." : "Backlight LCD1 desligado.")).c_str());
    } else if (lcdNum == 2) {
        //state ? lcd2.backlight() : lcd2.noBacklight();
        //LOG_INFO(&Serial, state ? "LCDControl: Backlight LCD2 ligado." : "LCDControl: Backlight LCD2 desligado.");
        LOG_DEBUG(&Serial, (String(text_lcdcontrol) + String(state ? "Backlight LCD2 ligado." : "Backlight LCD2 desligado.")).c_str());
    } else {
        LOG_ERROR(&Serial, (String(text_lcdcontrol) + String("Número do LCD inválido.")).c_str());
    }
}

bool LCDControl::sendMessageToLCD( uint8_t lcdNum, const char* message, uint8_t row ) {
    msg.lcdNum = lcdNum;
    strncpy(msg.message, message, LCD_COLUMNS);  // Limita a mensagem ao número de colunas
    msg.message[LCD_COLUMNS] = '\0';  // Garante que a mensagem está terminada em null
    msg.row = row;
    LOG_DEBUG(&Serial, (String(text_lcdcontrol) + String("LCD") + String(msg.lcdNum) + 
                                                  String("msg:") + String(msg.message)).c_str());
    return false;
}

void LCDControl::displayStatus() {
    sendMessageToLCD( 1, "Status OK", 0 );
    sendMessageToLCD( 2, "Status OK", 0 );
    LOG_DEBUG(&Serial, (String(text_lcdcontrol) + String("Status OK enviado para o LCDs.")).c_str());
}

void LCDControl::updateLCD(/*LiquidCrystal_I2C &lcd,*/ const char* message, uint8_t row) {
    //lcd.setCursor(0, row);
    //lcd.print("                "); // Limpa a linha (16 espaços)
    //lcd.setCursor(0, row);
    //lcd.print(message);
}

void LCDControl::setQueue(QueueHandle_t SendQueue, QueueHandle_t ReceiveQueue) {
    this->SendQueue = SendQueue;
    this->ReceiveQueue = ReceiveQueue;
}

void LCDControl::startTask() {
    xTaskCreate(taskWrapper, "LCD Control Task", 4096, this, 1, NULL);
}

bool LCDControl::sendMessageToLCDControlQueue(const char* msg) {
    if (this->SendQueue != nullptr) {
        if (xQueueSend(this->SendQueue, msg, portMAX_DELAY) == pdPASS)
        {
            LOG_DEBUG(&Serial, (String(text_lcdcontrol) + String("Sucesso ao enviar mensagem para a fila de LCDControl.")).c_str());
            return true;
        } else {
            LOG_WARNING(&Serial, (String(text_lcdcontrol) + String("Falha ao enviar mensagem para a fila de LCDControl.")).c_str());
        }
    }
    return false;
}

void LCDControl::monitorTask() {
    for (;;) {
        this->processCommand();

        /*if (msg.lcdNum == 1) {
            updateLCD(lcd1, msg.message, msg.row);
        } else if (msg.lcdNum == 2) {
            updateLCD(lcd2, msg.message, msg.row);
        } else {
            LOG_ERROR(&Serial, "LCDControl: Número do LCD inválido na mensagem.");
        }*/

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void LCDControl::processCommand() {
        /*LCDMessage msg;
        msg.lcdNum = lcdNum;
        strncpy(msg.message, message, LCD_COLUMNS);  // Limita a mensagem ao número de colunas
        msg.message[LCD_COLUMNS] = '\0';  // Garante que a mensagem está terminada em null
        msg.row = row;

        if (xQueueSend(lcdQueue, &msg, portMAX_DELAY) == pdPASS) {
            return true;
        } else {
            LOG_WARNING(&Serial, "LCDControl: Falha ao enviar mensagem para a fila do LCD.");
        }*/
        // update msg
    if (this->ReceiveQueue != nullptr) {
        //LOG_DEBUG(&Serial, ("WiFiManager: ReceiveQueue != nullptr"));
        char command[QUEUE_MESSAGE_SIZE];
        if (xQueueReceive((this->ReceiveQueue), &command, pdMS_TO_TICKS(100))== pdPASS)
        {
            LOG_DEBUG(&Serial, (String(text_lcdcontrol) + String("Comando recebido via ReceiveQueue: ") + String(command)).c_str());
            if (strncmp(command, "SEND LCD:", 9) == 0)
            {
                // Tratamento do comando SSID
                String ssid = String(command + 9);
                LOG_DEBUG(&Serial, (String(text_lcdcontrol) + String("Comando valido: ") + ssid).c_str());

            } else
            {
                LOG_DEBUG(&Serial, (String(text_lcdcontrol) + String("Comando recebido desconhecido.")).c_str());
            }
        } /*else
        {
            LOG_DEBUG(&Serial, ("WiFiManager: Falhou ao tentar coletar o comando da ReceiveQueue"));
        }*/

    } else {
        //LOG_DEBUG(&Serial, ("WiFiManager: ReceiveQueue == nullptr"));
    }
}