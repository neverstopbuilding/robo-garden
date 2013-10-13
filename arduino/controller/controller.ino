
/* 
 Stepper Motor Control - one revolution
 
 This program drives a unipolar or bipolar stepper motor. 
 The motor is attached to digital pins 8 - 11 of the Arduino.
 
 The motor should revolve one revolution in one direction, then
 one revolution in the other direction.  
 
  
 Created 11 Mar. 2007
 Modified 30 Nov. 2009
 by Tom Igoe
 
 */

#include <Stepper.h>

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
const float mmPerStep = 0.31;                                   // for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8,9,10,11);            

int enable = 7;

void setup() {
  
  pinMode(enable, OUTPUT);
  digitalWrite(enable, LOW);
  // set the speed at 60 rpm:
  myStepper.setSpeed(80);
  // initialize the serial port:
  Serial.begin(9600);
}

void loop() {
  

if (Serial.available())
{
  int cm = Serial.parseInt();
    Serial.println(cm);
    int steps = round((cm * 10) / mmPerStep);
    Serial.println(steps);
    
    digitalWrite(enable, HIGH);
    delay(20); 
    myStepper.step(steps);
    delay(20); 
    digitalWrite(enable, LOW);
}


  
  
  // step one revolution  in one direction:
//   Serial.println("clockwise");
//  myStepper.step(stepsPerRevolution);
//  delay(500);
  
   // step one revolution in the other direction:
//  Serial.println("counterclockwise");
//  myStepper.step(-stepsPerRevolution);
//  delay(500); 
}

