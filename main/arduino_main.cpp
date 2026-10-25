// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 Ricardo Quesada
// http://retro.moe/unijoysticle2

#include "sdkconfig.h"
#include <Arduino.h>
#include <Bluepad32.h>
#include <uni.h>
#include "controller_callbacks.h"
#include "line.h"
#include "color.h"
#include "motor.h"

#define ONBOARD_LED_PIN 2
// #include <QTRSensors.h>

// QTRSensors qtr;
// uint16_t sensors[2];

extern ControllerPtr myControllers[BP32_MAX_GAMEPADS]; // BP32 library allows for up to 4 concurrent controller connections, but we only need 1

void dumpGamepad(ControllerPtr ctl)
{
    Console.printf(
        "DPAD: %d A: %d B: %d X: %d Y: %d LX: %d LY: %d RX: %d RY: %d L1: %d R1: %d L2: %d R2: %d\n",
        ctl->dpad(), // D-pad
        ctl->a(),    // Letter buttons
        ctl->b(),
        ctl->x(),
        ctl->y(),
        ctl->axisX(),  // (-511 - 512) left X Axis
        ctl->axisY(),  // (-511 - 512) left Y axis
        ctl->axisRX(), // (-511 - 512) right X axis
        ctl->axisRY(), // (-511 - 512) right Y axis
        ctl->l1(),     // Bumpers
        ctl->r1(),
        ctl->l2(),
        ctl->r2());
}

void setup() {
    pinMode(ONBOARD_LED_PIN, OUTPUT);
    Serial.begin(115200);
    // qtr.setTypeAnalog(); // or setTypeAnalog()                                //////////////////////////////////////////////////.
    // qtr.setSensorPins((const uint8_t[]) {32, 33}, 2); // pin numbers go in the curly brackets {}, and number of sensors in use goes after

    // for (uint8_t i = 0; i < 250; i++) {
    //     Console.printf("calibrating %d/250\n", i); // 250 is the number of calibrations recommended by manufacturer
    //     qtr.calibrate();
    //     delay(20);
    // }

    setupLineSensor();
    setupColorSensor();

    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.forgetBluetoothKeys();
    esp_log_level_set("gpio", ESP_LOG_ERROR); // Suppress info log spam from gpio_isr_service
    bd_addr_t addr;
    sscanf_bd_addr("DC:CD:18:A5:A6:CB", addr);
    uni_bt_allowlist_add_addr(addr);
    uni_bt_allowlist_set_enabled(true);
}

void loop()
{
    vTaskDelay(1); // Ensures WDT does not get triggered when no controller is connected
    BP32.update();

    // qtr.readLineBlack(sensors); // Get calibrated sensor values returned into sensors[]
    // Console.printf("S1: %d S2: %d\n", sensors[0], sensors[1]);
    // delay(250);

    digitalWrite(ONBOARD_LED_PIN, HIGH); // writes a digital high to pin 2
    delay(100); // waits for 1000 milliseconds (1 second)
    digitalWrite(ONBOARD_LED_PIN, LOW); // writes a digital low to pin 2
    delay(100);

    for (auto myController : myControllers)
    { // Only execute code when controller is connected
        if (myController && myController->isConnected() && myController->hasData())
        {

            /*
            ====================
            Your code goes here!
            ====================
            */
            while (true)
            {
                if (myController->a())
                {
                    setupLineSensor(); // idk if this work... something like this... make functions for whatever we want to do with controller buttons
                }
            }

            dumpGamepad(myController); // Prints the gamepad state, delete or comment if don't need
        }
    }

    // TODO: add a manual drive mode with triggers and dpad

    // loopLineSensor();
    loopColorSensor();
}
