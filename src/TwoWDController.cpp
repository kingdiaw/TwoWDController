#include "TwoWDController.h"

TwoWDController* TwoWDController::instance = nullptr;
const int8_t TwoWDController::ENC_STATES[16] = {
  0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0
};

void IRAM_ATTR leftEncoderISRWrapper() {
  if (TwoWDController::instance) {
    TwoWDController::instance->leftEncLast <<= 2;
    TwoWDController::instance->leftEncLast |= ((digitalRead(TwoWDController::instance->leftEncPinA) << 1) | 
                                                  digitalRead(TwoWDController::instance->leftEncPinB));
    TwoWDController::instance->leftEncPos += TwoWDController::ENC_STATES[(TwoWDController::instance->leftEncLast & 0x0F)];
  }
}

void IRAM_ATTR rightEncoderISRWrapper() {
  if (TwoWDController::instance) {
    TwoWDController::instance->rightEncLast <<= 2;
    TwoWDController::instance->rightEncLast |= ((digitalRead(TwoWDController::instance->rightEncPinA) << 1) | 
                                                   digitalRead(TwoWDController::instance->rightEncPinB));
    TwoWDController::instance->rightEncPos += TwoWDController::ENC_STATES[(TwoWDController::instance->rightEncLast & 0x0F)];
  }
}

void TwoWDController::updatePIDWrapper(TwoWDController* instance) {
  if (instance) instance->updatePID();
}

TwoWDController::TwoWDController(
  int leftEnable, int leftIn1, int leftIn2,
  int rightEnable, int rightIn1, int rightIn2,
  int leftEncA, int leftEncB, int rightEncA, int rightEncB
) : mode(MODE_ESP32_GPIO),
    leftEnablePin(leftEnable), leftIn1Pin(leftIn1), leftIn2Pin(leftIn2),
    rightEnablePin(rightEnable), rightIn1Pin(rightIn1), rightIn2Pin(rightIn2),
    leftEncPinA(leftEncA), leftEncPinB(leftEncB),
    rightEncPinA(rightEncA), rightEncPinB(rightEncB),
    pcf8574(nullptr), pcf8574Initialized(false),
    leftEncPos(0), rightEncPos(0), leftEncLast(0), rightEncLast(0),
    Kp(DEFAULT_Kp), Kd(DEFAULT_Kd), Ki(DEFAULT_Ki), Ko(DEFAULT_Ko),
    wheelDiameter(DEFAULT_WHEEL_DIAMETER),
    encoderResolution(DEFAULT_ENCODER_RESOLUTION),
    pidInterval(DEFAULT_PID_INTERVAL),
    gearReduction(DEFAULT_GEAR_REDUCTION),
    autoUpdateEnabled(false) {
  leftPID = {0, 0, 0, 0, 0, 0};
  rightPID = {0, 0, 0, 0, 0, 0};
  instance = this;
}

TwoWDController::TwoWDController(
  uint8_t i2cAddr,
  int leftEnable, int rightEnable,
  int leftIn1, int leftIn2, int rightIn1, int rightIn2,
  int leftEncA, int leftEncB, int rightEncA, int rightEncB
) : mode(MODE_PCF8574_IO),
    leftEnablePin(leftEnable), rightEnablePin(rightEnable),
    leftIn1Pin(leftIn1), leftIn2Pin(leftIn2),
    rightIn1Pin(rightIn1), rightIn2Pin(rightIn2),
    leftEncPinA(leftEncA), leftEncPinB(leftEncB),
    rightEncPinA(rightEncA), rightEncPinB(rightEncB),
    i2cAddress(i2cAddr),
    pcf8574(nullptr), pcf8574Initialized(false),
    leftEncPos(0), rightEncPos(0), leftEncLast(0), rightEncLast(0),
    Kp(DEFAULT_Kp), Kd(DEFAULT_Kd), Ki(DEFAULT_Ki), Ko(DEFAULT_Ko),
    wheelDiameter(DEFAULT_WHEEL_DIAMETER),
    encoderResolution(DEFAULT_ENCODER_RESOLUTION),
    pidInterval(DEFAULT_PID_INTERVAL),
    gearReduction(DEFAULT_GEAR_REDUCTION),
    autoUpdateEnabled(false) {
  pcf8574 = new PCF8574(i2cAddr);
  leftPID = {0, 0, 0, 0, 0, 0};
  rightPID = {0, 0, 0, 0, 0, 0};
  instance = this;
}

TwoWDController::~TwoWDController() {
  stopAutoUpdate();
  if (pcf8574) delete pcf8574;
  if (instance == this) instance = nullptr;
}

void TwoWDController::initPCF8574() {
  if (!pcf8574 || pcf8574Initialized) return;
  
  pcf8574->pinMode(leftIn1Pin, OUTPUT);
  pcf8574->pinMode(leftIn2Pin, OUTPUT);
  pcf8574->digitalWrite(leftIn1Pin, LOW);
  pcf8574->digitalWrite(leftIn2Pin, LOW);
  
  pcf8574->pinMode(rightIn1Pin, OUTPUT);
  pcf8574->pinMode(rightIn2Pin, OUTPUT);
  pcf8574->digitalWrite(rightIn1Pin, LOW);
  pcf8574->digitalWrite(rightIn2Pin, LOW);
  
  if (!pcf8574->begin()) {
    Serial.println(F("ERROR: Could not initialize PCF8574!"));
    while (1) delay(100);
  }
  pcf8574Initialized = true;
}

void TwoWDController::begin() {
  if (mode == MODE_ESP32_GPIO) {
    pinMode(leftEnablePin, OUTPUT);
    pinMode(leftIn1Pin, OUTPUT);
    pinMode(leftIn2Pin, OUTPUT);
    pinMode(rightEnablePin, OUTPUT);
    pinMode(rightIn1Pin, OUTPUT);
    pinMode(rightIn2Pin, OUTPUT);
  } else {
    initPCF8574();
  }
  setupEncoders();
}

void TwoWDController::startAutoUpdate(float intervalSeconds) {
  if (!autoUpdateEnabled) {
    pidTicker.attach(intervalSeconds, updatePIDWrapper, this);
    autoUpdateEnabled = true;
  }
}

void TwoWDController::stopAutoUpdate() {
  if (autoUpdateEnabled) {
    pidTicker.detach();
    autoUpdateEnabled = false;
  }
}

void TwoWDController::setupEncoders() {
  pinMode(leftEncPinA, INPUT_PULLUP);
  pinMode(leftEncPinB, INPUT_PULLUP);
  pinMode(rightEncPinA, INPUT_PULLUP);
  pinMode(rightEncPinB, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(leftEncPinA), leftEncoderISRWrapper, CHANGE);
  attachInterrupt(digitalPinToInterrupt(leftEncPinB), leftEncoderISRWrapper, CHANGE);
  attachInterrupt(digitalPinToInterrupt(rightEncPinA), rightEncoderISRWrapper, CHANGE);
  attachInterrupt(digitalPinToInterrupt(rightEncPinB), rightEncoderISRWrapper, CHANGE);
}

void TwoWDController::drive(double leftSpeed, double rightSpeed) {
  double leftRevsPerSecond = leftSpeed / (wheelDiameter * PI);
  double rightRevsPerSecond = rightSpeed / (wheelDiameter * PI);
  leftPID.TargetTicksPerFrame = leftRevsPerSecond * encoderResolution * pidInterval * gearReduction;
  rightPID.TargetTicksPerFrame = rightRevsPerSecond * encoderResolution * pidInterval * gearReduction;
}

void TwoWDController::driveSpeed(int leftPercent, int rightPercent) {
  int leftPWM = constrain(leftPercent, -100, 100) * 255 / 100;
  int rightPWM = constrain(rightPercent, -100, 100) * 255 / 100;
  setMotorSpeeds(leftPWM, rightPWM);
}

void TwoWDController::stop() {
  setMotorSpeeds(0, 0);
  leftPID.TargetTicksPerFrame = 0;
  rightPID.TargetTicksPerFrame = 0;
}

void TwoWDController::updatePID() {
  leftPID.Encoder = leftEncPos;
  rightPID.Encoder = rightEncPos;
  computePID(leftPID);
  computePID(rightPID);
  setMotorSpeeds(leftPID.output, rightPID.output);
}

void TwoWDController::computePID(SetPointInfo &pid) {
  int input = pid.Encoder - pid.PrevEnc;
  long Perror = pid.TargetTicksPerFrame - input;
  long computedOutput = (Kp * Perror - Kd * (input - pid.PrevInput) + pid.ITerm) / Ko;
  pid.PrevEnc = pid.Encoder;
  computedOutput += pid.output;
  if (computedOutput >= MAX_PWM) computedOutput = MAX_PWM;
  else if (computedOutput <= -MAX_PWM) computedOutput = -MAX_PWM;
  else pid.ITerm += Ki * Perror;
  pid.output = computedOutput;
  pid.PrevInput = input;
}

void TwoWDController::setMotorSpeeds(int leftSpeed, int rightSpeed) {
  int leftPWM = constrain(abs(leftSpeed), 0, 255);
  int rightPWM = constrain(abs(rightSpeed), 0, 255);
  
  if (mode == MODE_ESP32_GPIO) {
    digitalWrite(leftIn1Pin, leftSpeed >= 0 ? HIGH : LOW);
    digitalWrite(leftIn2Pin, leftSpeed >= 0 ? LOW : HIGH);
    analogWrite(leftEnablePin, leftPWM);
    digitalWrite(rightIn1Pin, rightSpeed >= 0 ? HIGH : LOW);
    digitalWrite(rightIn2Pin, rightSpeed >= 0 ? LOW : HIGH);
    analogWrite(rightEnablePin, rightPWM);
  } else {
    if (!pcf8574Initialized) initPCF8574();
    pcf8574->digitalWrite(leftIn1Pin, leftSpeed >= 0 ? LOW : HIGH);
    pcf8574->digitalWrite(leftIn2Pin, leftSpeed >= 0 ? HIGH : LOW);
    analogWrite(leftEnablePin, leftPWM);
    pcf8574->digitalWrite(rightIn1Pin, rightSpeed >= 0 ? LOW : HIGH);
    pcf8574->digitalWrite(rightIn2Pin, rightSpeed >= 0 ? HIGH : LOW);
    analogWrite(rightEnablePin, rightPWM);
  }
}

void TwoWDController::resetPID() {
  resetEncoders();
  leftPID = {0, 0, 0, 0, 0, 0};
  rightPID = {0, 0, 0, 0, 0, 0};
}

void TwoWDController::resetEncoders() {
  leftEncPos = 0;
  rightEncPos = 0;
}

long TwoWDController::readEncoder(int motor) {
  return motor == 0 ? leftEncPos : rightEncPos;
}

double TwoWDController::calculateSpeed(long encoderTicks) {
  double wheelCircumference = wheelDiameter * PI;
  double distancePerTick = wheelCircumference / encoderResolution;
  return (encoderTicks * distancePerTick) / pidInterval;
}

double TwoWDController::getLeftSpeed() { return calculateSpeed(leftPID.PrevInput); }
double TwoWDController::getRightSpeed() { return calculateSpeed(rightPID.PrevInput); }
int TwoWDController::getLeftOutput() const { return leftPID.output; }
int TwoWDController::getRightOutput() const { return rightPID.output; }
long TwoWDController::getLeftPosition() const { return leftEncPos; }
long TwoWDController::getRightPosition() const { return rightEncPos; }
double TwoWDController::getTargetTicksPerFrame(int motor) const { return motor == 0 ? leftPID.TargetTicksPerFrame : rightPID.TargetTicksPerFrame; }
double TwoWDController::getSpeedRPM(int motor) { 
  double encPrevInput = motor == 0 ? leftPID.PrevInput : rightPID.PrevInput;
  return (encPrevInput / (encoderResolution * pidInterval) * 60);
}
void TwoWDController::setPIDConstants(int newKp, int newKd, int newKi, int newKo) { Kp = newKp; Kd = newKd; Ki = newKi; Ko = newKo; }
void TwoWDController::getPIDConstants(int &currentKp, int &currentKd, int &currentKi, int &currentKo) const { currentKp = Kp; currentKd = Kd; currentKi = Ki; currentKo = Ko; }
void TwoWDController::setRobotParameters(double newWheelDiameter, int newEncoderResolution, double newPidInterval, int newGearReduction) {
  wheelDiameter = newWheelDiameter; encoderResolution = newEncoderResolution; pidInterval = newPidInterval; gearReduction = newGearReduction;
}