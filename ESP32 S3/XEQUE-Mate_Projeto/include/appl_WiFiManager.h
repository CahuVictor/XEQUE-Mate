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
    void setQueue(QueueHandle_t SendQueue, QueueHandle_t ReceiveQueue);     // Define a fila de envio e recebimento de dados
    void startTask();                    // Inicia a tarefa FreeRTOS para gerenciar WiFi

    //TaskHandle_t getWiFiTaskHandle() const { return wifiTaskHandle; }

private:
    AsyncWebServer server;
    DNSServer dnsServer;
    //SemaphoreHandle_t wifiSemaphore;
    QueueHandle_t SendQueue;                    // Ponteiro para a fila de envio de dados
    QueueHandle_t ReceiveQueue;                 // Ponteiro para a fila de recebimento de dados
    bool apModeActive;

    void monitorWiFiTask();
    void processReceiveCommand();                     // Processa comandos recebidos na fila

    void connectToWiFi();
    void startAPMode();
    void setupCaptivePortal();
    void handleRootRequest(AsyncWebServerRequest* request);
    void handleWiFiConfig(AsyncWebServerRequest* request);

    void printIPAddress();                     // Função para imprimir o IP atual

    //TaskHandle_t wifiTaskHandle;  // Adiciona o handle da tarefa WiFi para receber notificações

    static void taskWrapper(void* param) {
        static_cast<WiFiManager*>(param)->monitorWiFiTask();
    }
};

#endif
