#include "appl_RFIDControl.h"

const char text_rfidcontrol[] =  "RFIDControl::";

RFIDControl::RFIDControl() : SendQueue(nullptr), ReceiveQueue(nullptr), spiMutex(nullptr), readMode(READ_DISABLED), readInterval(5000) {}

void RFIDControl::initialize() {
    gpioExpander.initialize();
    SPI.begin();
    spiMutex = xSemaphoreCreateMutex();
    if (spiMutex == nullptr) {
        LOG_ERROR(&Serial, (String(text_rfidcontrol) + String("Falha ao criar mutex SPI.")).c_str());
    }

    for( int i=0 ; i <= 7; i++)
    {
        for( int j=0 ; j <= 7; j++)
        {
            this->matrizPos[i][j] = 0;
        }
    }
    
    LOG_DEBUG(&Serial, (String(text_rfidcontrol) + String("SPI, expansor de GPIO e mutex inicializados.")).c_str());
}

void RFIDControl::setReadMode(RFIDReadMode mode) {
    this->readMode = mode;
    LOG_DEBUG(&Serial, mode == CONTINUOUS_READ ? (String(text_rfidcontrol) + String("Modo contínuo ativado")).c_str() :
                       mode == SINGLE_READ     ? (String(text_rfidcontrol) + String("Modo de leitura única ativado")).c_str() :
                                                 (String(text_rfidcontrol) + String("Leitura desativada")).c_str());
}

void RFIDControl::setReadInterval(uint32_t interval_ms) {
    this->readInterval = interval_ms;
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%cIntervalo de leitura configurado para %d ms", text_rfidcontrol, interval_ms);
    LOG_DEBUG(&Serial, buffer);
    LOG_DEBUG(&Serial, (String(text_rfidcontrol) + 
                        String("Intervalo de leitura configurado para ") +
                        String(interval_ms) + String(" ms")).c_str());
}

void RFIDControl::scanAll() {
    LOG_DEBUG(&Serial, (String(text_rfidcontrol) + String("Iniciando escaneamento de todos os 64 RFID.")).c_str());
    #ifdef __RFID_USE_FAKE_FUNCTIONS__
        this->updateMatrizFake();
    #endif
    for (uint8_t i = 0; i < 64; i++) {
        this->readRFID(i);
    }
}

void RFIDControl::readRFID(uint8_t rfidID) {
    gpioExpander.selectChannel(rfidID % 16);  // Seleciona o canal do RFID no multiplexador

    #ifdef __RFID_USE_FAKE_FUNCTIONS__
        LOG_DEBUG(&Serial, (String(text_rfidcontrol) + String("RFID " + String(rfidID) + 
                            String(": Status fictício ") + 
                            getPiece( this->matrizPos[rfidID/8][rfidID%8] ))).c_str());
    #else
        if (xSemaphoreTake(spiMutex, portMAX_DELAY) == pdTRUE) {
            // Código real para comunicação SPI com o RFID
            // SPI.transfer(...); // Exemplo fictício de comando SPI
            xSemaphoreGive(spiMutex);
        } else {
            LOG_ERROR(&Serial, "RFIDControl: Falha ao obter o mutex SPI.");
        }
    #endif
}

void RFIDControl::setQueue(QueueHandle_t SendQueue, QueueHandle_t ReceiveQueue) {
    this->SendQueue = SendQueue;
    this->ReceiveQueue = ReceiveQueue;
}

void RFIDControl::startTask() {
    xTaskCreate(taskWrapper, "RFID Control Task", 4096, this, 1, NULL);
}

bool RFIDControl::sendMessageToRFIDQueue(const char* msg) {
    if (this->SendQueue != nullptr) {
        if (xQueueSend(this->SendQueue, msg, portMAX_DELAY) == pdPASS)
        {
            LOG_DEBUG(&Serial, (String(text_rfidcontrol) + String("Sucesso ao enviar mensagem para a fila de RFID.")).c_str());
            return true;
        } else {
            LOG_WARNING(&Serial, (String(text_rfidcontrol) + String("Falha ao enviar mensagem para a fila de RFID.")).c_str());
        }
    }
    return false;
}

void RFIDControl::monitorTask() {
    char msg[32];
    for (;;) {
        if (this->readMode == CONTINUOUS_READ) {
            this->scanAll();
            vTaskDelay(pdMS_TO_TICKS(this->readInterval));
        } else
        {
            if (this->readMode == SINGLE_READ)
            {
                this->scanAll();
                this->readMode = READ_DISABLED;  // Após a leitura única, desativa o modo de leitura
            }
        }

        this->processCommand();  // Processa comandos recebidos

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void RFIDControl::processCommand() {
    LOG_DEBUG(&Serial, (String(text_rfidcontrol) + String("Verificando ReceiveQueue...")).c_str());
    if (this->ReceiveQueue != nullptr) {
        LOG_DEBUG(&Serial, (String(text_rfidcontrol) + String("ReceiveQueue != nullptr")).c_str());
        char command[QUEUE_MESSAGE_SIZE];
        if (xQueueReceive((this->ReceiveQueue), &command, portMAX_DELAY/*pdMS_TO_TICKS(100)*/)== pdPASS)
        {
            LOG_DEBUG(&Serial, (String(text_rfidcontrol) + String("ReceiveQueue = ") + String(command)).c_str());
            if (strncmp(command, "READ RFID", 9) == 0)
            {
                //LOG_DEBUG(&Serial, (String(text_rfidcontrol) + String("Escaneando RFID conforme mensagem recebida.")).c_str());
                //this->scanAll();
                //this->readMode = READ_DISABLED;  // Após a leitura única, desativa o modo de leitura
                LOG_DEBUG(&Serial, (String(text_rfidcontrol) + String("Atualizando modo para leitura única.")).c_str());
                this->readMode = SINGLE_READ;
            } else
            {
                LOG_DEBUG(&Serial, (String(text_rfidcontrol) + String("Comando recebido desconhecido.")).c_str());
            }
        }
    }
}






#ifdef __RFID_USE_FAKE_FUNCTIONS__
    String RFIDControl::getPiece(int statusPos)
    {
        if (statusPos ==  0) return "ausente";
        if (statusPos ==  1) return "peão Branco";
        if (statusPos == -1) return "peão Preto";
        if (statusPos ==  2) return "cavalo Branco";
        if (statusPos == -2) return "cavalo Preto";
        if (statusPos ==  3) return "bispo Branco";
        if (statusPos == -3) return "bispo Preto";
        if (statusPos ==  4) return "torre Branco";
        if (statusPos == -4) return "torre Preto";
        if (statusPos ==  5) return "rainha Branco";
        if (statusPos == -5) return "rainha Preto";
        if (statusPos ==  6) return "rei Branco";
        if (statusPos == -6) return "rei Preto";
        LOG_DEBUG(&Serial, (String(text_rfidcontrol) + String("Cadastro de peça inexistente")).c_str());
        return "ERROR";
    }
    
    void RFIDControl::updateMatrizFake()
    {
        LOG_DEBUG(&Serial, (String(text_rfidcontrol) + String("Funcões fictícias ativadas.")).c_str());
        static bool startedMatriz = false;
        if (!startedMatriz)
        {
            // Posicionando peões
            for( int i=0 ; i <= 7; i++)
            {
                this->matrizPos[1][i] = 1;
                this->matrizPos[6][i] = -1;
            }
            // Posicionando torres
            this->matrizPos[0][0] = 4;
            this->matrizPos[0][7] = 4;
            this->matrizPos[7][0] = -4;
            this->matrizPos[7][7] = -4;
            // Posicionando cavalos
            this->matrizPos[0][1] = 2;
            this->matrizPos[0][6] = 2;
            this->matrizPos[7][1] = -2;
            this->matrizPos[7][6] = -2;
            // Posicionando bispos
            this->matrizPos[0][2] = 3;
            this->matrizPos[0][5] = 3;
            this->matrizPos[7][2] = -3;
            this->matrizPos[7][5] = -3;
            // Posicionando Damas
            this->matrizPos[0][4] = 5;
            this->matrizPos[7][4] = -5;
            // Posicionando Rei
            this->matrizPos[0][3] = 6;
            this->matrizPos[7][3] = -6;
        }
        else
        {
            int LastPos[2];
            LastPos[0] = random(0, 7);
            LastPos[1] = random(0, 7);
            int NextPos[2];
            NextPos[0] = random(0, 7);
            NextPos[1] = random(0, 7);
            this->matrizPos[NextPos[0]][NextPos[1]] = this->matrizPos[LastPos[0]][LastPos[1]];
            this->matrizPos[LastPos[0]][LastPos[1]] = 0;
        }
    }
#endif