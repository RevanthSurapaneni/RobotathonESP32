#include <Arduino.h>
#include <Wire.h>
#include <Arduino_APDS9960.h>

#include "color.h"
#include "robot_config.h"

namespace {
constexpr uint8_t colorSensorInterruptPin = 2;
TwoWire i2c = TwoWire(0);
APDS9960 apds(i2c, colorSensorInterruptPin);
bool firstColorSaved = false;
int firstRed = 0;
int firstGreen = 0;
int firstBlue = 0;
bool leftFirstTile = false;
uint8_t matchingFirstColorReadings = 0;
uint32_t lastColorReadTime = 0;
constexpr int rgbTolerance = 10;
}

void setupColorSensor() {
    i2c.begin(RobotPins::colorSda, RobotPins::colorScl, 100000);
    apds.setInterruptPin(colorSensorInterruptPin);

    if (!apds.begin()) {
        Serial.println("APDS-9960 not found; check I2C wiring.");
    } else {
        Serial.println("APDS-9960 found.");
    }
}

void startColorTrack() {
    firstColorSaved = false;
    firstRed = 0;
    firstGreen = 0;
    firstBlue = 0;
    leftFirstTile = false;
    matchingFirstColorReadings = 0;
    lastColorReadTime = 0;
    Serial.println("color tracking started");
}

bool colorTrackStep() {
    if (millis() - lastColorReadTime < 50 || !apds.colorAvailable()) {
        return false;
    }

    lastColorReadTime = millis();

    int red;
    int green;
    int blue;
    int ambient;
    apds.readColor(red, green, blue, ambient);

    Serial.printf("RGB: R=%d G=%d B=%d A=%d\n",
                  red, green, blue, ambient);

    const int brightest = max(red, max(green, blue));
    const bool validColorReading = ambient >= 20 && brightest >= 30;
    if (!validColorReading) {
        if (firstColorSaved) {
            leftFirstTile = true;
        }
        matchingFirstColorReadings = 0;
        return false;
    }

    if (!firstColorSaved) {
        firstRed = red;
        firstGreen = green;
        firstBlue = blue;
        firstColorSaved = true;
        Serial.printf("saved first RGB: R=%d G=%d B=%d\n",
                      firstRed, firstGreen, firstBlue);
        return false;
    }

    bool redMatches = abs(red - firstRed) <= rgbTolerance;
    bool greenMatches = abs(green - firstGreen) <= rgbTolerance;
    bool blueMatches = abs(blue - firstBlue) <= rgbTolerance;

    const bool matchesFirstColor = redMatches && greenMatches && blueMatches;
    if (!matchesFirstColor) {
        leftFirstTile = true;
        matchingFirstColorReadings = 0;
        return false;
    }

    if (leftFirstTile && matchingFirstColorReadings < 3) {
        matchingFirstColorReadings++;
    }

    return leftFirstTile && matchingFirstColorReadings >= 3;
}