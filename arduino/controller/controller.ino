
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

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
const float mmPerStep = 0.31;                                   // for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8,9,10,11);            

int enable = 7;

int sr_serial = 4;
int sr_clock = 5;
int sr_register = 6;

int location_button = 3;



int location = 0;
int previous_location = 0;
int timer = 0;
int max_location = 8;
int location_button_state = 0;

void setup() {
  
  pinMode(enable, OUTPUT);
  
  pinMode(sr_serial, OUTPUT);
  pinMode(sr_clock, OUTPUT);
  pinMode(sr_register, OUTPUT);
  
  pinMode(location_button, INPUT);
  
  
  
  
  digitalWrite(enable, LOW);
  // set the speed at 60 rpm:
  myStepper.setSpeed(80);
  // initialize the serial port:
  Serial.begin(9600);
}

void loop() {
location_button_state = digitalRead(location_button);

if (previous_location != location && timer == 0){
  Serial.print("Gonna go to location: ");
  Serial.print(location);
  Serial.println();
  move(previous_location, location);
  previous_location = location;
} else {
  if (location_button_state == HIGH) {
    timer = 2000;
    if (location == 8) {
      location = 0;
    } else {
      location++;
    }
    ledNumber(location);
    delay(250);
  }
  if(timer > 0){
    delay(10);
    timer -= 10;
  }
}

}

void ledNumber(int number){
     digitalWrite(sr_register, LOW);
    shiftOut(sr_serial, sr_clock, MSBFIRST, number); 
    digitalWrite(sr_register, HIGH);
}

void move(int from, int to){
  int locations[] = {0, 5, 10, 15, 20, 25, 30, 35, 40};
  
  Serial.println(from);
  Serial.println(to);
  
  int from_mm = locations[from] * 10;
  
  Serial.println(from_mm);
  int to_mm = locations[to] * 10;
  Serial.println(to_mm);
  
  float movement = round((to_mm - from_mm) / mmPerStep);
  
  Serial.println(movement);
  
    digitalWrite(enable, HIGH);
    delay(20); 
    myStepper.step(movement);
    delay(20); 
    digitalWrite(enable, LOW);

}
