#include "appl_MyLogger.h"

#ifdef CONFIG_FREERTOS
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
SemaphoreHandle_t MyLogger::logMutex = nullptr;  // Inicializa o mutex como nullptr
#endif

void MyLogger::initialize() {
    #ifdef CONFIG_FREERTOS
    // Cria o mutex para controle de acesso
    if (logMutex == nullptr) {
        logMutex = xSemaphoreCreateMutex();
    }
    #endif
}

void MyLogger::logMessageFunc(Print* interface, int level, const char* message, const char* func, int line) {
    #ifdef CONFIG_FREERTOS
    // Apenas usa o mutex se FreeRTOS estiver disponível
    if (logMutex != nullptr && xSemaphoreTake(logMutex, portMAX_DELAY) == pdTRUE) {
        char log[256];
        snprintf(log, sizeof(log), "[%lu ms] | %s | %s:%d | %s",
                 millis(),
                 (level == __DEBUG) ? "DEBUG" : (level == __INFO) ? "INFO" : (level == __WARNING) ? "WARNING" : "ERROR",
                 func,
                 line,
                 message);

        logToInterface(interface, log);

        xSemaphoreGive(logMutex);  // Libera o mutex após a escrita
    }
    #else
    // Sem FreeRTOS, simplesmente executa o log sem o mutex
    char log[256];
    snprintf(log, sizeof(log), "[%lu ms] | %s | %s:%d | %s",
             millis(),
             (level == __DEBUG) ? "DEBUG" : (level == __INFO) ? "INFO" : (level == __WARNING) ? "WARNING" : "ERROR",
             func,
             line,
             message);

    logToInterface(interface, log);
    #endif
}

void MyLogger::logToInterface(Print* interface, const char* log) {
    if (interface != nullptr) {
        interface->println(log);
    }
}
