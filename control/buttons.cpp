#include "buttons.h"
#include "config.h"
#include <Arduino.h>

typedef struct {
  uint8_t pin;
  bool stableState;
  bool lastReading;
  unsigned long lastChange;
} DebouncedButton;

static DebouncedButton btnHead;
static DebouncedButton btnLeftB;
static DebouncedButton btnRightB;
static DebouncedButton btnHaz;
static DebouncedButton btnBrakeB;

static void initButton(DebouncedButton& b, uint8_t pin) {
  b.pin = pin;
  b.stableState = true;          // INPUT_PULLUP → solto = HIGH
  b.lastReading = true;
  b.lastChange = 0;
  pinMode(pin, INPUT_PULLUP);
}

static void updateButton(DebouncedButton& b) {
  bool reading = digitalRead(b.pin);
  if (reading != b.lastReading) {
    b.lastChange = millis();
    b.lastReading = reading;
  }

  if ((millis() - b.lastChange) > DEBOUNCE_MS) {
    b.stableState = reading;
  }
}

void buttonsInit() {
  initButton(btnHead, PIN_BTN_HEADLIGHT);
  initButton(btnLeftB, PIN_BTN_LEFT);
  initButton(btnRightB, PIN_BTN_RIGHT);
  initButton(btnHaz, PIN_BTN_HAZARD);
  initButton(btnBrakeB, PIN_BTN_BRAKE);
}

void buttonsUpdate() {
  updateButton(btnHead);
  updateButton(btnLeftB);
  updateButton(btnRightB);
  updateButton(btnHaz);
  updateButton(btnBrakeB);
}

// Funções públicas (true = botão pressionado)
bool btnHeadlight() { return btnHead.stableState == LOW; }
bool btnLeft()      { return btnLeftB.stableState == LOW; }
bool btnRight()     { return btnRightB.stableState == LOW; }
bool btnHazard()    { return btnHaz.stableState == LOW; }
bool btnBrake()     { return btnBrakeB.stableState == LOW; }
