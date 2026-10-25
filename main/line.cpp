#include "sdkconfig.h"
#include <Arduino.h>

#include <QTRSensors.h>
#include "line.h"

QTRSensors qtr;
uint16_t sensors[8];
bool blackness[8];

void setupLineSensor() { 
    // set up Serial Communication and sensor pins
    //USE LEFTMOST AND RIGHTMOST PINS MAYBE
    qtr.setTypeAnalog(); // or setTypeAnalog()
    qtr.setSensorPins((const uint8_t[]) {32, 33, 34, 35, 36, 39, 25, 26}, 8); // pin numbers go in the curly brackets {}, and number of sensors in use goes after

    // calibration sequence
    for (uint8_t i = 0; i < 250; i++) { 
        Serial.printf("calibrating %d/250\n", i); // 250 is the number of calibrations recommended by manufacturer
        qtr.calibrate(); 
        delay(20);
    }
    Serial.println("Calibration complete");
}

void loopLineSensor() {
    qtr.readLineBlack(sensors); // Get calibrated sensor values returned into sensors[]
    Serial.printf("S1: %d S2: %d\n", sensors[0], sensors[1]);

    //1000 = dark, 0 = white.

    //WIP
    for(int i = 0; i < 8; i++){
        blackness[i] = sensors[i] > 500;
    }

    if(blackness[0] && blackness[7]){
        //stop (both sides are black, 2 lines)
    }
    else if(blackness[0] && !blackness[7]){
        //turn left (line on left)
    }
    else if(!blackness[0] && blackness[7]){
        //turn right 
    }
    else{
        //go straight (both sides are white, robot is presumably on the line).
    }


    //if color closer to black than white, turn motor(s)
    //might only turn one motor to turn... can see if we need to turn by seeing how many sensors are on the line.


    delay(250);
}
