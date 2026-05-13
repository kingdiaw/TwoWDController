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
const double MAX_SPEED = 0.8; // Maximum speed for testing

unsigned long lastPrint = 0;

void setup() {
  Serial.begin(115200);
  pinMode(POT_PIN, INPUT);
  analogReadResolution(12);
  
  robot.begin();
  robot.startAutoUpdate(0.033);
  
  Serial.println("========================================");
  Serial.println("MOTOR SATURATION TEST");
  Serial.println("========================================");
  Serial.println("PWM(%)\tPWM\tRPM\tSpeed(m/s)");
  Serial.println("-------\t-------\t-------\t-----------");
}

void loop() {
  // Read potentiometer
  int adcValue = analogRead(POT_PIN);
  
  // Map to PWM (0-255)
  int pwmValue = map(adcValue, 0, ADC_MAX, 0, 255);
  double speed = map(adcValue, 0, ADC_MAX, 0, MAX_SPEED * 1000) / 1000.0;
  
  // Apply to robot
  robot.drive(speed, speed);
  
  // Get actual RPM
  double rpm = robot.getSpeedRPM(0);  //0 - left motor, 1 - right motor
  
  // Print every 500ms
  if (millis() - lastPrint >= 500) {
    lastPrint = millis();
    
    float pwmPercent = (pwmValue / 255.0) * 100;
    
    Serial.print(pwmPercent, 1);
    Serial.print("%\t");
    Serial.print(pwmValue);
    Serial.print("\t");
    Serial.print(rpm, 1);
    Serial.print("\t");
    Serial.println(speed, 3);
  }
  
  delay(50);
}