#include "display.h"
#include "config.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1

static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void displayInit() {
  Wire.begin(PIN_OLED_SDA, PIN_OLED_SCL); // SDA, SCL

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true); // falha crítica
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void displayUpdate(const TelemetryPacket& telemetry) {
  static unsigned long lastDraw = 0;
  if (millis() - lastDraw < 100) return;
  lastDraw = millis();

  display.clearDisplay();

  uint8_t f = telemetry.activeFlags;

  // Linha 1 — Luzes
  display.setCursor(0, 0);
  display.print((f & FLAG_HEADLIGHT) ? "HL " : "-- ");
  display.print((f & FLAG_LEFT)      ? "L "  : "-- ");
  display.print((f & FLAG_RIGHT)     ? "R "  : "-- ");
  display.print((f & FLAG_HAZARD)    ? "HZ"  : "--");

  // Linha 2 — Status
  display.setCursor(0, 16);
  display.print("FS:");
  display.print(telemetry.failsafe ? "ON " : "OK ");

  // Link simples (eco de sequência)
  static uint32_t lastSeq = 0;
  static unsigned long lastLink = 0;

  if (telemetry.seqEcho != lastSeq) {
    lastSeq = telemetry.seqEcho;
    lastLink = millis();
  }

  display.print("LK:");
  display.print((millis() - lastLink < 300) ? "OK" : "--");

  display.display();
}
