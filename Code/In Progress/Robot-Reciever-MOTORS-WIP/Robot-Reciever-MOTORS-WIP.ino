/**********************************************************************
  Filename    : Robot-Reciever-MOTORS-WIP
  Product     : Freenove 4WD Car for UNO
  Description : Receive data from remote control and control motors.
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

//Radio packets
//This is where I define the variables that get reassigned by the radio data
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

float joyXAbs = ((float(joystickXPos)-joystickXDefault)/(joystickXMax-joystickXDefault));
float joyYAbs = ((float(joystickYPos)-joystickYDefault)/(joystickYMax-joystickYDefault));

float joyMag;
float joyDeg;

float ThreeSixtyatanDeg(float XPos, float YPos) {
  if ( YPos >= 0 ) {
    float output = atan(float(XPos)/float(YPos)) * (180/PI) + 90;
    
    return output;
    
    
  } else {
    float output = atan(float(XPos)/float(YPos)) * (180/PI) + 180 + 90;
    
    return output;
  }  
}

float magnitude(float XPos, float YPos) {
  float Mag = sqrt(pow(XPos,2)+pow(YPos,2));
  if (Mag>1) {
    Mag = 1;
    return Mag;
  } else {
    return Mag;
  }
}



void driveForward(float mag) {
  Serial.println("Forward");
  digitalWrite(PIN_DIRECTION_LEFT, HIGH);
  digitalWrite(PIN_DIRECTION_RIGHT, LOW);
  analogWrite(PIN_MOTOR_PWM_LEFT, lround(255*mag));
  analogWrite(PIN_MOTOR_PWM_RIGHT, lround(255*mag));
}

void driveBackward(float mag) {
  Serial.println("Backward");
  digitalWrite(PIN_DIRECTION_LEFT, LOW);
  digitalWrite(PIN_DIRECTION_RIGHT, HIGH);
  analogWrite(PIN_MOTOR_PWM_LEFT, lround(255*mag));
  analogWrite(PIN_MOTOR_PWM_RIGHT, lround(255*mag));
}

void leftDriveForward(float mag) {
  Serial.println("LeftForward");
  digitalWrite(PIN_DIRECTION_LEFT, HIGH);
  analogWrite(PIN_MOTOR_PWM_LEFT, lround(255*mag));
}

void rightDriveForward(float mag) {
  Serial.println("RightForward");
  digitalWrite(PIN_DIRECTION_RIGHT, LOW);
  analogWrite(PIN_MOTOR_PWM_RIGHT, lround(255*mag));
}

void leftDriveBackward(float mag) {
  Serial.println("LeftBackward");
  digitalWrite(PIN_DIRECTION_LEFT, LOW);
  analogWrite(PIN_MOTOR_PWM_LEFT, lround(255*mag));
}

void rightDriveBackward(float mag) {
  Serial.println("RightBackward");
  digitalWrite(PIN_DIRECTION_RIGHT, HIGH);
  analogWrite(PIN_MOTOR_PWM_RIGHT, lround(255*mag));
}



RF24 radio(PIN_SPI_CE, PIN_SPI_CSN); // define an object to control NRF24L01
const byte addresses[6] = "TeAM2";   //set commucation address, same to remote controller
int nrfDataRead[8];            //define an array to save data from remote controller

void setup() {
  Serial.begin(9600);

  pinMode(PIN_DIRECTION_LEFT, OUTPUT);
  pinMode(PIN_MOTOR_PWM_LEFT, OUTPUT);
  pinMode(PIN_DIRECTION_RIGHT, OUTPUT);
  pinMode(PIN_MOTOR_PWM_RIGHT, OUTPUT);

  // NRF24L01
  if (radio.begin()) {                  // initialize RF24
    radio.setPALevel(RF24_PA_MAX);      // set power amplifier (PA) level
    radio.setDataRate(RF24_1MBPS);      // set data rate through the air
    radio.setRetries(0, 15);            // set the number and delay of retries
    radio.openWritingPipe(addresses);   // open a pipe for writing
    radio.openReadingPipe(1, addresses);// open a pipe for reading
    radio.startListening();             // start monitoringtart listening on the pipes opened
    Serial.println("Start listening remote data ... ");
  }
  else {
    Serial.println("Not found the nrf chip!");
  }

}

void loop() {
  delayMicroseconds(1000);
  if (radio.available()) {              // if receive the data
    while (radio.available()) {         // read all the data
      radio.read(nrfDataRead, sizeof(nrfDataRead));   // read data
    }
    Serial.print("P1/P2/X/Y/Z/S1/S2/S3 : ");
    //Hardcoded the data assignments because loops aren't that fun
    pot1Pos = nrfDataRead[0];
    pot2Pos = nrfDataRead[1];
    joystickXPos = nrfDataRead[2];
    joyXAbs = ((float(joystickXPos)-joystickXDefault)/(joystickXMax-joystickXDefault)); //convert to -1 to 1 value
    joystickYPos = nrfDataRead[3];
    joyYAbs = ((float(joystickYPos)-joystickYDefault)/(joystickYMax-joystickYDefault)); //convert to -1 to 1 value
    joyMag = magnitude(joyXAbs,joyYAbs);
    joyDeg = ThreeSixtyatanDeg(joyXAbs, joyYAbs);
    joystickZPos = nrfDataRead[4];
    s1Pos = nrfDataRead[5];
    s2Pos = nrfDataRead[6];
    s3Pos = nrfDataRead[7];
    //These print statements show a readout in the serial monitor tab, useful for debugging
    Serial.print(nrfDataRead[0]);
    Serial.print("\t");
    Serial.print(nrfDataRead[1]);
    Serial.print("\t");
    Serial.print(nrfDataRead[2]);
    Serial.print("\t");
    Serial.print(nrfDataRead[3]);
    Serial.print("\t");
    Serial.print(nrfDataRead[4]);
    Serial.print("\t");
    Serial.print(nrfDataRead[5]);
    Serial.print("\t");
    Serial.print(nrfDataRead[6]);
    Serial.print("\t");
    Serial.print(nrfDataRead[7]);
    Serial.print("\t Joy Deg: ");
    Serial.print(joyDeg);
    Serial.print("\t Joy Mag: ");
    Serial.print(joyMag);
    Serial.print('\n');
  }
  // Based on joystick input, assign the motors a speed
  if (165<joyDeg<195) {

    driveForward(joyMag);

  } else if (joyDeg>345 || joyDeg<15) {

    driveBackward(joyMag);

  } else if (joyDeg == 90) {

    leftDriveForward(joyMag);
    rightDriveBackward(joyMag);

  } else if (joyDeg == 270) {

    leftDriveBackward(joyMag);
    rightDriveForward(joyMag);

  } else if ( 165>=joyDeg>90 ) {

    leftDriveForward(joyMag);
    rightDriveForward(joyMag*sin((joyDeg-90)*(PI/180)));
    
  } else if (195<=joyDeg<270) {

    rightDriveForward(joyMag);
    leftDriveForward(joyMag*-1*cos((joyDeg)*(PI/180)));

  } else if (270<joyDeg<=345) {

    rightDriveBackward(joyMag);
    leftDriveBackward(joyMag*cos((joyDeg)*(PI/180)));

  }  else if (15<=joyDeg<90) {

    leftDriveBackward(joyMag);
    rightDriveBackward(joyMag*cos((joyDeg-90)*(PI/180)));

  } else {

    analogWrite(PIN_MOTOR_PWM_LEFT, 0);
    analogWrite(PIN_MOTOR_PWM_RIGHT, 0);

  }
 
}
