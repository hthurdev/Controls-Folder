/**********************************************************************
  Product     : Freenove 4WD Car for UNO
  Description : Set servo to 90 degree
  Auther      : www.freenove.com
  Modification: 2019/08/05
**********************************************************************/
#include "Servo.h"      //servo liberary
#include "time.h"

#define PIN_SERVO  2    //define servo pin

Servo servo;            //create servo object to control a servo
byte servoOffset = 0;   // change the value to Calibrate servo

void setup() {
  servo.attach(PIN_SERVO);        //initialize servo
  servo.write(90 + servoOffset);  //Calibrate servo
}

void loop() {
  servo.write(86);
  delay(4000);
  servo.write(0);
  delay(2000);
  servo.write(174);
  delay(2000);
}
