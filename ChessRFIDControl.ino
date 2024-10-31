#include <SPI.h>
#include <MFRC522.h>
#include "MuxControl.h"

#define NUM_READERS 64

// Pinos de controle dos MUX para os pinos SS
const byte muxS0 = 2;
const byte muxS1 = 3;
const byte muxS2 = 4;
const byte muxS3 = 5;

// Instâncias dos MUX (um para cada MUX de 16 canais)
MuxControl muxSS1(muxS0, muxS1, muxS2, muxS3);
// Repita para os outros MUX (muxSS2, muxSS3, muxSS4)

// Pinos SPI
#define RST_PIN         9   // Pino RST compartilhado
#define MOSI_PIN        11
#define MISO_PIN        12
#define SCK_PIN         13

// O pino SS é controlado pelo MUX
#define SS_PIN          10  // Pino SS do Arduino conectado aos MUX

MFRC522 mfrc522(SS_PIN, RST_PIN); // Instância do MFRC522

void setup() {
    Serial.begin(9600);
    while (!Serial);

    SPI.begin();        // Inicia o barramento SPI
    mfrc522.PCD_Init(); // Inicializa o leitor RFID

    // Inicializa os MUX
    // Repita para outros MUX se necessário
}

void loop() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        processCommand(command);
    }
}

void processCommand(String command) {
    command.trim();
    if (command.startsWith("read")) {
        int readerId = command.substring(5).toInt();
        readRFID(readerId);
    } else if (command.startsWith("write")) {
        int readerId = command.substring(6).toInt();
        String data = command.substring(command.indexOf(' ', 6) + 1);
        writeRFID(readerId, data);
    } else {
        Serial.println("Comando inválido. Use 'read <ID>' ou 'write <ID> <dados>'.");
    }
}

void selectReader(int readerId) {
    if (readerId < 0 || readerId >= NUM_READERS) {
        Serial.println("ID do leitor inválido");
        return;
    }
    // Seleciona o canal no MUX correspondente ao leitor
    // Supondo que estamos usando 4 MUX para os pinos SS

    byte muxIndex = readerId / 16;     // Índice do MUX (0 a 3)
    byte channel = readerId % 16;      // Canal dentro do MUX (0 a 15)

    switch (muxIndex) {
        case 0:
            muxSS1.selectChannel(channel);
            // Desabilitar os outros MUX
            // Exemplo: muxSS2.selectChannel(255); // Canal inválido para desabilitar
            break;
        case 1:
            // muxSS2.selectChannel(channel);
            // Desabilitar os outros MUX
            break;
        case 2:
            // muxSS3.selectChannel(channel);
            break;
        case 3:
            // muxSS4.selectChannel(channel);
            break;
    }
}

void readRFID(int readerId) {
    selectReader(readerId);

    // Reinicializa o MFRC522 se necessário
    mfrc522.PCD_Init();

    // Verifica se há um novo cartão presente
    if (!mfrc522.PICC_IsNewCardPresent()) {
        Serial.println("Nenhum cartão detectado");
        return;
    }

    // Tenta ler o UID do cartão
    if (!mfrc522.PICC_ReadCardSerial()) {
        Serial.println("Falha ao ler o cartão");
        return;
    }

    // Exibe o UID do cartão
    Serial.print("UID do cartão: ");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        Serial.print(" ");
    }
    Serial.println();

    // Finaliza a comunicação com o cartão
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}

void writeRFID(int readerId, String data) {
    selectReader(readerId);

    // Reinicializa o MFRC522 se necessário
    mfrc522.PCD_Init();

    // Autenticação e escrita dos dados
    // Implementar a lógica de escrita conforme necessário
    // Exemplo: escrever 'data' no bloco desejado do cartão

    // Lembre-se de que a escrita em cartões MIFARE Classic requer autenticação com a chave correta
}
