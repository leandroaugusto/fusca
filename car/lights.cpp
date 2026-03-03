#include "lights.h"
#include "vehicle_lights.h"
#include "config.h"
#include <Arduino.h>

// =============================
// CONFIG PWM (ESP32-C3 Core 3.x)
// =============================
#define TAIL_PWM_FREQ  5000
#define TAIL_PWM_RES   8   // 0-255

#define TAIL_DIM_LEVEL    60
#define TAIL_BRAKE_LEVEL  220

// =============================
// Controle de pisca
// =============================
static bool blinkState = false;
static unsigned long lastBlink = 0;

// =============================
// Inicialização
// =============================
void lightsInit() {

  pinMode(PIN_LED_HEADLIGHT, OUTPUT);
  pinMode(PIN_LED_LEFT,      OUTPUT);
  pinMode(PIN_LED_RIGHT,     OUTPUT);

  // API nova do Core 3.x
  ledcAttach(PIN_LED_TAIL, TAIL_PWM_FREQ, TAIL_PWM_RES);
  ledcWrite(PIN_LED_TAIL, 0);

  digitalWrite(PIN_LED_HEADLIGHT, LOW);
  digitalWrite(PIN_LED_LEFT,      LOW);
  digitalWrite(PIN_LED_RIGHT,     LOW);

  blinkState = false;
  lastBlink = millis();
}

// =============================
// Atualiza o estado do pisca
// =============================
void lightsLoop() {

  unsigned long now = millis();

  if (now - lastBlink >= BLINK_PERIOD_MS) {
    lastBlink = now;
    blinkState = !blinkState;
  }
}

// =============================
// Aplica estado lógico → hardware
// =============================
void lightsApplyState(const VehicleLightState& state) {

  // Farol dianteiro
  digitalWrite(PIN_LED_HEADLIGHT,
               state.headlightEnabled ? HIGH : LOW);

  // Lanterna traseira com intensidade
  if (state.brakeEnabled) {
    ledcWrite(PIN_LED_TAIL, TAIL_BRAKE_LEVEL);
  }
  else if (state.tailEnabled) {
    ledcWrite(PIN_LED_TAIL, TAIL_DIM_LEVEL);
  }
  else {
    ledcWrite(PIN_LED_TAIL, 0);
  }

  // Setas
  digitalWrite(PIN_LED_LEFT,
               (state.leftEnabled && blinkState) ? HIGH : LOW);

  digitalWrite(PIN_LED_RIGHT,
               (state.rightEnabled && blinkState) ? HIGH : LOW);
}