#include <Arduino.h>
#include <Bluepad32.h>
#include <uni.h>

#include "controller_callbacks.h"
#include "color.h"
#include "line.h"
#include "motor.h"
#include "robot_config.h"

enum class RunMode
{
    Manual,
    ColorTrack,
    LineFollow
};

void setup()
{
    Serial.begin(115200);
    // start the robot parts before bluetooth
    setupMotor();
    setupLineSensor();
    setupColorSensor();
    BP32.setup(&onConnectedController, &onDisconnectedController);
    uni_bt_allowlist_set_enabled(false);
}

void loop()
{
    vTaskDelay(1);
    BP32.update();

    static RunMode mode = RunMode::Manual;
    static bool lastA = false;
    static bool lastB = false;
    static bool lastX = false;
    static bool lastY = false;

    bool controllerActive = false;
    for (ControllerPtr controller : controllers)
    {
        if (controller == nullptr)
        {
            continue;
        }
        if (!controller->isConnected())
        {
            continue;
        }
        controllerActive = true;

        bool a = controller->b();
        bool b = controller->a();
        bool x = controller->y();
        bool y = controller->x();

        // buttons only change modes when they are first pressed
        if (a && !lastA)
        {
            Serial.println("pressed A");
            startColorTrack();
            mode = RunMode::ColorTrack;
        }
        if (b && !lastB)
        {
            Serial.println("pressed B");
            mode = RunMode::LineFollow;
        }
        if (x && !lastX) //not using this rn
        {
            Serial.println("pressed X");
            mode = RunMode::Manual;
        }
        if (y && !lastY)
        {
            Serial.println("pressed Y");
            calibrateLineSensor();
        }

        lastA = a;
        lastB = b;
        lastX = x;
        lastY = y;

        if (mode == RunMode::Manual)
        {
            constexpr int deadzone = 80;
            int forward = -controller->axisY();
            int turn = controller->axisRX();

            if (abs(forward) < deadzone)
            {
                forward = 0;
            }
            if (abs(turn) < deadzone)
            {
                turn = 0;
            }

            forward = forward * MotorMax / 512;
            turn = turn * MotorMax / 512;

            const int leftSpeed = constrain(forward + turn, -MotorMax, MotorMax);
            const int rightSpeed = constrain(forward - turn, -MotorMax, MotorMax);
            setMotorSpeeds(leftSpeed, rightSpeed);
        }
    }
    if (!controllerActive && mode == RunMode::Manual)
    {
        stopMotors();
    }

    if (mode == RunMode::ColorTrack)
    {
        
        setMotorSpeeds(150, 150);
        if (colorTrackStep())
        {
            Serial.println("first color reached again, stopping");
            stopMotors();
            mode = RunMode::Manual;
        }
    }
    else if (mode == RunMode::LineFollow)
    {
        bool reachedLineEnd = lineFollowStep();
        if (reachedLineEnd)
        {
            Serial.println("line ended, stopping");
            stopMotors();
            mode = RunMode::Manual;
        }
    }
}