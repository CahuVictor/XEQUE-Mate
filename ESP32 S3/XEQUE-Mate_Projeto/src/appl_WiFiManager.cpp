#include "appl_WiFiManager.h"

WiFiManager::WiFiManager() 
    : server(CAPTIVE_PORTAL_PORT), apModeActive(false), wifiSemaphore(nullptr), serialQueue(nullptr) {}

void WiFiManager::initialize() {
    wifiSemaphore = xSemaphoreCreateBinary();
    connectToWiFi();
}

void WiFiManager::startWiFiTask() {
    xTaskCreate(taskWrapper, "WiFi Task", 4096, this, 1, NULL);
}

void WiFiManager::setSerialQueue(QueueHandle_t queue) {
    serialQueue = queue;
}

void WiFiManager::connectToWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    LOG_INFO(&Serial, "Tentando conectar ao WiFi...");

    if (WiFi.waitForConnectResult() == WL_CONNECTED) {
        printIPAddress(); // Imprime o IP após conexão //LOG_INFO(&Serial, (String("Conectado ao WiFi. IP: ") + WiFi.localIP().toString()).c_str());
    } else {
        LOG_ERROR(&Serial, "Falha na conexão com o WiFi.");
        startAPMode();
    }
}

void WiFiManager::startAPMode() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

    setupCaptivePortal();

    LOG_INFO(&Serial, "Modo AP ativado. Conecte-se ao ponto de acesso para configurar o WiFi.");
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
        Serial.printf("Conectando a SSID: %s\n", ssid.c_str());

        if (WiFi.waitForConnectResult() == WL_CONNECTED) {
            LOG_INFO(&Serial, "Conectado com sucesso!");
            apModeActive = false;
            dnsServer.stop();
            server.end();

            printIPAddress(); // Imprime o IP após configuração
        } else {
            LOG_INFO(&Serial, "Falha na conexão. Tente novamente.");
        }
    }
    request->send(200, "text/html", "Configuração recebida! Tentando conectar...");
}

void WiFiManager::monitorWiFiTask() {
    for (;;) {
        if (WiFi.status() != WL_CONNECTED && !apModeActive) {
            LOG_INFO(&Serial, "WiFi desconectado. Ativando modo AP...");
            startAPMode();
        }

        // Processa comandos recebidos pela Serial
        processSerialCommands();

        dnsServer.processNextRequest();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void WiFiManager::processSerialCommands() {
    static String lastCommand = "";

    if (serialQueue != nullptr) {
        char command[QUEUE_MESSAGE_SIZE];
        if (xQueueReceive(serialQueue, &command, 0) == pdPASS) {
            // Verifica se o comando atual é diferente do último processado
            if (lastCommand != command) {
                lastCommand = command;  // Atualiza o último comando processado

                LOG_INFO(&Serial, (String("Comando recebido via Serial: ") + command).c_str());

                if (strncmp(command, "SSID:", 5) == 0) {
                    String ssid = String(command + 5);
                    LOG_INFO(&Serial, (String("Recebido SSID via Serial: ") + ssid).c_str());
                    WiFi.begin(ssid.c_str(), WiFi.psk().c_str());

                } else if (strncmp(command, "PASSWORD:", 9) == 0) {
                    String password = String(command + 9);
                    LOG_INFO(&Serial, "Senha WiFi recebida via Serial.");
                    WiFi.begin(WiFi.SSID().c_str(), password.c_str());

                } else if (strcmp(command, "GET IP") == 0) {
                    printIPAddress();
                }
            }
        }
    }
}
void WiFiManager::printIPAddress() {
    if (WiFi.status() == WL_CONNECTED) {
        LOG_INFO(&Serial, (String("IP conectado: ") + WiFi.localIP().toString()).c_str());
    } else if (WiFi.getMode() == WIFI_AP) {
        LOG_INFO(&Serial, (String("IP modo AP: ") + WiFi.softAPIP().toString()).c_str());
    } else {
        LOG_INFO(&Serial, "Nenhuma conexão WiFi ativa.");
    }
}