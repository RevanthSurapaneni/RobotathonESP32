
#include "sdkconfig.h"
#include <Arduino.h>
#include <ESP32Servo.h>
#include "motor.h"

#define IN1  16  // Control pin 1
#define IN2  17  // Control pin 2
Servo myServo;
void setupMotor() {
  Serial.begin(115200);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);







  //SERVO
  myServo.attach(12);
}

void loopMotor() {

    // Spin motor
    analogWrite(IN1, 255);  // PWM signal
    digitalWrite(IN2, LOW); // Direction control

    delay(1000);  // Run for 1 second

    // Stop motor
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);

    delay(1000); // Stop for 1 second


    //SERVO
    myServo.write(0); 
    delay(1000); // Delay 1000 ms
    myServo.write(180); 
    delay(1000);

    //TODO: need a different task delay? put servo in different file?

    vTaskDelay(1); // Yield CPU to not starve other ESP32 processes and cause WDT reset
}

void spinMotor1(){

}
