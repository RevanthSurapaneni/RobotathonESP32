#pragma once

#include <Bluepad32.h>

extern ControllerPtr controllers[BP32_MAX_GAMEPADS];

void onConnectedController(ControllerPtr controller);
void onDisconnectedController(ControllerPtr controller);