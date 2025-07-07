#include <Arduino.h>
#include <Wire.h>

#define NUM_PCF8574 8

// Lista dos endereços esperados dos dispositivos PCF8574
uint8_t pcf8574Addresses[NUM_PCF8574] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};

// Array para armazenar quais dispositivos foram encontrados
bool devicesFound[NUM_PCF8574];

#define PCF8574_ADDRESS 0x20  // Ajuste o endereço conforme necessário

void scanI2C() {
  byte error, address;
  int nDevices;

  Serial.println("Iniciando escaneamento I2C...");
  nDevices = 0;

  //for (address = 1; address < 127; address++) {
  for (int i = 0; i < NUM_PCF8574; i++) {
    address = pcf8574Addresses[i];
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Dispositivo I2C encontrado no endereço 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" !");
      nDevices++;
      devicesFound[i] = true;
    } else /*if (error == 4) {
      Serial.print("Erro desconhecido no endereço 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }*/
      devicesFound[i] = false;
  }

  if (nDevices == 0)
    //Serial.println("Nenhum dispositivo I2C encontrado\n");
    Serial.println("Nenhum dispositivo PCF8574 encontrado\n");
  else
    Serial.println("Escaneamento concluído\n");
}

void setup() {
  Serial.begin(115200);
  while (!Serial);  // Aguarda a inicialização do monitor serial
  
  // Substitua 15 e 16 pelos números dos pinos que você deseja usar
  int sdaPin = 15;
  int sclPin = 16;

  Wire.begin(sdaPin, sclPin);

  scanI2C();  // Realiza o escaneamento I2C
}

void loop() {
  /*for (int pin = 0; pin < 8; pin++) {
    uint8_t data = 0x00;  // Inicia com todos os pinos em LOW

    data |= (1 << pin);   // Define o pino atual para HIGH

    Wire.beginTransmission(PCF8574_ADDRESS);
    Wire.write(data);
    Wire.endTransmission();

    Serial.print("Pino P");
    Serial.print(pin);
    Serial.println(" em HIGH");
    delay(1000);  // Aguarda 1 segundo

    // Opcional: Coloca o pino de volta em LOW antes de passar para o próximo
    /*
    data &= ~(1 << pin);  // Define o pino atual para LOW
    Wire.beginTransmission(PCF8574_ADDRESS);
    Wire.write(data);
    Wire.endTransmission();
    */
  //}*/

  for (int i = 0; i < NUM_PCF8574; i++) {
    if (devicesFound[i]) {
      uint8_t address = pcf8574Addresses[i];
      Serial.print("Controlando PCF8574 no endereço 0x");
      Serial.println(address, HEX);

      for (int pin = 0; pin < 8; pin++) {
        uint8_t data = 0x00;  // Inicia com todos os pinos em LOW
        data |= (1 << pin);   // Define o pino atual para HIGH

        Wire.beginTransmission(address);
        Wire.write(data);
        Wire.endTransmission();

        Serial.print("PCF8574 (0x");
        Serial.print(address, HEX);
        Serial.print(") - Pino P");
        Serial.print(pin);
        Serial.println(" em HIGH");

        delay(500);

        // Opcional: Colocar o pino de volta em LOW
        data &= ~(1 << pin);  // Define o pino atual para LOW
        Wire.beginTransmission(address);
        Wire.write(data);
        Wire.endTransmission();

        delay(500);
      }
    }
  }
}
