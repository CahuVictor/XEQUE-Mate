#include <Arduino.h>

//#include "appl_MyLogger.h"
//#include "appl_LedControl.h"
//#include "appl_RFIDControl.h"
    //#include "ButtonControl.h"
//#include "appl_LCDControl.h"
#include "appl_SerialCommunication.h"
#include "appl_WiFiManager.h"
//#include "appl_WebServerControl.h"
    /*#include "I2CCommunication.h"
    #include "PowerMonitor.h"
    #include "SupervisorTask.h"
    #include "StateMachine.h"*/

// Instâncias globais das classes
//LedControl ledControl;
//RFIDControl rfidControl;
    //ButtonControl buttonControl;
//LCDControl lcdControl;
SerialCommunication serialComm(&Serial);  // Passa a interface Serial como parâmetro
QueueHandle_t serialQueue;
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
//WebServerControl webServer(8080);  // Porta 80 para o servidor web
    /*I2CCommunication i2cComm;
    PowerMonitor powerMonitor;
    SupervisorTask supervisor;
    StateMachine stateMachine;*/

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
    //ledControl.initialize();
    //rfidControl.initialize();
    //buttonControl.initialize();
    //lcdControl.initialize();
    serialComm.initialize();
    serialQueue = xQueueCreate(10, sizeof(char) * QUEUE_MESSAGE_SIZE);
    wifiManager.setSerialQueue(serialQueue);    // Passa a fila para WiFiManager
    wifiManager.initialize();

    // Inicia as tarefas
    serialComm.startTask();
    wifiManager.startTask();
    //webServer.initialize();
        /*i2cComm.initialize();
        powerMonitor.initialize();
        supervisor.initialize();
        stateMachine.initialize();*/

    // Inicia tarefas FreeRTOS para cada módulo (exemplo)
    //wifiManager.startTask();            //  http://http://192.168.4.1/
    //serialComm.setWiFiTaskHandle(wifiManager.getWiFiTaskHandle());
    //serialComm.startTask();
    //ledControl.startTask();
    //rfidControl.startTask();
    /*xTaskCreate([](void*) { buttonControl.readButtons(); }, "Button Task", 2048, NULL, 1, NULL);
    xTaskCreate([](void*) { powerMonitor.logConsumption(); }, "Power Monitor Task", 2048, NULL, 1, NULL);
    xTaskCreate([](void*) { supervisor.monitorTasks(); }, "Supervisor Task", 2048, NULL, 1, NULL);*/
    wifiManager.startWiFiTask();            //  http://http://192.168.4.1/
    serialComm.setWiFiTaskHandle(wifiManager.getWiFiTaskHandle());
    //webServer.startTask();                  //  http://<IP_DO_ESP>/     http://<IP_DO_ESP>/status
    serialComm.startTasks();
    //xTaskCreate([](void*) { stateMachine.updateState(0); }, "State Machine Task", 2048, NULL, 1, NULL);*

    // Configurações de teste
    

    // Cria a nova tarefa para monitorar a fila
    xTaskCreate(printQueueTask, "Queue Monitor Task", 2048, NULL, 1, NULL);
    //ledControl.sendMessage("Iniciar teste de LEDs");
    //rfidControl.sendMessageToRFIDQueue("Iniciar leitura RFID");
    //rfidControl.setReadMode(CONTINUOUS_READ);  // Alterna para leitura contínua
    //rfidControl.setReadInterval(3000);         // Define o intervalo de leitura contínua para 3 segundos
    //rfidControl.setReadMode(READ_DISABLED);      // Configura para leitura única
    //rfidControl.setReadMode(SINGLE_READ);      // Configura para leitura única
    //rfidControl.sendMessageToRFIDQueue("Iniciar leitura única RFID");
    //lcdControl.sendMessageToLCD(1, "Hello, LCD1!", 0);
    //lcdControl.sendMessageToLCD(2, "Hello, LCD2!", 0);
    serialComm.sendMessage("Hello from SerialCommunication!");
    //webServer.sendMessageToWebServer("Mensagem inicial para o servidor!");
}

void loop()
{
    // O loop principal não será usado, pois as tarefas são gerenciadas pelo FreeRTOS
    // Exemplo de log periódico
    LOG_INFO(&Serial, "Loop principal em execução.");

    delay(10000);
}
