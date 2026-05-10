#ifndef TwoWDController_h
#define TwoWDController_h

#include <Arduino.h>
#include <PCF8574.h>
#include <Ticker.h>

// PID Control Structure
struct SetPointInfo {
  double TargetTicksPerFrame;
  long Encoder;
  long PrevEnc;
  int PrevInput;
  int ITerm;
  long output;
};

class TwoWDController {
public:
  enum ControlMode { MODE_ESP32_GPIO, MODE_PCF8574_IO };
  
  // Constructor for ESP32 GPIO control
  TwoWDController(
    int leftEnable, int leftIn1, int leftIn2,
    int rightEnable, int rightIn1, int rightIn2,
    int leftEncA, int leftEncB, int rightEncA, int rightEncB
  );
  
  // Constructor for PCF8574 control
  TwoWDController(
    uint8_t i2cAddress,
    int leftEnable, int rightEnable,
    int leftIn1, int leftIn2, int rightIn1, int rightIn2,
    int leftEncA, int leftEncB, int rightEncA, int rightEncB
  );
  
  ~TwoWDController();
  
  void begin();
  void startAutoUpdate(float intervalSeconds = 0.033);
  void stopAutoUpdate();
  void drive(double leftSpeed, double rightSpeed);
  void driveSpeed(int leftPercent, int rightPercent);
  void stop();
  void updatePID();
  void resetPID();
  void resetEncoders();
  long readEncoder(int motor);
  double getLeftSpeed();
  double getRightSpeed();
  int getLeftOutput() const;
  int getRightOutput() const;
  long getLeftPosition() const;
  long getRightPosition() const;
  double getTargetTicksPerFrame(int motor) const;
  double getSpeedRPM(int motor);
  void setPIDConstants(int Kp, int Kd, int Ki, int Ko);
  void getPIDConstants(int &Kp, int &Kd, int &Ki, int &Ko) const;
  void setRobotParameters(double wheelDiameter, int encoderResolution, double pidInterval, int gearReduction);
  bool isAutoUpdateRunning() const { return autoUpdateEnabled; }
  
  // Robot parameters
  static constexpr double DEFAULT_WHEEL_DIAMETER = 0.067;
  static constexpr int DEFAULT_ENCODER_RESOLUTION = 3950;
  static constexpr double DEFAULT_PID_INTERVAL = 0.033;
  static constexpr int DEFAULT_GEAR_REDUCTION = 1;
  
  // PID parameters
  static constexpr int DEFAULT_Kp = 20;
  static constexpr int DEFAULT_Kd = 12;
  static constexpr int DEFAULT_Ki = 0;
  static constexpr int DEFAULT_Ko = 50;
  static constexpr int MAX_PWM = 255;
  
  // Note: P0-P7 constants are already defined in PCF8574.h
  // Use them directly from the PCF8574 library

private:
  ControlMode mode;
  int leftEnablePin, leftIn1Pin, leftIn2Pin;
  int rightEnablePin, rightIn1Pin, rightIn2Pin;
  int leftEncPinA, leftEncPinB;
  int rightEncPinA, rightEncPinB;
  uint8_t i2cAddress;
  PCF8574 *pcf8574;
  bool pcf8574Initialized;
  
  volatile long leftEncPos;
  volatile long rightEncPos;
  volatile uint8_t leftEncLast;
  volatile uint8_t rightEncLast;
  
  SetPointInfo leftPID;
  SetPointInfo rightPID;
  
  int Kp, Kd, Ki, Ko;
  double wheelDiameter;
  int encoderResolution;
  double pidInterval;
  int gearReduction;
  
  Ticker pidTicker;
  bool autoUpdateEnabled;
  
  void setupEncoders();
  void computePID(SetPointInfo &pid);
  double calculateSpeed(long encoderTicks);
  void setMotorSpeeds(int leftSpeed, int rightSpeed);
  void initPCF8574();
  
  static void updatePIDWrapper(TwoWDController* instance);
  
  friend void IRAM_ATTR leftEncoderISRWrapper();
  friend void IRAM_ATTR rightEncoderISRWrapper();
  
  static TwoWDController* instance;
  static const int8_t ENC_STATES[16];
};

#endif