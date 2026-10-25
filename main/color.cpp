#include "sdkconfig.h"
#include <Arduino.h>

#include <Wire.h>
#include <Arduino_APDS9960.h>

#define APDS9960_INT -1   // INT pin not connected
#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_FREQ 100000

TwoWire I2C_0 = TwoWire(0);
APDS9960 apds = APDS9960(I2C_0, APDS9960_INT);

void setupColorSensor() {
    I2C_0.begin(I2C_SDA, I2C_SCL, I2C_FREQ);

    if (!apds.begin()) {
        printf("APDS-9960 NOT FOUND - check wiring\n");
    } else {
        printf("APDS-9960 found\n");
    }
}

void loopColorSensor() {
    static uint32_t last = 0;
    if (millis() - last < 100) return;
    last = millis();

    if (!apds.colorAvailable()) {
        printf("color not available\n");
        return;
    }

    int r, g, b, a;
    apds.readColor(r, g, b, a);
    printf("RED: %d GREEN: %d BLUE: %d AMBIENT: %d\n", r, g, b, a);
}