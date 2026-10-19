/**********************************************************************
  Filename    : Robot-Reciever-MOTORS-WIP
  Product     : Freenove 4WD Car for UNO
  Description : Receive data from remote control and control motors, 
                with added autonomous distance-driving capability.
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
// AUTONOMOUS DISTANCE CONFIGURATION
// ==========================================
// Set your desired distance here:
// Positive (+) for forward, Negative (-) for backward.
const float targetDistanceMeters = 0.5; // e.g., 0.5 meters (50 cm)

// Set your wheel radius in meters (e.g., a standard Freenove wheel is ~0.0325m radius / 65mm diameter)
//Note: factor of 0.0254 converts the 2.54 inches diamter to meters. Then multipled by 0.5 to get radius
const float wheelRadiusMeters = 0.5*2.54*0.0254; 

// Set a default motor power level for autonomous movement (0 to 1.0)
const float autoDriveMag = 0.6;
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

bool hasRunAutonomous = false; // Ensures the distance code only runs once on startup

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

void driveForward(float mag) {
  digitalWrite(PIN_DIRECTION_LEFT, HIGH);
  digitalWrite(PIN_DIRECTION_RIGHT, LOW);
  analogWrite(PIN_MOTOR_PWM_LEFT, lround(255 * mag));
  analogWrite(PIN_MOTOR_PWM_RIGHT, lround(255 * mag));
}

void driveBackward(float mag) {
  digitalWrite(PIN_DIRECTION_LEFT, LOW);
  digitalWrite(PIN_DIRECTION_RIGHT, HIGH);
  analogWrite(PIN_MOTOR_PWM_LEFT, lround(255 * mag));
  analogWrite(PIN_MOTOR_PWM_RIGHT, lround(255 * mag));
}

void stopMotors() {
  analogWrite(PIN_MOTOR_PWM_LEFT, 0);
  analogWrite(PIN_MOTOR_PWM_RIGHT, 0);
}

void leftDriveForward(float mag) {
  digitalWrite(PIN_DIRECTION_LEFT, HIGH);
  analogWrite(PIN_MOTOR_PWM_LEFT, lround(255 * mag));
}

void rightDriveForward(float mag) {
  digitalWrite(PIN_DIRECTION_RIGHT, LOW);
  analogWrite(PIN_MOTOR_PWM_RIGHT, lround(255 * mag));
}

void leftDriveBackward(float mag) {
  digitalWrite(PIN_DIRECTION_LEFT, LOW);
  analogWrite(PIN_MOTOR_PWM_LEFT, lround(255 * mag));
}

void rightDriveBackward(float mag) {
  digitalWrite(PIN_DIRECTION_RIGHT, HIGH);
  analogWrite(PIN_MOTOR_PWM_RIGHT, lround(255 * mag));
}

// Function to drive a specific distance based on wheel geometry and estimated speed/time
void runAutonomousDistance(float distanceMeters, float radius, float mag) {
  // Calculate wheel circumference: C = 2 * pi * r
  float circumference = 2 * PI * radius;
  
  // Find how many wheel rotations are needed
  float rotationsNeeded = abs(distanceMeters) / circumference;
  
  // NOTE: Convert rotations to milliseconds based on your robot's top speed calibration.
  // A typical Freenove car at full speed (mag = 1.0) travels roughly 0.8 meters per second. 
  // Adjust the calibration scalar (0.8) if your robot drives too far or too short.
  float estimatedSpeedMPS = 1.2 * mag; 
  float travelTimeSeconds = abs(distanceMeters) / estimatedSpeedMPS;
  unsigned long travelTimeMillis = (unsigned long)(travelTimeSeconds * 1000);

  Serial.print("Driving autonomous distance: ");
  Serial.print(distanceMeters);
  Serial.println(" meters.");

  if (distanceMeters > 0) {
    driveForward(mag);
  } else if (distanceMeters < 0) {
    driveBackward(mag);
  }

  delay(travelTimeMillis);
  stopMotors();
  Serial.println("Autonomous travel complete.");
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
  // Run the autonomous distance routine once right when the program boots up, 
  // before switching to remote control listening mode.
  if (!hasRunAutonomous) {
    if (targetDistanceMeters != 0.0) {
      runAutonomousDistance(targetDistanceMeters, wheelRadiusMeters, autoDriveMag);
    }
    hasRunAutonomous = true;
  }

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
    joyYAbs = ((float(joystickYPos) - joystickYDefault) / (joyYMax - joystickYDefault)); 
    joyMag = magnitude(joyXAbs, joyYAbs);
    joyDeg = ThreeSixtyatanDeg(joyXAbs, joyYAbs);
    joystickZPos = nrfDataRead[4];
    s1Pos = nrfDataRead[5];
    s2Pos = nrfDataRead[6];
    s3Pos = nrfDataRead[7];
  }

  // Normal Joystick Control Loop
  if (joyDeg > 165 && joyDeg < 195) {
    driveForward(joyMag);
  } else if (joyDeg > 345 || joyDeg < 15) {
    driveBackward(joyMag);
  } else if (joyDeg == 90) {
    leftDriveForward(joyMag);
    rightDriveBackward(joyMag);
  } else if (joyDeg == 270) {
    leftDriveBackward(joyMag);
    rightDriveForward(joyMag);
  } else if (joyDeg >= 90 && joyDeg <= 165) {
    leftDriveForward(joyMag);
    rightDriveForward(joyMag * sin((joyDeg - 90) * (PI / 180)));
  } else if (joyDeg >= 195 && joyDeg < 270) {
    rightDriveForward(joyMag);
    leftDriveForward(joyMag * -1 * cos((joyDeg) * (PI / 180)));
  } else if (joyDeg > 270 && joyDeg <= 345) {
    rightDriveBackward(joyMag);
    leftDriveBackward(joyMag * cos((joyDeg) * (PI / 180)));
  } else if (joyDeg >= 15 && joyDeg < 90) {
    leftDriveBackward(joyMag);
    rightDriveBackward(joyMag * cos((joyDeg - 90) * (PI / 180)));
  } else {
    stopMotors();
  }
}