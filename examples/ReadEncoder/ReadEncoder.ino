#include <TwoWDController.h>

// PCF8574 I2C Mode
TwoWDController robot(
  0x20, 4, 2,
  P2, P3, P0, P1,
  18, 19, 25, 13
);

unsigned long lastPrint = 0;

void setup() {
  Serial.begin(115200);
  robot.begin();
  robot.resetEncoders();
  robot.startAutoUpdate(0.033);
  
  Serial.println("Simple Encoder Reader");
  Serial.println("Move the robot manually to see encoder values");
  Serial.println("Time(ms)\tLeftPos\tRightPos");
}

void loop() {
  // Simple continuous encoder reading
  if (millis() - lastPrint >= 500) {  // Update every 200ms
    lastPrint = millis();
    
    Serial.print(millis());
    Serial.print("\t");
    Serial.print(robot.getLeftPosition());
    Serial.print("\t");
    Serial.println(robot.getRightPosition());
  }
  
  // Optional: Add manual control
  if (Serial.available()) {
    char cmd = Serial.read();
    switch(cmd) {
      case 'f':  // forward
        robot.drive(0.15, 0.15);
        Serial.println("Moving forward");
        break;
      case 'b':  // backward
        robot.drive(-0.15, -0.15);
        Serial.println("Moving backward");
        break;
      case 'l':  // left turn
        robot.drive(-0.1, 0.1);
        Serial.println("Turning left");
        break;
      case 'r':  // right turn
        robot.drive(0.1, -0.1);
        Serial.println("Turning right");
        break;
      case 's':  // stop
        robot.stop();
        Serial.println("Stopped");
        break;
      case 'z':  // reset encoders
        robot.resetEncoders();
        Serial.println("Encoders reset to zero");
        break;
      default:
        break;
    }
  }
}