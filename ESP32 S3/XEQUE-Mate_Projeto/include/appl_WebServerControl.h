#ifndef __appl_WebServerControl_h__
#define __appl_WebServerControl_h__

#include "Arduino.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <ESPAsyncWebServer.h>
#include "appl_MyLogger.h"

#define QUEUE_MESSAGE_SIZE 64  // Tamanho máximo de uma mensagem na fila

class WebServerControl {
public:
    WebServerControl(uint16_t port = 80);    // Construtor que define a porta do servidor
    void initialize();                        // Inicializa o servidor web e configura as rotas

    void setQueue(QueueHandle_t SendQueue, QueueHandle_t ReceiveQueue);     // Define a fila de envio e recebimento de dados
    void startTask();                         // Inicia a tarefa FreeRTOS para monitorar a fila

private:
    AsyncWebServer server;                    // Instância do servidor web

    void setupRoutes();                       // Configura as rotas HTTP do servidor web
    void handleRootRequest(AsyncWebServerRequest *request); // Função para responder à rota raiz

    QueueHandle_t SendQueue;                    // Ponteiro para a fila de envio de dados
    QueueHandle_t ReceiveQueue;                 // Ponteiro para a fila de recebimento de dados

    bool sendMessageToQueueWebServer(const char* message); // Envia mensagem para a fila do servidor web
    void monitorTask();                     // Tarefa para monitorar a fila de mensagens
    void processCommand();                     // Processa comandos recebidos na fila

    static void taskWrapper(void* _this) {
        static_cast<WebServerControl*>(_this)->monitorTask();
    }
};

#endif
