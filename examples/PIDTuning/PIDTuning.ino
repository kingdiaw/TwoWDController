#include <TwoWDController.h>

TwoWDController robot(
  0x20, 15, 2,
  P2, P3,
  P0, P1,
  18, 19, 12, 13
);

unsigned long lastPrint = 0;
int testPhase = 0;
unsigned long testStart = 0;

void setup() {
  Serial.begin(115200);
  robot.begin();
  robot.startAutoUpdate(0.033);
  
  // Start with conservative PID values
  robot.setPIDConstants(15, 10, 0, 50);
  robot.drive(0.2, 0.2);
  
  Serial.println("PID Tuning Mode");
  Serial.println("Adjust Kp, Kd, Ki in real-time");
  Serial.println("Commands: p[value], d[value], i[value]");
  testStart = millis();
}

void loop() {
  // Auto-test sequence
  if (millis() - testStart > 5000) {
    testPhase++;
    testStart = millis();
    
    switch(testPhase % 4) {
      case 0: robot.setPIDConstants(15, 10, 0, 50); break;
      case 1: robot.setPIDConstants(25, 15, 0, 50); break;
      case 2: robot.setPIDConstants(35, 20, 0, 50); break;
      case 3: robot.setPIDConstants(20, 12, 1, 50); break;
    }
    
    Serial.print("Testing PID: Kp=");
    int kp, kd, ki, ko;
    robot.getPIDConstants(kp, kd, ki, ko);
    Serial.print(kp);
    Serial.print(" Kd=");
    Serial.print(kd);
    Serial.print(" Ki=");
    Serial.println(ki);
  }
  
  // Real-time monitoring
  if (millis() - lastPrint >= 500) {
    lastPrint = millis();
    Serial.print("Left: ");
    Serial.print(robot.getLeftSpeed(), 3);
    Serial.print(" m/s | PWM: ");
    Serial.print(robot.getLeftOutput());
    Serial.print(" | Target: ");
    Serial.print(robot.getTargetTicksPerFrame(0));
    Serial.print(" | Pos: ");
    Serial.print(robot.getLeftPosition());
    
    Serial.print("  Right: ");
    Serial.print(robot.getRightSpeed(), 3);
    Serial.print(" m/s | PWM: ");
    Serial.print(robot.getRightOutput());
    Serial.print(" | RPM: ");
    Serial.println(robot.getSpeedRPM(1));
  }
  
  // Serial command input for manual tuning
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.length() > 1) {
      char type = cmd.charAt(0);
      int value = cmd.substring(1).toInt();
      int kp, kd, ki, ko;
      robot.getPIDConstants(kp, kd, ki, ko);
      
      switch(type) {
        case 'p': case 'P': kp = value; break;
        case 'd': case 'D': kd = value; break;
        case 'i': case 'I': ki = value; break;
        default: return;
      }
      robot.setPIDConstants(kp, kd, ki, ko);
      Serial.print("Updated PID: Kp=");
      Serial.print(kp);
      Serial.print(" Kd=");
      Serial.print(kd);
      Serial.print(" Ki=");
      Serial.println(ki);
    }
  }
}