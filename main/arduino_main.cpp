#include <Arduino.h>
#include <Bluepad32.h>
#include <uni.h>

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
    uni_bt_allowlist_set_enabled(false);
}

void loop() {
    vTaskDelay(1);
    BP32.update();

    static RunMode mode = RunMode::Manual;
    static bool lastA = false;
    static bool lastB = false;
    static bool lastX = false;
    static bool lastY = false;
   
    
    bool controllerActive = false;
    for (ControllerPtr controller : controllers) {
        if (controller == nullptr) {
            continue;
        }
        if (!controller->isConnected()) {
            continue;
        }
        controllerActive = true;
        if (!controller->hasData()) {
            continue;
        }

        controllerActive = true;
        uint8_t dpad = controller->dpad();

        bool a = controller->b();
        bool b = controller->a();
        bool x = controller->y();
        bool y = controller->x();

        bool left = dpad & DPAD_LEFT;
        bool right = dpad & DPAD_RIGHT;
        bool up = dpad & DPAD_UP;
        bool down = dpad & DPAD_DOWN;





        // buttons only change modes when they are first pressed
        if (a && !lastA) {
            Serial.println("pressed A");
            startColorTrack();
            mode = RunMode::ColorTrack;
        }
        if (b && !lastB) {
            Serial.println("pressed B");
            mode = RunMode::LineFollow;
        }
        if (x && !lastX) {
            Serial.println("pressed X");
            mode = RunMode::Manual;
        }
        if(y && !lastY) {
            Serial.println("pressed Y");
            calibrateLineSensor();
        }

        lastA = a;
        lastB = b;
        lastX = x;
        lastY = y;

        if (mode == RunMode::Manual) {
            // int speed;
            // if (abs(controller->axisY()) < 80) {
            //     speed = 0;
            // } else {
            //     speed = -controller->axisY() * 255 / 512;
            //     speed = constrain(speed, -255, 255);
            // }
            // int turn = abs(controller->axisX()) < 80 ? 0 : controller->axisX() * 120 / 512;
            // setMotorSpeeds(speed + turn, speed - turn);
            int leftSpeed;
            int rightSpeed;

            if(left && !right) {
                leftSpeed = -255;
                rightSpeed = 255;
            } else if(!left && right) {
                leftSpeed = 255;
                rightSpeed = -255;
            } else if(up && !down) {
                leftSpeed = 255;
                rightSpeed = 255;
            } else if(!up && down) {
                leftSpeed = -255;
                rightSpeed = -255;
            } else {
                leftSpeed = 0;
                rightSpeed = 0;
            }
            setMotorSpeeds(leftSpeed, rightSpeed);

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