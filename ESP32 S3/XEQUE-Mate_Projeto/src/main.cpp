#include <Arduino.h>

//#include "MyLogger.h"
#include "appl_SerialCommunication.h"
#include "appl_WiFiManager.h"

// Instâncias globais das classes
SerialCommunication serialComm(&Serial);  // Passa a interface Serial como parâmetro
WiFiManager wifiManager;
QueueHandle_t __queue__;
const int QueueElementSize = 10;

void setup() {
    // Inicialização da comunicação serial
    Serial.begin(115200);
    while (!Serial);  // Aguarda a inicialização da porta serial

    MyLogger::initialize();  // Inicializa o logger com proteção de mutex se FreeRTOS estiver disponível
    LOG_INFO(&Serial, "Sistema inicializado com sucesso.");

    // Inicializa e configura a fila
    __queue__ = xQueueCreate(QueueElementSize, sizeof(char) * QUEUE_MESSAGE_SIZE);        // Create the queue which will have <QueueElementSize>
                                                                                            // number of elements, each of size `message_t` and 
                                                                                            // pass the address to <QueueHandle>.
    // Check if the queue was successfully created
    if (__queue__ == NULL) {
        LOG_INFO(&Serial, "Queue could not be created. Halt.");
        while (1)
        {
            delay(1000);  // Halt at this point as is not possible to continue
        }
    }

    // Passa a fila para os módulos
    serialComm.setQueue(&__queue__);
    wifiManager.setQueue(&__queue__);

    // Inicialização dos módulos
    serialComm.initialize();
    wifiManager.initialize();

    // Inicia as tarefas
    serialComm.startTask();
    wifiManager.startTask();

    // Inicia tarefas FreeRTOS para cada módulo (exemplo)
    //wifiManager.startTask();            //  http://http://192.168.4.1/
    //serialComm.setWiFiTaskHandle(wifiManager.getWiFiTaskHandle());
    //serialComm.startTask();

    // Configurações de teste
    serialComm.sendMessage("Hello from SerialCommunication!");
}

void loop() {
    // O loop principal não será usado, pois as tarefas são gerenciadas pelo FreeRTOS
    // Exemplo de log periódico
    LOG_INFO(&Serial, "Loop principal em execução.");

    delay(10000);
}
