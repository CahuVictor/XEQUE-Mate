#include <Arduino.h>

#include "appl_MyLogger.h"
#include "appl_SerialCommunication.h"
#include "appl_WiFiManager.h"

// Instâncias globais das classes
SerialCommunication serialComm(&Serial);  // Passa a interface Serial como parâmetro
WiFiManager wifiManager;

// Queue FreeRtos
QueueHandle_t queue;                        // Queue onde todos os módulos podem escrever
QueueHandle_t WiFiQueue;                    // Queue onde apenas o módulo WiFi ler
QueueHandle_t SerialQueue;                    // Queue onde apenas o módulo WiFi ler
const int QueueElementSize = 10;

// Função da nova tarefa para imprimir o conteúdo da fila
void printQueueTask(void* pvParameters) {
    char receivedMessage[64];  // Buffer para armazenar a mensagem da fila
    for (;;) {
        // Tenta ler a fila sem bloquear
        if (xQueueReceive(queue, &receivedMessage, pdMS_TO_TICKS(100)) == pdPASS) {
            LOG_INFO(&Serial, ("[Queue Monitor] Mensagem recebida da fila: %s", receivedMessage));

            if (strstr(receivedMessage, "GET IP") != nullptr || 
                strstr(receivedMessage, "SSID:") != nullptr || 
                strstr(receivedMessage, "PASSWORD:") != nullptr)
            {
                // Comandos para WiFiManager
                if ( WiFiQueue != nullptr)
                {
                    xQueueSend( WiFiQueue , &receivedMessage, portMAX_DELAY);  // Envia o comando para a Queue de envio
                    LOG_INFO(&Serial, (String("[Queue Monitor] Comando enviado para a fila do WiFiManager. Comando: ") + String(receivedMessage)).c_str());
                } else 
                {
                    LOG_INFO(&Serial, "[Queue Monitor] Queue WiFi Manager não inicializado.");
                }
            }
        } else {
            LOG_INFO(&Serial, "[Queue Monitor] Nenhuma mensagem na fila.");
        }

        // Aguarda 10 segundos antes de verificar novamente
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void setup() {
    // Inicialização da comunicação serial
    Serial.begin(115200);
    while (!Serial);  // Aguarda a inicialização da porta serial

    MyLogger::initialize();  // Inicializa o logger com proteção de mutex se FreeRTOS estiver disponível
    LOG_INFO(&Serial, "Sistema inicializado com sucesso.");

    // Inicializa e configura a fila
    queue = xQueueCreate(QueueElementSize, sizeof(char) * QUEUE_MESSAGE_SIZE);        // Create the queue which will have <QueueElementSize>
                                                                                            // number of elements, each of size `message_t` and 
                                                                                            // pass the address to <QueueHandle>.
    WiFiQueue = xQueueCreate(QueueElementSize, sizeof(char) * QUEUE_MESSAGE_SIZE); 
    SerialQueue = xQueueCreate(QueueElementSize, sizeof(char) * QUEUE_MESSAGE_SIZE); 

    // Check if the queue was successfully created
    if (queue == NULL) {
        LOG_INFO(&Serial, "Queue could not be created. Halt.");
        while (1)
        {
            delay(1000);  // Halt at this point as is not possible to continue
        }
    }

    // Passa a fila para os módulos
    serialComm.setQueue(queue, SerialQueue);
    wifiManager.setQueue(queue, WiFiQueue);

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
    

    // Cria a nova tarefa para monitorar a fila
    xTaskCreate(printQueueTask, "Queue Monitor Task", 2048, NULL, 1, NULL);
}

void loop()
{
    // O loop principal não será usado, pois as tarefas são gerenciadas pelo FreeRTOS
    // Exemplo de log periódico
    LOG_INFO(&Serial, "Loop principal em execução.");

    delay(10000);
}
