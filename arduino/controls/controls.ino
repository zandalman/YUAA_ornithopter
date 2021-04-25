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
int LEFT_HORIZONTAL = 4;
int FLAPPING_AMPLITUDE = 5;

// User defined values
const float maxFrequency = 7; // Fastest flapping frequency
const int zeroThrottleAngle = 15; // Gliding angle when throttle is turned down
const int levelAngle = 0; // Angle at middle of flapping cycle
const int maxAmplitude = 45; // Maximum amplitude of flapping cycle
const int maxRollPhaseShift = 30; // Maximum phase shift in degrees when roll is at max
const int maxPitchAngle = 10; // Maximum change in angle when pitch is at max
const int maxYawAngle = 10; // Maximum change in angle when yaw is at max
const int deadzone = 50; // Range in which a control registers as a default value
const int delayTime = 5; // How fast in ms the loop cycles

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

void updatePosition(float throttle, float roll, float pitch, float yaw, float amplitude) {
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

  wings.left = levelAngle + amplitude * sin(leftTheta) + pitch * maxPitchAngle + yaw * maxYawAngle;
  wings.right = levelAngle + amplitude * sin(rightTheta) + pitch * maxPitchAngle - yaw * maxYawAngle;
}

void toggleControls(int state) {
  if (state == 0) {
    LEFT_HORIZONTAL = 4;
    FLAPPING_AMPLITUDE = 5;
  } else {
    LEFT_HORIZONTAL = 5;
    FLAPPING_AMPLITUDE = 4;
  }
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
