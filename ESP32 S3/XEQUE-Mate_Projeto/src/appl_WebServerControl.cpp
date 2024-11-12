#include "appl_WebServerControl.h"

const char text_webservercontrol[] =  "WebServerControl::";

WebServerControl::WebServerControl(uint16_t port) 
    : server(port), SendQueue(nullptr), ReceiveQueue(nullptr) {}

void WebServerControl::initialize() {
    // Configura as rotas HTTP do servidor
    setupRoutes();
    this->server.begin();
    LOG_DEBUG(&Serial, (String(text_webservercontrol) + String("Servidor web iniciado.")).c_str());
}

void WebServerControl::setupRoutes() {
    // Rota raiz
    this->server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
        this->handleRootRequest(request);
    });

    // Rota de status (exemplo)
    this->server.on("/status", HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Servidor Web em execução!");
    });
}

void WebServerControl::handleRootRequest(AsyncWebServerRequest *request) {
    // Responde com uma mensagem simples
    request->send(200, "text/plain", "Bem-vindo ao Servidor Web!");
}

void WebServerControl::setQueue(QueueHandle_t SendQueue, QueueHandle_t ReceiveQueue) {
    this->SendQueue = SendQueue;
    this->ReceiveQueue = ReceiveQueue;
}

bool WebServerControl::sendMessageToQueueWebServer(const char* message) {
    if (this->SendQueue != nullptr) {
        if (xQueueSend(this->SendQueue, message, portMAX_DELAY) == pdPASS) {
            return true;
        } else {
            LOG_WARNING(&Serial, (String(text_webservercontrol) + String("Falha ao enviar mensagem para a fila do servidor.")).c_str());
        }
    }
    return false;
}

void WebServerControl::startTask() {
    xTaskCreate(this->taskWrapper, "WebServer Task", 4096, this, 1, NULL);
}

void WebServerControl::monitorTask() {
    for (;;) {
        this->processCommand();

        vTaskDelay(pdMS_TO_TICKS(100)); // Pequeno delay para liberar a CPU
    }
}

void WebServerControl::processCommand() {
    if (this->ReceiveQueue != nullptr) 
    {
        //LOG_DEBUG(&Serial, ("WiFiManager: ReceiveQueue != nullptr"));
        char command[QUEUE_MESSAGE_SIZE];
        if (xQueueReceive((this->ReceiveQueue), &command, pdMS_TO_TICKS(100)/*portMAX_DELAY*/)== pdPASS)
        {
            LOG_DEBUG(&Serial, (String(text_webservercontrol) + String("Processando mensagem recebida.")).c_str());
            // Lida com a mensagem recebida e armazena informações para resposta na rota
            char logMessage[128];
            snprintf(logMessage, sizeof(logMessage), "Mensagem recebida: %s", command);
            //LOG_DEBUG(&Serial, logMessage);
            LOG_DEBUG(&Serial, (String(text_webservercontrol) + String(logMessage)).c_str());

            // Aqui, podemos atualizar variáveis ou respostas do servidor com a mensagem

            // Adicionar dados a fila de envio do servidor
            char message[]="WebServer:Dado Recebido, Processado e Respondido";
            this->sendMessageToQueueWebServer(message);
        }
    } else {
        //LOG_DEBUG(&Serial, (String(text_webservercontrol) + String("ReceiveQueue == nullptr")).c_str());
    }
}
