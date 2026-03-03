#pragma once
#include "vehicle_lights.h"

void lightsInit();
void lightsLoop();
void lightsApplyState(const VehicleLightState& state);
