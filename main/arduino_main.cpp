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
#define TEST_L_IN1 16
#define TEST_L_IN2 17
#define TEST_R_IN1 18
#define TEST_R_IN2 19
#define TEST_DEADZONE 80

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
//alrigh lowk all this rn 
void testMotorInit() {
    pinMode(TEST_L_IN1, OUTPUT); pinMode(TEST_L_IN2, OUTPUT);
    pinMode(TEST_R_IN1, OUTPUT); pinMode(TEST_R_IN2, OUTPUT);
}

void testMotorSet(int in1, int in2, int speed) {   // -255..255
    if (speed > 0)      { analogWrite(in1, speed); digitalWrite(in2, LOW); }
    else if (speed < 0) { digitalWrite(in1, LOW);  analogWrite(in2, -speed); }
    else                { digitalWrite(in1, LOW);  digitalWrite(in2, LOW); }
}

void testMotorStop() {
    testMotorSet(TEST_L_IN1, TEST_L_IN2, 0);
    testMotorSet(TEST_R_IN1, TEST_R_IN2, 0);
}

void testDrive(ControllerPtr ctl) {
    int y = -ctl->axisY();                    // up = positive = forward
    if (abs(y) < TEST_DEADZONE) y = 0;
    int speed = constrain(y * 255 / 512, -255, 255);
    testMotorSet(TEST_L_IN1, TEST_L_IN2, speed);
    testMotorSet(TEST_R_IN1, TEST_R_IN2, speed);
}

void setup() {
    pinMode(ONBOARD_LED_PIN, OUTPUT);
    Serial.begin(115200);
    testMotorInit();
    // qtr.setTypeAnalog(); // or setTypeAnalog()                                //////////////////////////////////////////////////.
    // qtr.setSensorPins((const uint8_t[]) {32, 33}, 2); // pin numbers go in the curly brackets {}, and number of sensors in use goes after

    // for (uint8_t i = 0; i < 250; i++) {
    //     Console.printf("calibrating %d/250\n", i); // 250 is the number of calibrations recommended by manufacturer
    //     qtr.calibrate();
    //     delay(20);
    // }

    //setupLineSensor();
    //setupColorSensor();

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
    vTaskDelay(1); // keeps the watchdog happy
    BP32.update();

    // Non-blocking LED blink (replaces the two delay(100) calls)
    static uint32_t lastBlink = 0;
    if (millis() - lastBlink >= 200) {
        lastBlink = millis();
        digitalWrite(ONBOARD_LED_PIN, !digitalRead(ONBOARD_LED_PIN));
    }

    static bool lastA = false;
    bool controllerActive = false;
    for (auto myController : myControllers)
    {
        if (myController && myController->isConnected()) controllerActive = true;
        if (myController && myController->isConnected() && myController->hasData())
        {
            dumpGamepad(myController);
            testDrive(myController);
            controllerActive = true;
            // A button: act once per press, not every loop
            bool a = myController->a();
            if (a && !lastA) {
                Console.printf(">>> A pressed\n");
                // setupLineSensor();  // re-enable later; just gonna comment for now, to test this
            }
            lastA = a;
        }
    }
    if (!controllerActive) testMotorStop();

    // loopLineSensor();
    // loopColorSensor();   // commented out so nothing slows the loop while testing
}