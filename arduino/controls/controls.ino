#include <Servo.h>
#include <PulsePosition.h>

struct servoPair {
  Servo left;
  Servo right;
} wingServos;

// Define pins
int leftServoPin = 4;
int rightServoPin = 3;
int rxPin = 9;

// Initialize RC input
PulsePositionInput RCInput;

// RC controller channel mapping
const int RIGHT_VERTICAL = 1;
const int RIGHT_HORIZONTAL = 2;
const int LEFT_VERTICAL = 3;
int LEFT_HORIZONTAL = 4;
int LEFT_DIAL = 5;
int RIGHT_DIAL = 6;
int LEFT_SWITCH = 7;
int RIGHT_SWITCH = 8;

// Define control modes
const int NORMAL_MODE = 1000;
const int BASIC_MODE = 1500;
const int AMP_MODE = 2000;

// User defined values
const float maxFrequency = 5; // Fastest flapping frequency
const int zeroThrottleAngle = 15; // Gliding angle when throttle is turned down
const int levelAngle = 0; // Angle at middle of flapping cycle
const int maxAmplitude = 45; // Maximum amplitude of flapping cycle
const int maxRollPhaseShift = 30; // Maximum phase shift in degrees when roll is at max
const int maxPitchAngle = 10; // Maximum change in angle when pitch is at max
const int maxYawAngle = 10; // Maximum change in angle when yaw is at max
const int deadzone = 50; // Range in which a control registers as a default value
const int delayTime = 5; // How fast in ms the loop cycles

// Initialize variables
int control_mode;
float throttle;
float roll;
float pitch;
float yaw;
float amp;

int startTime;
int elapsedTime;

float levelTheta;
float leftTheta;
float rightTheta;

struct pair {
  int left;
  int right;
} wings;

void updatePosition(float throttle, float roll, float pitch, float yaw, float amp) {
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

  wings.left = levelAngle + maxAmplitude * amp * sin(leftTheta) + pitch * maxPitchAngle + yaw * maxYawAngle;
  wings.right = levelAngle + maxAmplitude * amp * sin(rightTheta) + pitch * maxPitchAngle - yaw * maxYawAngle;
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
  control_mode = RCInput.read(RIGHT_SWITCH);
  yaw = normalize_steering(RCInput.read(RIGHT_VERTICAL));
  pitch = normalize_steering(RCInput.read(RIGHT_HORIZONTAL));
  throttle = normalize_throttle(RCInput.read(LEFT_VERTICAL));

  if (control_mode == NORMAL_MODE) {
    roll = normalize_steering(RCInput.read(LEFT_HORIZONTAL));
    amp = normalize_throttle(RCInput.read(LEFT_DIAL));
  } else if (control_mode == AMP_MODE) {
    roll = normalize_steering(RCInput.read(LEFT_DIAL));
    amp = normalize_throttle(RCInput.read(LEFT_HORIZONTAL));
  } else if (control_mode == BASIC_MODE) {
    roll = 0;
    amp = 0.6;
  }
  
  updatePosition(throttle, roll, pitch, yaw, amp);

  wingServos.left.write(90 - wings.left);
  wingServos.right.write(90 + wings.right);

  delay(delayTime);
}
