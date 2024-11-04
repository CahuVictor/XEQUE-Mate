#ifndef __WiFiManager_h__
#define __WiFiManager_h__

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include "appl_MyLogger.h"

#define WIFI_SSID "your_default_ssid"        // SSID padrão para conexão inicial
#define WIFI_PASSWORD "your_default_password"// Senha padrão
#define AP_SSID "ESP32_Config"               // Nome do ponto de acesso
#define AP_PASSWORD "12345678"               // Senha do ponto de acesso (mínimo 8 caracteres)
#define CAPTIVE_PORTAL_PORT 80               // Porta do servidor captive portal
#define DNS_PORT 53                          // Porta do servidor DNS
#define QUEUE_MESSAGE_SIZE 64

class WiFiManager {
public:
    WiFiManager();
    void initialize();
    void startWiFiTask();                    // Inicia a tarefa FreeRTOS para gerenciar WiFi
    void setSerialQueue(QueueHandle_t queue);  // Define a fila da comunicação serial

    TaskHandle_t getWiFiTaskHandle() const { return wifiTaskHandle; }

private:
    AsyncWebServer server;
    DNSServer dnsServer;
    SemaphoreHandle_t wifiSemaphore;
    QueueHandle_t serialQueue;                // Ponteiro para a fila da comunicação serial
    bool apModeActive;

    void connectToWiFi();
    void startAPMode();
    void setupCaptivePortal();
    void handleRootRequest(AsyncWebServerRequest* request);
    void handleWiFiConfig(AsyncWebServerRequest* request);
    void monitorWiFiTask();
    void processSerialCommands();              // Processa comandos da fila serial
    void printIPAddress();                     // Função para imprimir o IP atual

    TaskHandle_t wifiTaskHandle;  // Adiciona o handle da tarefa WiFi para receber notificações

    static void taskWrapper(void* _this) {
        static_cast<WiFiManager*>(_this)->monitorWiFiTask();
    }
};

#endif
