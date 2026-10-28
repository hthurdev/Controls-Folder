
#include <Servo.h>

#define PIN_SERVO 2

Servo servo;

// Rack and pinion
const float travelMM = 20.0;  // Distance to move (mm)
const float pitch = 6.71;     // Circular pitch (mm)
const int teeth = 20;

// Servo settings
const int speed = 10;         // Same speed both ways
const float RPM = 10.0;       // Calibrate actual RPM

void setup() {
  servo.attach(PIN_SERVO);
  servo.write(90);  // Stop
  delay(1000);
}

void loop() {

  // Calculate movement time from rack distance
  float circumference = pitch * teeth;
  float timeMS = (travelMM / circumference)
                 * (60000.0 / RPM);

  // Extend
  servo.write(90 + speed);
  delay(timeMS);

  // Stop
  servo.write(90);
  delay(1000);

  // Retract
  servo.write(90 - speed);
  delay(timeMS);

  // Stop
  servo.write(90);
  delay(1000);
}
