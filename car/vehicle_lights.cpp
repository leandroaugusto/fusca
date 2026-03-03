#include "vehicle_lights.h"
#include "protocol.h"
#include "config.h"
#include <Arduino.h>

static VehicleLightState state;
static unsigned long lastCommandTime = 0;

void vehicleLightsInit() {
  state = {};
  lastCommandTime = millis();
}

void vehicleLightsUpdateFromCommand(uint8_t flags) {
  lastCommandTime = millis();
  // Serial.println("UPDATED lastCommandTime");
  state.failsafe = false;

  state.headlightEnabled = flags & FLAG_HEADLIGHT;
  state.tailEnabled      = state.headlightEnabled;

  state.hazardEnabled = flags & FLAG_HAZARD;
  state.brakeEnabled = flags & FLAG_BRAKE;

  if (state.hazardEnabled) {
    state.leftEnabled  = true;
    state.rightEnabled = true;
  } else {
    state.leftEnabled  = flags & FLAG_LEFT;
    state.rightEnabled = flags & FLAG_RIGHT;
  }
}

void vehicleLightsLoop() {
  // if (millis() - lastCommandTime > FAILSAFE_TIMEOUT_MS) {
  //   state.failsafe = true;

  //   state.hazardEnabled = true;
  //   state.leftEnabled  = true;
  //   state.rightEnabled = true;

  //   state.headlightEnabled = false;
  //   state.tailEnabled = false;
  // }
  unsigned long delta = millis() - lastCommandTime;

  // Serial.print("DELTA: ");
  // Serial.println(delta);

  if (delta > FAILSAFE_TIMEOUT_MS) {
    state.failsafe = true;

    state.hazardEnabled = true;
    state.leftEnabled  = true;
    state.rightEnabled = true;

    state.headlightEnabled = false;
    state.tailEnabled = false;

    state.brakeEnabled = false;
  }
}

VehicleLightState vehicleLightsGetState() {
  return state;
}

uint8_t vehicleLightsGetActiveFlags() {
  uint8_t flags = 0;

  if (state.headlightEnabled) flags |= FLAG_HEADLIGHT;
  if (state.leftEnabled)      flags |= FLAG_LEFT;
  if (state.rightEnabled)     flags |= FLAG_RIGHT;
  if (state.hazardEnabled)    flags |= FLAG_HAZARD;

  return flags;
}
