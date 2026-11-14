#include <Arduino.h>
#include <QTRSensors.h>

#include "line.h"
#include "motor.h"
#include "robot_config.h"

namespace {
QTRSensors lineSensor;
uint16_t calibratedSensorValues[RobotPins::lineSensorCount];
int consecutiveAllWhiteReadings = 0;

constexpr int whiteSurfaceThreshold = 800;
constexpr int requiredEndReadings = 25;
constexpr uint16_t calibrationReadingCount = 250;
bool lineSensorIsCalibrated = false;
}

void setupLineSensor() {
    lineSensor.setTypeAnalog();
    lineSensor.setSensorPins(RobotPins::lineSensors, RobotPins::lineSensorCount);
    lineSensorIsCalibrated = false;
}

void calibrateLineSensor(){

    stopMotors();
    Serial.println("calibrating line sensors");
    for (uint16_t reading = 0; reading < calibrationReadingCount; reading++) {
        lineSensor.calibrate();
        delay(20);    
    }

    consecutiveAllWhiteReadings = 0;
    lineSensorIsCalibrated = true;
    Serial.println("line sensor calibrated");
}
bool lineFollowStep() {
    if (!lineSensorIsCalibrated) {
        stopMotors();
        return false;
    }

    lineSensor.readLineWhite(calibratedSensorValues);

    bool allSensorsSeeWhite = true;
    for (uint8_t i = 0; i < RobotPins::lineSensorCount; i++) {
        if (calibratedSensorValues[i] < whiteSurfaceThreshold) {
            allSensorsSeeWhite = false;
            break;
        }
    }

    if (allSensorsSeeWhite) {
        if (consecutiveAllWhiteReadings < requiredEndReadings) {
            consecutiveAllWhiteReadings++;
        }
    } else {
        consecutiveAllWhiteReadings = 0;
    }

    bool centerSeesWhite =
        calibratedSensorValues[3] >= whiteSurfaceThreshold ||
        calibratedSensorValues[4] >= whiteSurfaceThreshold;
    bool leftSeesWhite =
        calibratedSensorValues[0] >= whiteSurfaceThreshold ||
        calibratedSensorValues[1] >= whiteSurfaceThreshold ||
        calibratedSensorValues[2] >= whiteSurfaceThreshold;
    bool rightSeesWhite =
        calibratedSensorValues[5] >= whiteSurfaceThreshold ||
        calibratedSensorValues[6] >= whiteSurfaceThreshold ||
        calibratedSensorValues[7] >= whiteSurfaceThreshold;

    if (centerSeesWhite) {
        setMotorSpeeds(250, 250);
    } else if (leftSeesWhite) {
        setMotorSpeeds(-100, 150);
    } else if (rightSeesWhite) {
        setMotorSpeeds(150, -100);
    } else {
        stopMotors();  //if there is no white, probably at the end
    }

    return consecutiveAllWhiteReadings >= requiredEndReadings;
}
