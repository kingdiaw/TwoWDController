#include <TwoWDController.h>

TwoWDController robot(
  0x20, 15, 2,
  P2, P3,
  P0, P1,
  18, 19, 12, 13
);

unsigned long lastPIDUpdate = 0;
unsigned long lastPrint = 0;

void setup() {
  Serial.begin(115200);
  robot.begin();
  robot.resetPID();
  robot.drive(0.15, 0.15);
  
  Serial.println("Manual PID Update Mode - No Ticker");
  Serial.println("PID updates every 33ms in main loop");
}

void loop() {
  // Manual PID update every 33ms
  if (millis() - lastPIDUpdate >= 33) {
    lastPIDUpdate = millis();
    robot.updatePID();
  }
  
  // Print debug info every 500ms
  if (millis() - lastPrint >= 500) {
    lastPrint = millis();
    Serial.print("Left: ");
    Serial.print(robot.getLeftSpeed(), 3);
    Serial.print(" m/s | PWM: ");
    Serial.print(robot.getLeftOutput());
    Serial.print("  Right: ");
    Serial.print(robot.getRightSpeed(), 3);
    Serial.print(" m/s | PWM: ");
    Serial.println(robot.getRightOutput());
  }
  
  // You can add other non-blocking tasks here
  // without worrying about Ticker interrupts
}