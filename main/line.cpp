#include <Arduino.h>
#include <QTRSensors.h>

#include "line.h"
#include "motor.h"
#include "robot_config.h"

namespace {
QTRSensors qtr;
uint16_t sensorValues[RobotPins::lineSensorCount];
int lastError = 0;
int darkReadings = 0;
// the sensor position is measured in steps of 1000
constexpr int kLineCenter = (RobotPins::lineSensorCount - 1) * 1000 / 2;
constexpr float kP = 0.085f;
constexpr float kD = 0.12f;
constexpr int kDarkSensorValue = 800;
constexpr int kEndReadingsNeeded = 25;
}

void setupLineSensor() {
    qtr.setTypeAnalog();
    qtr.setSensorPins(RobotPins::lineSensors, RobotPins::lineSensorCount);

    Serial.println("calibrating line sensors, move them across the line");
    for (uint8_t i = 0; i < 250; i++) {
        qtr.calibrate();
        delay(20);    
    }
    Serial.println("line sensor calibration complete");
}

bool lineFollowStep() {
    const uint16_t position = qtr.readLineBlack(sensorValues);
    const int positionAsInt = position;
    const int error = positionAsInt - kLineCenter;
    const int derivative = error - lastError;
    lastError = error;

    int correction = kP * error + kD * derivative;
    correction = constrain(correction, -kLineMaxCorrection, kLineMaxCorrection);
    setMotorSpeeds(kLineBaseSpeed + correction, kLineBaseSpeed - correction);

    bool allSensorsSeeDark = true;
    for (uint8_t i = 0; i < RobotPins::lineSensorCount; i++) {
        if (sensorValues[i] < kDarkSensorValue) {
            allSensorsSeeDark = false;
            break;
        }
    }

    if (allSensorsSeeDark) {
        if (darkReadings < kEndReadingsNeeded) {
            darkReadings = darkReadings + 1;
        }
    } else {
        darkReadings = 0;
    }

    return darkReadings >= kEndReadingsNeeded;
}
