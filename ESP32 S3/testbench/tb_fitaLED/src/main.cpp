#include <Arduino.h>

#include "appl_MyLogger.h"

#include <FastLED.h>

// https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf

#define NUM_LEDS 5      // Número total de LEDs
#define DATA_PIN 5        // Pino de dados conectado ao DI dos LEDs
#define BRIGHTNESS  64    // Brilho máximo (0-255)

CRGB leds[NUM_LEDS];

void rainbowCycle(uint8_t wait);

CRGB Wheel(byte WheelPos);

void setup() {
  // Inicializa a biblioteca FastLED
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();
}

void loop() {
  static int stateMachine = 0;
  
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
  rainbowCycle(10);  // Velocidade da animação
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
