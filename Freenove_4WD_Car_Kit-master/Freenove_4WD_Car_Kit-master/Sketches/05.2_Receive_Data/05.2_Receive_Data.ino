/**********************************************************************
  Filename    : Receive_Data.ino
  Product     : Freenove 4WD Car for UNO
  Description :Receive data from remote control and print it to monitor.
  Auther      : www.freenove.com
  Modification: 2019/08/06
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
int pot1Pos = 0;
int pot2Pos = 0;
int joystickXPos = 0;
int joystickYPos = 0;
int joystickZPos = 0;
int s1Pos = 0;
int s2Pos = 0;
int s3Pos = 0;

RF24 radio(PIN_SPI_CE, PIN_SPI_CSN); // define an object to control NRF24L01
const byte addresses[6] = "Free1";   //set commucation address, same to remote controller
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
    pot1Pos = nrfDataRead[0];
    pot2Pos = nrfDataRead[1];
    joystickXPos = nrfDataRead[2];
    joystickYPos = nrfDataRead[3];
    joystickZPos = nrfDataRead[4];
    s1Pos = nrfDataRead[5];
    s2Pos = nrfDataRead[6];
    s3Pos = nrfDataRead[7];
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
    Serial.print('\n');
  }
  
  if (joystickXPos>502) {
    Serial.println("Forward");
    digitalWrite(PIN_DIRECTION_LEFT, HIGH);
    digitalWrite(PIN_DIRECTION_RIGHT, LOW);
    analogWrite(PIN_MOTOR_PWM_LEFT, round(255*((joystickXPos-502)/(1023-502)))); //(255/(1023-502))*(joystickXPos-502)
    analogWrite(PIN_MOTOR_PWM_RIGHT, round(255*((joystickXPos-502)/(1023-502))));
  } else if ( joystickXPos<502 ) {
    Serial.println("Backward");
    digitalWrite(PIN_DIRECTION_LEFT, LOW);
    digitalWrite(PIN_DIRECTION_RIGHT, HIGH);
    analogWrite(PIN_MOTOR_PWM_LEFT, round(255*((502-joystickXPos)/502))); 
    analogWrite(PIN_MOTOR_PWM_RIGHT, round(255*((502-joystickXPos)/502)));
    } else {
      analogWrite(PIN_MOTOR_PWM_LEFT, 0); 
      analogWrite(PIN_MOTOR_PWM_RIGHT, 0);
    }
  
 
}
