TwoWDController Library - Additional Information

Installation:
1. Copy the TwoWDController folder to your Arduino/libraries/ directory
2. Restart Arduino IDE
3. The library will appear in Sketch > Include Library > TwoWDController

Dependencies:
- This library requires the PCF8574 library for I2C expander support
- Install PCF8574 library via Library Manager if using I2C expander

Hardware Requirements:
- ESP32 board (for encoder interrupt support)
- L298N motor driver (or compatible)
- DC gear motors with encoders
- Optional: PCF8574 I2C expander

Pin Connections (Direct GPIO):
- Motor pins: PWM enable pins, direction control pins
- Encoder pins: A and B channels for left and right encoders

Pin Connections (PCF8574):
- Direction control pins connected to PCF8574
- PWM enable pins connected directly to ESP32
- Encoder pins connected directly to ESP32

Important Notes:
1. The PID control loop should be called periodically (every 33ms recommended)
2. Encoder interrupts are automatically configured
3. The library uses floating-point calculations for speed control
4. Default PID constants work well for typical 2WD robots

Troubleshooting:
- If motors don't move, check pin assignments and power supply
- If encoder readings are noisy, add pull-up resistors or use INPUT_PULLUP
- Adjust PID constants in PIDControl.h for your specific robot

Library Methods:
- begin(): Initialize the controller
- update(): Update PID control loop
- driveSpeed(left_mps, right_mps): Drive at specified speeds
- drivePWM(left_pwm, right_pwm): Direct PWM control
- stop(): Emergency stop
- getLeftEncoder()/getRightEncoder(): Read encoder positions
- getLeftSpeedMPS()/getRightSpeedMPS(): Get actual speeds
- resetEncoders(): Reset encoder positions to zero
- setMaxSpeed(mps): Set maximum speed limit