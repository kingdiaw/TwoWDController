#include <TwoWDController.h>

TwoWDController robot(0x20, 4, 2, P2, P3, P0, P1, 18, 19, 25, 13);

// Calculated for 180° right arc with 0.4572m radius
const double LEFT_SPEED = 0.12;     // Left wheel faster
const double RIGHT_SPEED = 0.08;    // Right wheel slower
const long TARGET_TICKS = 26940;    // Ticks for 180° arc

long startTicks = 0;

void setup() {
  Serial.begin(115200);
  robot.begin();
  robot.startAutoUpdate(0.033);
  
  Serial.println("180° Right Arc (Radius: 0.4572m)");
  Serial.println("Starting in 2 seconds...");
  delay(2000);
  
  robot.resetEncoders();
  startTicks = robot.getLeftPosition();
  robot.drive(LEFT_SPEED, RIGHT_SPEED);
}

void loop() {
  long ticks = abs(robot.getLeftPosition() - startTicks);
  
  if (ticks >= TARGET_TICKS) {
    robot.stop();
    Serial.println("180° Right Arc Complete!");
    while(1);  // Stop here
  }
  
  delay(10);
}