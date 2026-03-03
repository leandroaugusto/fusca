#include <WiFi.h>
#include <esp_now.h>
#include <ESP32Servo.h>
#include "config.h"
#include "protocol.h"
#include "vehicle_lights.h"
#include "lights.h"
#include <esp_wifi.h>

Servo steeringServo;

// Último comando recebido
static ControlPacket lastCmd = {};

// Telemetria enviada ao controle
static TelemetryPacket telemetry = {};

// MAC do controle (aprendido dinamicamente)
// static uint8_t controlMAC[6] = {0};
// uint8_t controlMAC[] = {0x18, 0x8B, 0x0E, 0x31, 0x91, 0xE8};
uint8_t controlMAC[] = {0x24, 0xEC, 0x4A, 0xCA, 0xFC, 0x88};

void onReceive(
  const esp_now_recv_info_t *info,
  const uint8_t *data,
  int len
) {
  Serial.println("RX OK");

  if (len == sizeof(ControlPacket)) {
    memcpy(&lastCmd, data, sizeof(ControlPacket));
    memcpy(controlMAC, info->src_addr, 6);

    // Serial.print("CMD FLAGS: ");
    // Serial.println(lastCmd.flags, BIN);

    // Atualiza estado lógico do veículo
    vehicleLightsUpdateFromCommand(lastCmd.flags);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("BOOT");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  esp_wifi_set_ps(WIFI_PS_NONE);
  esp_wifi_set_channel(11, WIFI_SECOND_CHAN_NONE);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro ao iniciar ESP-NOW Carrinho");
    return;
  }

  esp_now_set_pmk((uint8_t*)"pmk1234567890123");
  esp_now_register_recv_cb(onReceive);

  steeringServo.attach(PIN_SERVO, 500, 2400);

  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, controlMAC, 6);
  peer.channel = 11;
  peer.encrypt = false;

  if (esp_now_add_peer(&peer) != ESP_OK) {
    Serial.println("Erro add peer carrinho");
  }

  vehicleLightsInit();
  lightsInit();
}

void loop() {
  // Atualiza regras internas (failsafe, prioridades, etc)
  vehicleLightsLoop();

  uint16_t potValue = lastCmd.steering;

  // converte 0–4095 para 180–0 graus
  int angle = map(potValue, 0, 4095, 150, 0);
  steeringServo.write(angle);

  Serial.print("Pot: ");
  Serial.print(potValue);
  Serial.print(" Angle: ");
  Serial.println(angle);

  // if (vehicleLightsGetState().failsafe > 0) {
  //   Serial.print(" FAILSAFE: ");
  //   Serial.println(vehicleLightsGetState().failsafe);
  // }

  lightsLoop();
  lightsApplyState(vehicleLightsGetState());

  // Monta telemetria (estado REAL do veículo)
  telemetry.activeFlags = vehicleLightsGetActiveFlags();
  telemetry.failsafe    = vehicleLightsGetState().failsafe;
  telemetry.seqEcho     = lastCmd.seq;

  // Envia telemetria de volta ao controle (~20 Hz)
  static unsigned long lastSend = 0;
  if (controlMAC[0] != 0 && millis() - lastSend >= 80) {
    lastSend = millis();

    esp_now_send(controlMAC,
                 (uint8_t*)&telemetry,
                 sizeof(telemetry));
  }
}
