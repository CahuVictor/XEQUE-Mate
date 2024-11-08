#include "appl_WebServerControl.h"

WebServerControl::WebServerControl(uint16_t port) 
    : server(port), webServerQueue(nullptr) {}

void WebServerControl::initialize() {
    // Inicializa a fila para mensagens de controle do servidor web
    webServerQueue = xQueueCreate(10, sizeof(char) * QUEUE_MESSAGE_SIZE);
    if (webServerQueue == nullptr) {
        LOG_ERROR(&Serial, "WebServerControl: Falha ao criar a fila de mensagens.");
    } else {
        LOG_INFO(&Serial, "WebServerControl: Fila de mensagens criada com sucesso.");
    }

    // Configura as rotas HTTP do servidor
    setupRoutes();
    server.begin();
    LOG_INFO(&Serial, "WebServerControl: Servidor web iniciado.");
}

void WebServerControl::startTask() {
    xTaskCreate(taskWrapper, "WebServer Task", 4096, this, 1, NULL);
}

bool WebServerControl::sendMessageToWebServer(const char* message) {
    if (webServerQueue != nullptr) {
        if (xQueueSend(webServerQueue, message, portMAX_DELAY) == pdPASS) {
            return true;
        } else {
            LOG_WARNING(&Serial, "WebServerControl: Falha ao enviar mensagem para a fila do servidor.");
        }
    }
    return false;
}

void WebServerControl::webServerTask() {
    char message[QUEUE_MESSAGE_SIZE];
    for (;;) {
        if (xQueueReceive(webServerQueue, &message, portMAX_DELAY) == pdPASS) {
            LOG_INFO(&Serial, "WebServerControl: Processando mensagem recebida.");
            processMessage(message);
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Pequeno delay para liberar a CPU
    }
}

void WebServerControl::setupRoutes() {
    // Rota raiz
    server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
        handleRootRequest(request);
    });

    // Rota de status (exemplo)
    server.on("/status", HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Servidor Web em execução!");
    });
}

void WebServerControl::handleRootRequest(AsyncWebServerRequest *request) {
    // Responde com uma mensagem simples
    request->send(200, "text/plain", "Bem-vindo ao Servidor Web!");
}

void WebServerControl::processMessage(const char* message) {
    // Lida com a mensagem recebida e armazena informações para resposta na rota
    char logMessage[128];
    snprintf(logMessage, sizeof(logMessage), "Mensagem recebida: %s", message);
    LOG_INFO(&Serial, logMessage);

    // Aqui, podemos atualizar variáveis ou respostas do servidor com a mensagem
}
