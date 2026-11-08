#include <Arduino.h>
#include <Bluepad32.h>

#include "controller_callbacks.h"
#include "color.h"
#include "line.h"
#include "motor.h"

enum class RunMode { Manual, ColorTrack, LineFollow };

void setup() {
    Serial.begin(115200);
    // start the robot parts before bluetooth
    setupMotor();
    setupLineSensor();
    setupColorSensor();

    BP32.setup(&onConnectedController, &onDisconnectedController);
}

void loop() {
    vTaskDelay(1);
    BP32.update();

    static RunMode mode = RunMode::Manual;
    static bool lastA = false;
    static bool lastB = false;

    
    bool controllerActive = false;
    for (ControllerPtr controller : controllers) {
        if (controller == nullptr) {
            continue;
        }
        if (!controller->isConnected()) {
            continue;
        }
        if (!controller->hasData()) {
            continue;
        }

        controllerActive = true;
        bool a = controller->a();
        bool b = controller->b();
        // buttons only change modes when they are first pressed
        if (a && !lastA) {
            startColorTrack();
            mode = RunMode::ColorTrack;
        }
        if (b && !lastB) {
            mode = RunMode::LineFollow;
        }
        if (controller->x()) {
            mode = RunMode::Manual;
        }
        lastA = a;
        lastB = b;

        if (mode == RunMode::Manual) {
            int speed;
            if (abs(controller->axisY()) < 80) {
                speed = 0;
            } else {
                speed = -controller->axisY() * 255 / 512;
                speed = constrain(speed, -255, 255);
            }
            int turn = controller->axisX() * 120 / 512;
            setMotorSpeeds(speed + turn, speed - turn);
        }
    }
    if (!controllerActive && mode == RunMode::Manual) {
        stopMotors();
    }

    // both autonomous modes use the same line follower
    if (mode == RunMode::ColorTrack) {
        lineFollowStep();
        if (colorTrackStep()) {
            Serial.println("first color reached again, stopping");
            stopMotors();
            mode = RunMode::Manual;
        }
    } else if (mode == RunMode::LineFollow) {
        bool reachedLineEnd = lineFollowStep();
        if (reachedLineEnd) {
            Serial.println("line ended, stopping");
            stopMotors();
            mode = RunMode::Manual;
        }
    }
}