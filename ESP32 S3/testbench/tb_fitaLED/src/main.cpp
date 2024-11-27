#include <Arduino.h>

#include "appl_MyLogger.h"

#include <FastLED.h>

// https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf

#define NUM_LEDS 576      // Número total de LEDs
#define DATA_PIN 5        // Pino de dados conectado ao DI dos LEDs
#define BRIGHTNESS  64    // Brilho máximo (0-255)

CRGB leds[NUM_LEDS];

void rainbowCycle(uint8_t wait);

CRGB Wheel(byte WheelPos);

// Função para converter posição (ex: "E4") em índices de coluna e linha
bool parsePosition(const char* pos, int &file, int &rank) {
  if (pos[0] >= 'A' && pos[0] <= 'H') {
    file = pos[0] - 'A';
  } else if (pos[0] >= 'a' && pos[0] <= 'h') {
    file = pos[0] - 'a';
  } else {
    return false;
  }
  
  if (pos[1] >= '1' && pos[1] <= '8') {
    rank = pos[1] - '1';
  } else {
    return false;
  }
  
  return true;
}

// Função para obter os índices dos LEDs correspondentes a uma posição
void getLEDIndices(int file, int rank, int ledIndices[9]) {
  int index = 0;
  for (int layer = 0; layer < 3; layer++) {
    int strip = rank * 3 + layer;
    int strip_base = strip * 24;
    bool direction = (strip % 2 == 0); // true para esquerda->direita
    int p = direction ? file : 7 - file;
    for (int k = 0; k < 3; k++) {
      int led_in_strip = p * 3 + k;
      int led_index = strip_base + led_in_strip;
      ledIndices[index++] = led_index;
    }
  }
}

// Função para definir a cor de uma posição
void setPositionColor(const char* pos, CRGB color) {
  int file, rank;
  if (!parsePosition(pos, file, rank)) {
    Serial.println("Posição inválida");
    return;
  }
  Serial.println(String("Pos:") + String(pos) + String(", Linha:") + String(file) + String(", Coluna:") + String(rank));
  
  int ledIndices[9];
  getLEDIndices(file, rank, ledIndices);
  Serial.print("Indices LEDs: ");
  for (int i = 0; i < 9; i++) {
    leds[ledIndices[i]] = color;
    Serial.print(String(ledIndices[i]) + String(", "));
  }
  Serial.println("");
  FastLED.show();
}

// Função para limpar uma posição
void clearPosition(const char* pos) {
  setPositionColor(pos, CRGB::Black);
}

void setup() {
  Serial.begin(115200);
  // Inicializa a biblioteca FastLED
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();
}

void loop() {
  /*static int stateMachine = 0;
  
  if (stateMachine == 0) {
    // Exemplo 1: Acender todos os LEDs na cor vermelha
    fill_solid(leds, NUM_LEDS, CRGB::Red);
  }
  else if (stateMachine == 1) {
    // Exemplo 2: Acender todos os LEDs na cor vermelha
    fill_solid(leds, NUM_LEDS, CRGB::Green);
  }
  else if (stateMachine == 2) {
    // Exemplo 2: Acender todos os LEDs na cor vermelha
    fill_solid(leds, NUM_LEDS, CRGB::Blue);
  }

  stateMachine++;
  if (stateMachine > 2)
    stateMachine = 0;

  FastLED.show();
  delay(1000);

  // Exemplo 2: Apagar todos os LEDs
  FastLED.clear();
  FastLED.show();
  delay(1000);

  // Exemplo 3: Animação arco-íris
  rainbowCycle(10);  // Velocidade da animação*/
  // Exemplo: Acender a posição E4 em vermelho
  setPositionColor("A1", CRGB::Red);
  delay(1000);
  setPositionColor("B1", CRGB::Red);
  delay(1000);
  setPositionColor("C1", CRGB::Red);
  delay(1000);
  setPositionColor("D1", CRGB::Red);
  delay(1000);

  // Apagar a posição E4
  clearPosition("A1");
  delay(1000);
  clearPosition("B1");
  delay(1000);
  clearPosition("C1");
  delay(1000);
  clearPosition("D1");
  delay(1000);

  setPositionColor("E4", CRGB::Red);
  delay(1000);

  // Acender múltiplas posições
  setPositionColor("D2", CRGB::Blue);
  setPositionColor("G5", CRGB::Green);
  delay(1000);

  setPositionColor("C5", CRGB::Green);
  delay(1000);

  setPositionColor("F8", CRGB::Green);
  delay(1000);

  setPositionColor("G8", CRGB::Green);
  delay(1000);

  setPositionColor("H8", CRGB::Green);
  delay(1000);

  // Apagar todas as posições
  FastLED.clear();
  FastLED.show();
  delay(1000);
}

// Função para animação arco-íris
void rainbowCycle(uint8_t wait) {
  static uint16_t j = 0;
  for(int i=0; i<NUM_LEDS; i++) {
    leds[i] = Wheel((i + j) & 255);
  }
  FastLED.show();
  delay(wait);
  j++;
}

// Função auxiliar para cores do arco-íris
CRGB Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return CRGB(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
    return CRGB(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
    WheelPos -= 170;
    return CRGB(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}
