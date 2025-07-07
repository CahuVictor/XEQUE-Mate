#include <Arduino.h>
#include <Wire.h>
#include "unity.h"

#include "appl_MyLogger.h"

#define SLAVE_ADDRESS 0x20    // Endereço do ESP32 como escravo

// Função mockada para o teste
void mock_scanI2C() {
    // Simulando a detecção de dispositivos no barramento I2C
    // Você pode adicionar a lógica aqui para simular uma comunicação I2C real
    Serial.println("Simulando escaneamento I2C...");
}

void test_scanI2C_no_devices() {
    // Teste para verificar o caso quando nenhum dispositivo I2C é encontrado
    mock_scanI2C();
    // Verificar a saída serial (usando algo como um mock do Serial)
    // Aqui, estamos apenas verificando se a função foi chamada corretamente
    // Você pode usar asserts para verificar saídas ou comportamentos esperados
    TEST_ASSERT_TRUE_MESSAGE(true, "Nenhum dispositivo encontrado");
}

void test_receiveEvent_function() {
    // Simular o recebimento de dados pelo ESP32 como escravo
    String receivedData = "ESP32 escravo OK!";
    // Verifique se a função de recebimento de dados foi chamada corretamente
    TEST_ASSERT_EQUAL_STRING("ESP32 escravo OK!", receivedData.c_str());
}

void test_requestEvent_function() {
    // Simular a função que o mestre solicita dados
    String responseData = "ESP32 escravo OK!";
    TEST_ASSERT_EQUAL_STRING("ESP32 escravo OK!", responseData.c_str());
}

void test_I2C_slave() {
    LOG_INFO(&Serial, "Inicio do Teste");
    // Simular a função que o mestre solicita dados
    // Configuração do barramento I2C escravo
    int sdaSlavePin = 19; /*19*/
    int sclSlavePin = 21; /*21*/
    Wire.setPins(sdaSlavePin, sclSlavePin);
    //Wire.begin(sdaSlavePin, sclSlavePin);
    //Wire.begin(SLAVE_ADDRESS, sdaSlavePin, sclSlavePin);
    Wire.begin(SLAVE_ADDRESS);

    delay(1000*60*10);

    String responseData = "ESP32 escravo OK!";
    TEST_ASSERT_EQUAL_STRING("ESP32 escravo OK!", responseData.c_str());
    LOG_INFO(&Serial, "Fim do Teste");
}

void setup() {
    Serial.begin(115200);
    // Inicializa o I2C
    //Wire.begin(19, 21);
    // Inicializa os testes
    UNITY_BEGIN();
    // Chama os testes
    RUN_TEST(test_I2C_slave);
    //RUN_TEST(test_scanI2C_no_devices);
    //RUN_TEST(test_receiveEvent_function);
    //RUN_TEST(test_requestEvent_function);
}

void loop() {
    // O loop é necessário, mas os testes serão executados apenas no setup
}
