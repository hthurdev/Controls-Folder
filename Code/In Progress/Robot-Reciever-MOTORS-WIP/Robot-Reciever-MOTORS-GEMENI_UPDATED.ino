/**********************************************************************
  Filename    : Robot-Reciever-MOTORS-WIP
  Product     : Freenove 4WD Car for UNO
  Description : Receive data from remote control and control motors with swapped steering.
  Author      : Hayden Thurman and Freenove
  Modification: 9/24/26
**********************************************************************/
#include <SPI.h>
#include "RF24.h"
#include <math.h>

//Radio pins
#define PIN_SPI_CE      9
#define PIN_SPI_CSN     10

//Motor Pins
#define PIN_DIRECTION_RIGHT 3
#define PIN_DIRECTION_LEFT  4
#define PIN_MOTOR_PWM_RIGHT 5
#define PIN_MOTOR_PWM_LEFT  6

// ==========================================
// TUNING & DRIFT CORRECTION
// ==========================================
const float leftMotorTrim    = 0.9760; 
const float rightMotorTrim   = 1.0000; 
const float steeringScale    = 0.65;   // Keeps turns smooth and prevents motor maxing out
// ==========================================

//Radio packets
int pot1Pos;
int pot2Pos;
int joystickXPos;
int joystickYPos;
int joystickZPos;
int s1Pos;
int s2Pos;
int s3Pos;

const float joystickXMax = 1004;
const float joystickXDefault = 502;

const float joystickYMax = 1018;
const float joystickYDefault = 509;

float joyXAbs = 0.0;
float joyYAbs = 0.0;
float joyMag = 0.0;
float joyDeg = 0.0;

float ThreeSixtyatanDeg(float XPos, float YPos) {
  if (YPos >= 0) {
    float output = atan(float(XPos) / float(YPos)) * (180 / PI) + 90;
    return output;
  } else {
    float output = atan(float(XPos) / float(YPos)) * (180 / PI) + 180 + 90;
    return output;
  }  
}

float magnitude(float XPos, float YPos) {
  float Mag = sqrt(pow(XPos, 2) + pow(YPos, 2));
  if (Mag > 1) {
    Mag = 1;
    return Mag;
  } else {
    return Mag;
  }
}

// Unified smooth motor mixing function
void moveRobot(float throttle, float steering) {
  float scaledSteering = steering * steeringScale;

  float leftSpeed  = throttle + scaledSteering;
  float rightSpeed = throttle - scaledSteering;

  float maxVal = max(abs(leftSpeed), abs(rightSpeed));
  if (maxVal > 1.0) {
    leftSpeed /= maxVal;
    rightSpeed /= maxVal;
  }

  leftSpeed  *= leftMotorTrim;
  rightSpeed *= rightMotorTrim;

  if (leftSpeed >= 0) {
    digitalWrite(PIN_DIRECTION_LEFT, HIGH);
    analogWrite(PIN_MOTOR_PWM_LEFT, lround(255 * leftSpeed));
  } else {
    digitalWrite(PIN_DIRECTION_LEFT, LOW);
    analogWrite(PIN_MOTOR_PWM_LEFT, lround(255 * abs(leftSpeed)));
  }

  if (rightSpeed >= 0) {
    digitalWrite(PIN_DIRECTION_RIGHT, LOW);
    analogWrite(PIN_MOTOR_PWM_RIGHT, lround(255 * rightSpeed));
  } else {
    digitalWrite(PIN_DIRECTION_RIGHT, HIGH);
    analogWrite(PIN_MOTOR_PWM_RIGHT, lround(255 * abs(rightSpeed)));
  }
}

RF24 radio(PIN_SPI_CE, PIN_SPI_CSN); 
const byte addresses[6] = "TeAM2";   
int nrfDataRead[8];                 

void setup() {
  Serial.begin(9600);

  pinMode(PIN_DIRECTION_LEFT, OUTPUT);
  pinMode(PIN_MOTOR_PWM_LEFT, OUTPUT);
  pinMode(PIN_DIRECTION_RIGHT, OUTPUT);
  pinMode(PIN_MOTOR_PWM_RIGHT, OUTPUT);

  if (radio.begin()) {                  
    radio.setPALevel(RF24_PA_MAX);      
    radio.setDataRate(RF24_1MBPS);      
    radio.setRetries(0, 15);            
    radio.openWritingPipe(addresses);   
    radio.openReadingPipe(1, addresses);
    radio.startListening();             
    Serial.println("Start listening remote data ... ");
  }
  else {
    Serial.println("Not found the nrf chip!");
  }
}

void loop() {
  delayMicroseconds(1000);
  if (radio.available()) {              
    while (radio.available()) {         
      radio.read(nrfDataRead, sizeof(nrfDataRead));   
    }
    
    pot1Pos = nrfDataRead[0];
    pot2Pos = nrfDataRead[1];
    joystickXPos = nrfDataRead[2];
    joyXAbs = ((float(joystickXPos) - joystickXDefault) / (joystickXMax - joystickXDefault)); 
    joystickYPos = nrfDataRead[3];
    joyYAbs = ((float(joystickYPos) - joystickYDefault) / (joystickYMax - joystickYDefault)); 
    joyMag = magnitude(joyXAbs, joyYAbs);
    joyDeg = ThreeSixtyatanDeg(joyXAbs, joyYAbs);
    joystickZPos = nrfDataRead[4];
    s1Pos = nrfDataRead[5];
    s2Pos = nrfDataRead[6];
    s3Pos = nrfDataRead[7];
  }

  // Deadzone filter
  if (abs(joyYAbs) < 0.05 && abs(joyXAbs) < 0.05) {
    analogWrite(PIN_MOTOR_PWM_LEFT, 0);
    analogWrite(PIN_MOTOR_PWM_RIGHT, 0);
  } else {
    float throttle = joyXAbs; 
    // Inverted steering (left becomes right) by adding a negative sign
    float steering = -joyYAbs;  
    
    moveRobot(throttle, steering);
  }
}