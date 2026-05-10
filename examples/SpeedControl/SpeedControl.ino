#include <TwoWDController.h>

TwoWDController robot(
  0x20, 15, 2,
  P2, P3,  // Left motor pins (from PCF8574.h)
  P0, P1,  // Right motor pins (from PCF8574.h)
  18, 19, 12, 13
);

unsigned long lastPrint = 0;

void setup() {
  Serial.begin(115200);
  robot.begin();
  robot.startAutoUpdate(0.033);
  robot.drive(0.15, 0.15);
  
  Serial.println("Speed Control Active");
  Serial.println("Time(ms)\tLeft(m/s)\tRight(m/s)\tLeftPWM\tRightPWM");
}

void loop() {
  if (millis() - lastPrint >= 500) {
    lastPrint = millis();
    Serial.print(millis());
    Serial.print("\t");
    Serial.print(robot.getLeftSpeed(), 3);
    Serial.print("\t");
    Serial.print(robot.getRightSpeed(), 3);
    Serial.print("\t");
    Serial.print(robot.getLeftOutput());
    Serial.print("\t");
    Serial.println(robot.getRightOutput());
  }
}