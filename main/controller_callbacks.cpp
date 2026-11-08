#include <Arduino.h>
#include <Bluepad32.h>

#include "controller_callbacks.h"

ControllerPtr controllers[BP32_MAX_GAMEPADS] = {};

void onConnectedController(ControllerPtr controller) {
    // keep the controller in the first free slot
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (controllers[i] == nullptr) {
            controllers[i] = controller;
            Serial.print("controller connected: ");
            Serial.println(controller->getModelName());
            return;
        }
    }
}

void onDisconnectedController(ControllerPtr controller) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (controllers[i] == controller) {
            controllers[i] = nullptr;
            Serial.println("controller disconnected");
            return;
        }
    }
}
