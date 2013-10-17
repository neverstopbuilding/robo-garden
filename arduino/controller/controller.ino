/* 
 _____       _            _____               _            
|  __ \     | |          / ____|             | |           
| |__) |___ | |__   ___ | |  __  __ _ _ __ __| | ___ _ __  
|  _  // _ \| '_ \ / _ \| | |_ |/ _` | '__/ _` |/ _ \ '_ \ 
| | \ \ (_) | |_) | (_) | |__| | (_| | | | (_| |  __/ | | |
|_|  \_\___/|_.__/ \___/ \_____|\__,_|_|  \__,_|\___|_| |_|
                                                            
RoboGarden Control Software

RoboGarden by Jason Fox is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
http://creativecommons.org/licenses/by-nc-sa/3.0/deed.en_US
*/

#include <Stepper.h>

//Global Constants
const int stepsPerRevolution = 200;  //Steps for full revolution of motor
const float stepLength = 0.29066320618892; //Distance carriage moves per step in mm
const int controlSwitchDelay = 1750; //Number of milliseconds since last button press before movement will occur
const float maxDistance = 720; //Maximum distance in mm that the carriage can travel before the other side hits the right end
const int numberOfPlants = 8; //Number of plants between 0 and 8
const float homeOffset = 15; //Distance from homeswitch close where the home would be in mm
const float spoutOffset = 20.46; //distance in mm from the edge of the carriage that hits the home switch and the center of the water spout

//Pins
// Pin 0 Unused
// Pin 1 Unused
int homeSwitch = 0; //Actually into pin 2 but this is the 0th interrupt pin
int controlSwitch = 3;
int shiftRegisterSerial = 4;
int shiftRegisterClock = 5;
int shiftRegisterLatch = 6;
int stepperEnable = 7;
Stepper stepperMotor(stepsPerRevolution, 8,9, 10, 11);     
// Pin 12 Unused
// Pin 13 Unused

//Global and Starting State Variables
int oldControlSwitchState = LOW;
unsigned long controlSwitchTimeOnDown = 0;
unsigned long timeOfLastShortPress = 0;
volatile int displayNumber = 0;
volatile int oldDisplayNumber = 0;
volatile float currentPosition = 0;
volatile float absoluteDistance = maxDistance + homeOffset;
volatile int direction = -1;

//Setup
void setup() {

  //Pin Modes
  attachInterrupt(homeSwitch, resetHomePosition, RISING);
  pinMode(controlSwitch, INPUT);
  pinMode(shiftRegisterSerial, OUTPUT);
  pinMode(shiftRegisterClock, OUTPUT);
  pinMode(shiftRegisterLatch, OUTPUT);
  pinMode(stepperEnable, OUTPUT);

  //Starting State and Settings
  digitalWrite(stepperEnable, LOW);
  stepperMotor.setSpeed(100);
  Serial.begin(9600);
  
  //Print out the plant locations
  printPlantLocations();
}

//Main Loop
void loop() {

  //Movement logic sends carriage to a location based on distance in mm and direction (-1 is right, 1 is left)
  if (absoluteDistance > 0) {
    digitalWrite(stepperEnable, HIGH);
    stepperMotor.step(direction); //Move a single step either right or left based on sign
    if (currentPosition >= maxDistance + spoutOffset) {
      absoluteDistance = 0;
    } else {
      currentPosition += (direction * stepLength);
      absoluteDistance -= stepLength;
      if (absoluteDistance <= 0) {
        Serial.print("Current position: ");
        Serial.println(currentPosition);
      }
    }
  }
  digitalWrite(stepperEnable, LOW);

  //Selection of location logic
  if (displayNumber != oldDisplayNumber && getTimeSinceLastShortPress() > controlSwitchDelay){
    Serial.print("New location: ");
    Serial.println(displayNumber);
    float relativeDistance = getPlantLocation(displayNumber) - currentPosition;
    absoluteDistance = abs(relativeDistance);
    direction = relativeDistance/ abs(relativeDistance);
    oldDisplayNumber = displayNumber;
  }

  //Control switch logic
  int controlSwitchState = digitalRead(controlSwitch);

  if(controlSwitchState != oldControlSwitchState) {
    if (controlSwitchState == HIGH){
      controlSwitchTimeOnDown = millis();
    }
    oldControlSwitchState = controlSwitchState;
  }

  if (controlSwitchTimeOnDown > 0){
    int controlSwitchPressLength = millis() - controlSwitchTimeOnDown;
    if (controlSwitchState == HIGH) {
      if (controlSwitchPressLength >= 1000 ) {
        //Logic on long press
        currentPosition = 0;
        absoluteDistance = maxDistance + homeOffset;
        direction = -1;
        Serial.println("Homing...");

        //Reset logic
        controlSwitchTimeOnDown = 0;
      }
    } else {
      //Logic on short press

      if (displayNumber == 8) {
        displayNumber = 0;
      } else {
        displayNumber++;
      }
      timeOfLastShortPress = millis();
      Serial.print("New display number: ");
      Serial.println(displayNumber);

      //Reset logic
      controlSwitchTimeOnDown = 0;
    }
  }
  setDisplayNumber(displayNumber);

}

//Helper Functions
void setDisplayNumber(int number){
  digitalWrite(shiftRegisterLatch, LOW);
  shiftOut(shiftRegisterSerial, shiftRegisterClock, MSBFIRST, number); 
  digitalWrite(shiftRegisterLatch, HIGH);
}

unsigned long getTimeSinceLastShortPress(){
  if (timeOfLastShortPress == 0){
    return 0;
  } else {
    return millis() - timeOfLastShortPress;
  }
}

float getPlantLocation(int plantNumber){
  if (plantNumber == 0){
    return 0;
  }
  float plantWidth = maxDistance / numberOfPlants;
  return ((plantNumber - 1) * plantWidth ) + (.5 * plantWidth);
}

void printPlantLocations(){
  Serial.println("Please place your plants at these distances from the water spout center at home:");
  for (int p = 1; p <= numberOfPlants; p++){
    Serial.print("Plant ");
    Serial.print(p);
    Serial.print(" at ");
    Serial.print(getPlantLocation(p));
    Serial.println("mm from home.");
  }
}

//Interrupt Functions
void resetHomePosition(){
  direction = 1;
  currentPosition = - homeOffset;
  absoluteDistance = homeOffset;
  displayNumber = 0;
  oldDisplayNumber = 0;
}
