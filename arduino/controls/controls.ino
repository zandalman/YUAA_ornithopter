#include <Servo.h>
#include <PulsePosition.h>

struct servoPair {
  Servo left;
  Servo right;
} wingServos;

// Define pins
int leftServoPin = 3;
int rightServoPin = 4;
int rxPin = 9;

// Initialize RC input
PulsePositionInput RCInput;

// RC controller channel mapping
const int RIGHT_VERTICAL = 1;
const int RIGHT_HORIZONTAL = 2;
const int LEFT_VERTICAL = 3;
const int LEFT_HORIZONTAL = 4;

// User defined values
const float maxFrequency = 7;
const int zeroThrottleAngle = 15;
const int minLevelAngle = -30;
const int maxLevelAngle = 30;
const int maxRollPhaseShift = 30;
const int maxPitchAngle = 10;
const int maxYawAngle = 10;
const int deadzone = 50;
const int delayTime = 5;

// Initialize variables
float throttle;
float roll;
float pitch;
float yaw;

int startTime;
int elapsedTime;

float levelTheta;
float leftTheta;
float rightTheta;

struct pair {
  int left;
  int right;
} wings;

void updatePosition(float throttle, float roll, float pitch, float yaw) {
  if (throttle == 0) {
    wings.left = zeroThrottleAngle + pitch * maxPitchAngle + yaw * maxYawAngle;
    wings.right = zeroThrottleAngle + pitch * maxPitchAngle - yaw * maxYawAngle;
    return;
  }

  float deltaTheta = throttle * maxFrequency * ((float) delayTime / 1000) * 2 * PI;
  levelTheta += deltaTheta;

  if (roll < 0) {
    leftTheta = levelTheta;
    rightTheta = levelTheta + roll * maxRollPhaseShift * PI / 180;
  } else {
    leftTheta = levelTheta - roll * maxRollPhaseShift * PI / 180;
    rightTheta = levelTheta;
  }

  float amplitude = (float) (maxLevelAngle - minLevelAngle) / 2;

  wings.left = amplitude * sin(leftTheta) + pitch * maxPitchAngle + yaw * maxYawAngle;
  wings.right = amplitude * sin(rightTheta) + pitch * maxPitchAngle - yaw * maxYawAngle;
}


void setup() {
  // Begin serial
  Serial.begin(9600);
  // Begin reading RC input
  RCInput.begin(rxPin);
  
  wingServos.left.attach(leftServoPin);
  wingServos.right.attach(rightServoPin);
  
  wingServos.left.write(90);
  wingServos.right.write(90);

  delay(1000);
  // Start clock
  startTime = millis();
}

// Normalize throttle readings
float normalize_throttle (float value) {
  if (value < 1000 + deadzone) {
    return 0;
  } else {
    return (value - (1000 + deadzone)) / (1000 - deadzone);
  }
}

// Normalizr steering readings
float normalize_steering (float value) {
  if (value < 1500 + deadzone && value > 1500 - deadzone) {
    return 0;
  } else if (value < 1500) {
    return (value - 1500 + deadzone) / (500 - deadzone);
  } else {
    return (value - 1500 - deadzone) / (500 - deadzone);
  }
}


void loop() {
  // Read RC input
  float yaw = normalize_steering(RCInput.read(RIGHT_VERTICAL));
  float pitch = normalize_steering(RCInput.read(RIGHT_HORIZONTAL));
  float throttle = normalize_throttle(RCInput.read(LEFT_VERTICAL));
  float roll = normalize_steering(RCInput.read(LEFT_HORIZONTAL));
  
  updatePosition(throttle, roll, pitch, yaw);

  wingServos.left.write(90 - wings.left);
  wingServos.right.write(90 + wings.right);

  delay(delayTime);
}
