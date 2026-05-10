#include <TwoWDController.h>

// PCF8574 I2C Mode
// Note: P0, P1, P2, P3 are defined in PCF8574.h
TwoWDController robot(
  0x20,          // I2C address
  15, 2,         // left enable, right enable (PWM pins on ESP32)
  P2, P3,        // Left motor IN1, IN2 on PCF8574 (P2, P3 from library)
  P0, P1,        // Right motor IN1, IN2 on PCF8574 (P0, P1 from library)
  18, 19, 12, 13 // encoders: leftA, leftB, rightA, rightB (GPIO pins)
);

void setup() {
  Serial.begin(115200);
  robot.begin();
  robot.resetPID();
  robot.startAutoUpdate(0.033);
  
  Serial.println("Starting movement sequence...");
  delay(2000);
}

void loop() {
  Serial.println("Moving forward");
  robot.drive(0.2, 0.2);
  delay(3000);
  
  Serial.println("Turning left");
  robot.drive(-0.1, 0.1);
  delay(2000);
  
  Serial.println("Moving backward");
  robot.drive(-0.2, -0.2);
  delay(3000);
  
  Serial.println("Turning right");
  robot.drive(0.1, -0.1);
  delay(2000);
  
  Serial.println("Stop");
  robot.stop();
  delay(2000);
}