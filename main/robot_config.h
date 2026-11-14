#pragma once

#include <stdint.h>

// change the pins here if the wiring changes
namespace RobotPins {
constexpr uint8_t leftMotorIn1 = 16;
constexpr uint8_t leftMotorIn2 = 17;
constexpr uint8_t rightMotorIn1 = 18;
constexpr uint8_t rightMotorIn2 = 19;

constexpr uint8_t colorSda = 21;
constexpr uint8_t colorScl = 22;

// left to right when looking forward
constexpr uint8_t lineSensors[] = {12, 14, 27, 26, 25, 33, 32, 35};
constexpr uint8_t lineSensorCount = sizeof(lineSensors) / sizeof(lineSensors[0]);
}  // namespace RobotPins

constexpr int kMotorMax = 255;
constexpr int kLineBaseSpeed = 145;
constexpr int kLineMaxCorrection = 145;
 