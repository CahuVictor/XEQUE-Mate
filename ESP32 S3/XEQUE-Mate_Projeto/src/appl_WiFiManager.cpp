#include "appl_WiFiManager.h"

WiFiManager::WiFiManager() 
    : server(CAPTIVE_PORTAL_PORT), apModeActive(false), /*wifiSemaphore(nullptr),*/ SendQueue(nullptr), ReceiveQueue(nullptr) {}

void WiFiManager::initialize() {
    //wifiSemaphore = xSemaphoreCreateBinary();
    connectToWiFi();
}

void WiFiManager::connectToWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    LOG_INFO(&Serial, "WiFiManager: Tentando conectar ao WiFi...");

    if (WiFi.waitForConnectResult() == WL_CONNECTED) {
        printIPAddress(); // Imprime o IP após conexão //LOG_INFO(&Serial, (String("Conectado ao WiFi. IP: ") + WiFi.localIP().toString()).c_str());
    } else {
        LOG_ERROR(&Serial, "WiFiManager: Falha na conexão com o WiFi.");
        startAPMode();
    }
}

void WiFiManager::startAPMode() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

    setupCaptivePortal();

    LOG_INFO(&Serial, "WiFiManager: Modo AP ativado. Conecte-se ao ponto de acesso para configurar o WiFi.");
    apModeActive = true;

    printIPAddress(); // Imprime o IP no modo AP //LOG_INFO(&Serial, (String("Modo AP criado. IP: ") + WiFi.softAPIP().toString()).c_str());
}

void WiFiManager::setupCaptivePortal() {
    server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
        handleRootRequest(request);
    });

    server.on("/wifi_config", HTTP_POST, [this](AsyncWebServerRequest *request) {
        handleWiFiConfig(request);
    });

    server.begin();
}

void WiFiManager::handleRootRequest(AsyncWebServerRequest *request) {
    String html = "<html><body><h1>Configuração WiFi</h1>";
    html += "<form action='/wifi_config' method='POST'>";
    html += "<label for='ssid'>SSID:</label><input type='text' name='ssid'><br>";
    html += "<label for='password'>Senha:</label><input type='password' name='password'><br>";
    html += "<input type='submit' value='Conectar'></form></body></html>";
    request->send(200, "text/html", html);
}

void WiFiManager::handleWiFiConfig(AsyncWebServerRequest *request) {
    if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
        String ssid = request->getParam("ssid", true)->value();
        String password = request->getParam("password", true)->value();

        WiFi.begin(ssid.c_str(), password.c_str());
        Serial.printf("WiFiManager: Conectando a SSID: %s\n", ssid.c_str());

        if (WiFi.waitForConnectResult() == WL_CONNECTED) {
            LOG_INFO(&Serial, "WiFiManager: Conectado com sucesso!");
            apModeActive = false;
            dnsServer.stop();
            server.end();

            printIPAddress(); // Imprime o IP após configuração
        } else {
            LOG_INFO(&Serial, "WiFiManager: Falha na conexão. Tente novamente.");
        }
    }
    request->send(200, "text/html", "Configuração recebida! Tentando conectar...");
}

void WiFiManager::printIPAddress() {
    if (WiFi.status() == WL_CONNECTED) {
        LOG_INFO(&Serial, (String("WiFiManager: IP conectado: ") + WiFi.localIP().toString()).c_str());
    } else if (WiFi.getMode() == WIFI_AP) {
        LOG_INFO(&Serial, (String("WiFiManager: IP modo AP: ") + WiFi.softAPIP().toString()).c_str());
    } else {
        LOG_WARNING(&Serial, "WiFiManager: Nenhuma conexão WiFi ativa.");
    }
}

void WiFiManager::setQueue(QueueHandle_t SendQueue, QueueHandle_t ReceiveQueue) {
    this->SendQueue = SendQueue;
    this->ReceiveQueue = ReceiveQueue;
}

void WiFiManager::startTask() {
    xTaskCreate(taskWrapper, "WiFi Task",                                   // A name just for humans
                4096,                                                       // The stack size can be checked by calling `uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);`
                this,                                                       // 
                1,                                                          // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
                NULL/*&wifiTaskHandle*/);                                           // Salva o handle da tarefa
}

void WiFiManager::monitorTask() {
    for (;;) {
        if (WiFi.status() != WL_CONNECTED && !apModeActive) {
            LOG_INFO(&Serial, "WiFiManager: WiFi desconectado. Ativando modo AP...");
            startAPMode();
        }
        
        processCommand();  // Processa comandos recebidos

        dnsServer.processNextRequest();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void WiFiManager::processCommand() {
    //static String lastCommand = "";
    //LOG_DEBUG(&Serial, "WiFiManager: Verificando ReceiveQueue...");

    if (this->ReceiveQueue != nullptr) {
        //LOG_DEBUG(&Serial, ("WiFiManager: ReceiveQueue != nullptr"));
        char command[QUEUE_MESSAGE_SIZE];
        if (xQueueReceive((this->ReceiveQueue), &command, pdMS_TO_TICKS(100))== pdPASS)
        {
            LOG_DEBUG(&Serial, (String("WiFiManager: Comando recebido: ") + String(command)).c_str());
            if (strncmp(command, "SSID:", 5) == 0)
            {
                // Tratamento do comando SSID
                String ssid = String(command + 5);
                LOG_INFO(&Serial, (String("WiFiManager: Recebido SSID via ReceiveQueue: ") + ssid).c_str());
                WiFi.begin(ssid.c_str(), WiFi.psk().c_str());

            } else if (strncmp(command, "PASSWORD:", 9) == 0)
            {
                // Tratamento do comando PASSWORD
                String password = String(command + 9);
                LOG_INFO(&Serial, "WiFiManager: Senha WiFi recebida via ReceiveQueue.");
                WiFi.begin(WiFi.SSID().c_str(), password.c_str());

            } else if (strncmp(command, "GET IP", 6) == 0)
            {
                printIPAddress();
            } else
            {
                LOG_DEBUG(&Serial, "WiFiManager: Comando recebido desconhecido.");
            }
        } /*else
        {
            LOG_DEBUG(&Serial, ("WiFiManager: Falhou ao tentar coletar o comando da ReceiveQueue"));
        }*/

    } else {
        //LOG_DEBUG(&Serial, ("WiFiManager: ReceiveQueue == nullptr"));
    }
}