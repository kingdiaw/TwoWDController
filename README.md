# TwoWDController Library

A comprehensive 2WD mobile robot controller library for ESP32 with PID speed control, encoder feedback, and automatic PID updates using Ticker.

## Features

- ✅ **Complete 2WD Robot Control** - Perfect for differential drive robots
- ✅ **PID Speed Control** - Precise speed regulation with configurable constants
- ✅ **Encoder Support** - Real-time position and speed tracking
- ✅ **Dual Control Modes** - Direct GPIO or I2C (PCF8574) for motor drivers
- ✅ **Automatic PID Updates** - Built-in Ticker for regular PID loops (default 33ms)
- ✅ **Multiple Movement Types** - Speed control in m/s or percentage
- ✅ **Easy-to-Use API** - Simple and intuitive function calls
- ✅ **Real-time Monitoring** - Get speed, position, and PWM output values

## Requirements

### Hardware
- ESP32 development board
- 2WD mobile robot chassis
- DC gear motors with encoders
- Motor driver (L298N, L293D, or similar)
- Optionally: PCF8574 I2C I/O expander

### Software
- Arduino IDE 1.8.0 or newer
- ESP32 board support (installed via Boards Manager)
- [PCF8574_library](https://github.com/xreef/PCF8574_library) by Renzo Mischianti (required for I2C mode)

## Installation

### Method 1: Arduino Library Manager (Recommended)

1. Open Arduino IDE
2. Go to **Sketch → Include Library → Manage Libraries**
3. Search for **"TwoWDController"**
4. Click **Install**

**Note:** The library manager will automatically install the required dependency `PCF8574_library`.

### Method 2: Manual Installation

1. Download the latest release from [GitHub Releases](https://github.com/kingdiaw/TwoWDController/releases)
2. Extract the ZIP file
3. Copy the `TwoWDController` folder to your Arduino libraries directory:
   - **Windows:** `Documents\Arduino\libraries\`
   - **Mac:** `~/Documents/Arduino/libraries/`
   - **Linux:** `~/Arduino/libraries/`
4. **Install dependency:** In Arduino IDE, go to **Tools → Manage Libraries** and search for "PCF8574_library" by Renzo Mischianti
5. Restart Arduino IDE

### Method 3: Git Clone

```bash
cd ~/Arduino/libraries/
git clone https://github.com/kingdiaw/TwoWDController.git
```

Then install the PCF8574_library dependency as described in Method 2.

## Wiring Diagrams

| Encoder | ESP32 Pin |
|---------|-----------|
| Left Encoder A | GPIO 18 |
| Left Encoder B | GPIO 19 |
| Right Encoder A | GPIO 12 |
| Right Encoder B | GPIO 13 |

### I2C Mode (with PCF8574)

Connect PCF8574 to ESP32:
- **SDA** → GPIO 21 (default)
- **SCL** → GPIO 22 (default)
- **VCC** → 3.3V or 5V
- **GND** → GND

Connect motor driver to PCF8574:
- Left IN1 → P2
- Left IN2 → P3
- Right IN1 → P0
- Right IN2 → P1

Connect PWM pins directly to ESP32:
- Left Enable → GPIO 15
- Right Enable → GPIO 2

Encoder pins connect directly to ESP32 (same as GPIO mode).

## Basic Usage

### Example 1: Simple Movement with Auto PID

```cpp
#include <TwoWDController.h>

// For PCF8574 I2C Mode
TwoWDController robot(
  0x20,          // I2C address
  15, 2,         // left enable, right enable
  P2, P3,        // Left motor IN1, IN2
  P0, P1,        // Right motor IN1, IN2
  18, 19, 12, 13  // encoders: leftA, leftB, rightA, rightB
);

void setup() {
  Serial.begin(115200);
  robot.begin();
  robot.resetPID();
  
  // Start automatic PID updates every 33ms
  robot.startAutoUpdate(0.033);
}

void loop() {
  // Move forward at 0.2 m/s
  robot.drive(0.2, 0.2);
  delay(3000);
  
  // Turn left
  robot.drive(-0.1, 0.1);
  delay(2000);
  
  // Stop
  robot.stop();
  delay(2000);
}
```

### Example 2: Percentage Speed Control

```cpp
#include <TwoWDController.h>

TwoWDController robot(
  0x20, 15, 2, 
  P2, P3,
  P0, P1,
  18, 19, 12, 13
);

void setup() {
  robot.begin();
  robot.startAutoUpdate();
  
  // Move at 75% speed forward
  robot.driveSpeed(75, 75);
  delay(2000);
  
  // Turn right at 50% speed
  robot.driveSpeed(50, -50);
  delay(1000);
  
  robot.stop();
}

void loop() {}
```

## API Reference

### Constructor

#### I2C Mode (PCF8574)
```cpp
TwoWDController(uint8_t i2cAddress,
                int leftEnable, int rightEnable,
                int leftIn1, int leftIn2, int rightIn1, int rightIn2,
                int leftEncA, int leftEncB, int rightEncA, int rightEncB)
```

### Core Methods

| Method | Description |
|--------|-------------|
| `void begin()` | Initialize the controller and hardware |
| `void startAutoUpdate(float intervalSeconds = 0.033)` | Start automatic PID updates using Ticker |
| `void stopAutoUpdate()` | Stop automatic PID updates |
| `void drive(double leftSpeed, double rightSpeed)` | Set speed in meters per second |
| `void driveSpeed(int leftPercent, int rightPercent)` | Set speed as percentage (-100 to 100) |
| `void stop()` | Stop both motors |
| `void updatePID()` | Manually update PID (if not using auto-update) |
| `void resetPID()` | Reset PID parameters and encoders |
| `void resetEncoders()` | Reset encoder positions to zero |

### Monitoring Methods

| Method | Description |
|--------|-------------|
| `long readEncoder(int motor)` | Read encoder position (0=left, 1=right) |
| `double getLeftSpeed()` | Get left motor speed (m/s) |
| `double getRightSpeed()` | Get right motor speed (m/s) |
| `int getLeftOutput()` | Get left motor PWM output (-255 to 255) |
| `int getRightOutput()` | Get right motor PWM output (-255 to 255) |
| `long getLeftPosition()` | Get left encoder position |
| `long getRightPosition()` | Get right encoder position |
| `double getSpeedRPM(int motor)` | Get motor speed in RPM (0=left, 1=right) |

### Configuration Methods

| Method | Description |
|--------|-------------|
| `void setPIDConstants(int Kp, int Kd, int Ki, int Ko)` | Set PID constants |
| `void getPIDConstants(int &Kp, int &Kd, int &Ki, int &Ko)` | Get current PID constants |
| `void setRobotParameters(double wheelDiameter, int encoderResolution, double pidInterval, int gearReduction)` | Configure robot physical parameters |

### Pre-defined Constants

```cpp
// Default Robot Parameters
TwoWDController::DEFAULT_WHEEL_DIAMETER      // 0.067 meters
TwoWDController::DEFAULT_ENCODER_RESOLUTION  // 3950 ticks/rev
TwoWDController::DEFAULT_PID_INTERVAL        // 0.033 seconds
TwoWDController::DEFAULT_GEAR_REDUCTION      // 1

// Default PID Constants
TwoWDController::DEFAULT_Kp  // 20
TwoWDController::DEFAULT_Kd  // 12
TwoWDController::DEFAULT_Ki  // 0
TwoWDController::DEFAULT_Ko  // 50
TwoWDController::MAX_PWM     // 255
```

## Advanced Examples

### PID Tuning Example

```cpp
#include <TwoWDController.h>

TwoWDController robot(0x20, 15, 2, 
                      P2, P3,
                      P0, P1,
                      18, 19, 12, 13);

unsigned long lastPrint = 0;

void setup() {
  Serial.begin(115200);
  robot.begin();
  robot.startAutoUpdate();
  
  // Set custom PID constants for tuning
  robot.setPIDConstants(25, 15, 1, 50);
  robot.drive(0.2, 0.2);  // Target 0.2 m/s
}

void loop() {
  if (millis() - lastPrint > 500) {
    lastPrint = millis();
    
    Serial.print("Left: ");
    Serial.print(robot.getLeftSpeed(), 3);
    Serial.print(" m/s | PWM: ");
    Serial.print(robot.getLeftOutput());
    Serial.print(" | Pos: ");
    Serial.print(robot.getLeftPosition());
    
    Serial.print("  Right: ");
    Serial.print(robot.getRightSpeed(), 3);
    Serial.print(" m/s | PWM: ");
    Serial.print(robot.getRightOutput());
    Serial.print(" | Pos: ");
    Serial.println(robot.getRightPosition());
  }
}
```

### Manual PID Update (Without Ticker)

```cpp
#include <TwoWDController.h>

TwoWDController robot(0x20, 15, 2, 
                      P2, P3,
                      P0, P1,
                      18, 19, 12, 13);

unsigned long lastPIDUpdate = 0;

void setup() {
  robot.begin();
  robot.drive(0.2, 0.2);
}

void loop() {
  // Manually update PID every 33ms
  if (millis() - lastPIDUpdate >= 33) {
    lastPIDUpdate = millis();
    robot.updatePID();
  }
  
  // Other tasks can run here without interruption
}
```

## Troubleshooting

### Common Issues and Solutions

**Issue:** PCF8574 not initializing
```
ERROR: Could not initialize PCF8574! Check wiring, I2C address, SDA/SCL and power.
```
**Solution:** 
- Check I2C connections (SDA, SCL)
- Verify PCF8574 address (use I2C scanner sketch)
- Ensure you called `pinMode()` and `digitalWrite()` BEFORE `begin()`

**Issue:** Motors not responding
**Solution:**
- Check motor driver power supply
- Verify pin mappings in constructor
- Test motor driver with simple sketch first

**Issue:** Encoder readings are erratic
**Solution:**
- Add pull-up resistors (10kΩ) on encoder lines
- Check for loose connections
- Verify encoder pin assignments

**Issue:** PID auto-update not working
**Solution:**
- Ensure `startAutoUpdate()` is called after `begin()`
- Check that `loop()` is running (not blocked by delays)
- Try manual update mode as alternative

## Performance Notes

- **PID Update Interval:** Default 33ms (~30Hz) works well for most robots
- **Ticker Usage:** The Ticker runs in interrupt context; keep `updatePID()` fast
- **I2C Speed:** Standard 100kHz is sufficient; can increase to 400kHz
- **Encoder ISRs:** Interrupts are efficient; keep ISR code minimal

## License

This library is released under the MIT License. See the [LICENSE](LICENSE) file for more details.

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Submit a pull request

## Support

- **Issues:** [GitHub Issues](https://github.com/kingdiaw/TwoWDController/issues)
- **Email:** kingdiawehsut@gmail.com

## Acknowledgements

- [PCF8574_library](https://github.com/xreef/PCF8574_library) by Renzo Mischianti
- ESP32 community for excellent documentation

## Version History

### v1.0.0
- Initial release
- Support for GPIO and PCF8574 modes
- PID speed control with auto-update via Ticker
- Encoder position tracking
- Complete API documentation

---

**Happy Roboting!** 🤖
```

This README provides:

1. **Clear installation instructions** for all three methods
2. **Important notes** about dependencies and configuration
3. **Wiring diagrams** for both connection methods
4. **Comprehensive API reference** with all methods
5. **Practical examples** from basic to advanced
6. **Troubleshooting guide** for common issues
7. **Performance notes** to help users optimize their robots

The README follows standard Arduino library documentation practices and should be placed in the root of your library folder alongside the `library.properties` file.# TwoWDController
