#include <TwoWDController.h>

TwoWDController robot(
  0x20, 4, 2,
  P2, P3, P0, P1,
  18, 19, 25, 13
);

// USER INPUT: Calculate your target ticks here
// Formula: TARGET_TICKS = (1 meter) / (wheel_diameter * PI / encoder_resolution)
// For wheel diameter = 0.067m, encoder_resolution = 3950:
// TARGET_TICKS = 1 / (0.067 * 3.14159 / 3950) = 18770
const long TARGET_TICKS = 18770;  // ← CHANGE THIS BASED ON YOUR ROBOT

const double MOVE_SPEED = 0.15;

long startTicks = 0;

void setup() {
  Serial.begin(115200);
  robot.begin();
  robot.startAutoUpdate(0.033);
  
  Serial.println("Move 1 Meter");
  Serial.print("Target Ticks: ");
  Serial.println(TARGET_TICKS);
  Serial.println("Starting in 2 seconds...");
  delay(2000);
  
  robot.resetEncoders();
  startTicks = robot.getLeftPosition();
  robot.drive(MOVE_SPEED, MOVE_SPEED);
}

void loop() {
  long currentTicks = robot.getLeftPosition() - startTicks;
  
  if (currentTicks >= TARGET_TICKS) {
    robot.stop();
    Serial.print("Stopped at ");
    Serial.print(currentTicks);
    Serial.println(" ticks");
    while(1);  // Stop here
  }
  
  delay(10);
}