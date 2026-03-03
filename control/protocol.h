#pragma once
#include <stdint.h>
#include <stdbool.h>

// Comandos CONTROLE → CARRINHO
#define FLAG_HEADLIGHT  (1 << 0)
#define FLAG_LEFT       (1 << 1)
#define FLAG_RIGHT      (1 << 2)
#define FLAG_HAZARD     (1 << 3)
#define FLAG_BRAKE      (1 << 4)

typedef struct {
  uint8_t flags;
  uint32_t seq;
  uint16_t steering;
} ControlPacket;

// Telemetria CARRINHO → CONTROLE
typedef struct {
  uint8_t activeFlags;   // estado final das luzes
  bool failsafe;
  uint32_t seqEcho;      // eco do último comando
} TelemetryPacket;
