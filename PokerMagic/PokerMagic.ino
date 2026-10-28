#include <Servo.h>

#define PIN_SERVO 2

Servo servo;

// Continuous servo speed settings
const int speed = 5;          // 90 + 5 = forward, 90 - 5 = backward

void setup() {
  servo.attach(PIN_SERVO);
  servo.write(90);  // Stop
  
  // Setup delay so you have time to step back
  int k = 1; // 2 seconds delay
  delay(1000 * k);

  // --- ADJUST THESE TIMES DIRECTLY ---
  // Since backward is much faster/stronger than forward, 
  // give them separate, independent run times in milliseconds (ms).
  
  unsigned long backwardTimeMS = 1500; // Time moving backward (1.5 seconds - adjust as needed)
  unsigned long forwardTimeMS  = 2800; // Time moving forward  (4 seconds - adjust as needed)
// With new batteries Forward Time / Backward Time = 2.8/1.5
// With old batteries Forward Time / Backward Time = 2.0/1.5
  // 1. Move Backward
  servo.write(90 - speed);
  delay(backwardTimeMS);

  // Stop briefly between directions
  servo.write(90);
  delay(1000 * k);

  // 2. Move Forward
  servo.write(90 + speed);
  delay(forwardTimeMS);

  // Stop completely and detach so it holds position
  servo.write(90);
  servo.detach(); 
}

void loop() {
  // Left empty so it only runs once on startup
}