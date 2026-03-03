#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct {
  bool headlightEnabled;   // farol / lanterna ligados
  bool tailEnabled;

  bool leftEnabled;        // seta esquerda habilitada
  bool rightEnabled;       // seta direita habilitada
  bool hazardEnabled;      // pisca-alerta habilitado
  bool brakeEnabled;

  bool failsafe;
} VehicleLightState;

void vehicleLightsInit();
void vehicleLightsUpdateFromCommand(uint8_t flags);
void vehicleLightsLoop();

VehicleLightState vehicleLightsGetState();
uint8_t vehicleLightsGetActiveFlags();
