#include <TwoWDController.h>

TwoWDController robot(
  0x20,          // I2C address
  4, 2,          // left enable (GPIO4), right enable (GPIO2)
  P2, P3,        // Left motor IN1, IN2 on PCF8574
  P0, P1,        // Right motor IN1, IN2 on PCF8574
  18, 19, 25, 13 // encoders: leftA(GPIO18), leftB(GPIO19), rightA(GPIO25), rightB(GPIO13)
);

const int POT_PIN = 32;
const int ADC_MAX = 4095;
const double MAX_SPEED = 0.8;  // Maximum target speed (m/s)

unsigned long lastPrint = 0;

void setup() {
  Serial.begin(115200);
  pinMode(POT_PIN, INPUT);
  analogReadResolution(12);
  
  robot.begin();
  robot.startAutoUpdate(0.033);
  
  Serial.println("================================================================");
  Serial.println("MOTOR SATURATION TEST - Target vs Actual Speed");
  Serial.println("================================================================");
  Serial.println("PWM\tTarget(m/s)\tActual(m/s)\tRPM");
  Serial.println("-------\t-----------\t-----------\t-------");
}

void loop() {
  // Read potentiometer
  int adcValue = analogRead(POT_PIN);
  
  // Calculate target speed (linear mapping from potentiometer)
  double targetSpeed = map(adcValue, 0, ADC_MAX, 0, MAX_SPEED * 1000) / 1000.0;
  
  // Apply target speed to robot
  robot.drive(targetSpeed, targetSpeed);
  
  // Get actual PWM output from the motor driver
  int pwmValue = robot.getLeftOutput();  // Get actual left motor PWM output
  
  // Get actual speed and RPM from encoders
  double actualSpeed = robot.getLeftSpeed();  // Get actual left motor speed
  double rpm = robot.getSpeedRPM(0);          // Get left motor RPM
  
  // Print every 500ms
  if (millis() - lastPrint >= 500) {
    lastPrint = millis();
    
    Serial.print(pwmValue);
    Serial.print("\t");
    Serial.print(targetSpeed, 3);
    Serial.print("\t\t");
    Serial.print(actualSpeed, 3);
    Serial.print("\t\t");
    Serial.println(rpm, 1);
  }
  
  delay(50);
}