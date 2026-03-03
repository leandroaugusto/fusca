#include <WiFi.h>
#include <esp_now.h>
#include "config.h"
#include "protocol.h"
#include "buttons.h"
#include "display.h"
#include <esp_wifi.h>

// MAC DO CARRINHO
uint8_t carMAC[] = {0x98, 0x3D, 0xAE, 0x52, 0x6B, 0x48};

static TelemetryPacket telemetry = {};
static ControlPacket pkt = {};

unsigned long lastSend = 0;

void onReceive(
  const esp_now_recv_info_t *info,
  const uint8_t *data,
  int len
) {
  if (len == sizeof(TelemetryPacket)) {
    memcpy(&telemetry, data, sizeof(TelemetryPacket));
    // Serial.print("RX FLAGS: ");
    // Serial.println(telemetry.activeFlags, BIN);
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  esp_wifi_set_ps(WIFI_PS_NONE);
  esp_wifi_set_channel(11, WIFI_SECOND_CHAN_NONE);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro ESP-NOW Controle");
    return;
  }

  esp_now_set_pmk((uint8_t*)"pmk1234567890123");
  esp_now_register_recv_cb(onReceive);

  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, carMAC, 6);
  peer.channel = 11;
  peer.encrypt = false;

  // esp_now_add_peer(&peer);
  if (esp_now_add_peer(&peer) != ESP_OK) {
    Serial.println("Erro add peer");
  }

  buttonsInit();
  displayInit();

  pinMode(PIN_POT, INPUT);

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
}

void loop() {
  buttonsUpdate();
  displayUpdate(telemetry);

  // envio de comando (~66 Hz)
  if (millis() - lastSend >= 80) {
    lastSend = millis();

    pkt.flags = 0;
    pkt.seq++;

    if (btnHeadlight()) pkt.flags |= FLAG_HEADLIGHT;
    if (btnLeft())      pkt.flags |= FLAG_LEFT;
    if (btnRight())     pkt.flags |= FLAG_RIGHT;
    if (btnHazard())    pkt.flags |= FLAG_HAZARD;
    if (btnBrake())     pkt.flags |= FLAG_BRAKE;

    uint16_t potValue = analogRead(PIN_POT);

    pkt.steering = potValue;

    // esp_now_send(carMAC, (uint8_t*)&pkt, sizeof(pkt));
    esp_err_t result = esp_now_send(carMAC, (uint8_t*)&pkt, sizeof(pkt));
    
    Serial.println(potValue);

    if (result != ESP_OK) {
      Serial.println("Erro envio");
    }
  }

  // DEBUG DE TELEMETRIA (a cada 500 ms)
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 500) {
    lastPrint = millis();

    // Serial.print("FLAGS: ");
    // Serial.print(telemetry.activeFlags, BIN);
    // Serial.print(" FAILSAFE: ");
    // Serial.println(telemetry.failsafe);
  }
}

