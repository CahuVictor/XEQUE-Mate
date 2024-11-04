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

    // Inicialização dos módulos
    //ledControl.initialize();
    //rfidControl.initialize();
    //buttonControl.initialize();
    //lcdControl.initialize();
    serialComm.initialize();
    serialQueue = xQueueCreate(10, sizeof(char) * QUEUE_MESSAGE_SIZE);
    wifiManager.setSerialQueue(serialQueue);    // Passa a fila para WiFiManager
    wifiManager.initialize();
    //webServer.initialize();
        /*i2cComm.initialize();
        powerMonitor.initialize();
        supervisor.initialize();
        stateMachine.initialize();*/

    // Inicia tarefas FreeRTOS para cada módulo (exemplo)
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

void loop() {
    // O loop principal não será usado, pois as tarefas são gerenciadas pelo FreeRTOS
    // Exemplo de log periódico
    LOG_INFO(&Serial, "Loop principal em execução.");

    delay(10000);
}
