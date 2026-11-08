#include <Arduino.h>
#include <Wire.h>
#include <Arduino_APDS9960.h>

#include "color.h"
#include "robot_config.h"

namespace {
TwoWire i2c = TwoWire(0);
APDS9960 apds(i2c, -1);
ColorId firstColor = ColorId::None;
ColorId previousColor = ColorId::None;
bool leftFirstTile = false;
uint8_t stableSamples = 0;
uint32_t lastRead = 0;

ColorId classifyColor(int red, int green, int blue, int ambient) {
    // ignore readings that are too dark or too close to gray
    if (ambient < 20) {
        return ColorId::None;
    }

    const int brightest = max(red, max(green, blue));
    const int darkest = min(red, min(green, blue));
    if (brightest < 30 || brightest - darkest < brightest / 5) {
        return ColorId::None;
    }

    bool looksYellow = red > blue * 3 / 2 && green > blue * 3 / 2;
    bool looksRed = red > green * 5 / 4 && red > blue * 5 / 4;
    bool looksGreen = green > red * 5 / 4 && green > blue * 5 / 4;
    bool looksBlue = blue > red * 5 / 4 && blue > green * 5 / 4;

    if (looksYellow) {
        return ColorId::Yellow;
    }
    if (looksRed) {
        return ColorId::Red;
    }
    if (looksGreen) {
        return ColorId::Green;
    }
    if (looksBlue) {
        return ColorId::Blue;
    }

    return ColorId::None;
}
}

void setupColorSensor() {
    i2c.begin(RobotPins::colorSda, RobotPins::colorScl, 100000);

    if (!apds.begin()) {
        Serial.println("APDS-9960 not found; check I2C wiring.");
    } else {
        Serial.println("APDS-9960 found.");
    }
}

void startColorTrack() {
    firstColor = ColorId::None;
    previousColor = ColorId::None;
    leftFirstTile = false;
    stableSamples = 0;
}

bool colorTrackStep() {
    bool enoughTimePassed = millis() - lastRead >= 50;
    bool colorReadingAvailable = apds.colorAvailable();
    if (!enoughTimePassed || !colorReadingAvailable) {
        return false;
    }

    lastRead = millis();
    int r, g, b, a;
    apds.readColor(r, g, b, a);
    const ColorId color = classifyColor(r, g, b, a);

    if (color == ColorId::None) {
        // no color means we have left the current tile
        stableSamples = 0;
        if (firstColor != ColorId::None) leftFirstTile = true;
        previousColor = ColorId::None;
        return false;
    }

    if (color == previousColor) {
        if (stableSamples < 3) {
            stableSamples = stableSamples + 1;
        }
    } else {
        previousColor = color;
        stableSamples = 1;
    }

    if (firstColor == ColorId::None && stableSamples >= 3) {
        firstColor = color;
        Serial.println("remembered first color");
    }

    bool foundFirstColorAgain = leftFirstTile
        && color == firstColor
        && stableSamples >= 3;
    return foundFirstColorAgain;
}