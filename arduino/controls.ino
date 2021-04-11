#include <Servo.h>

struct servoPair {
  Servo left;
  Servo right;
} wingServos;

// Tweak these --------------------
const float maxFrequency = 5;
const int zeroThrottleAngle = 15;
const int minLevelAngle = -30;
const int maxLevelAngle = 30;
const int maxRollPhaseShift = 30;
const int maxPitchAngle = 10;
const int maxYawAngle = 10;
// --------------------------------


// Don't tweak these --------------
float throttle;
float roll;
float pitch;
float yaw;

int startTime;
const int delayTime = 15;
int elapsedTime;

float levelTheta;
float leftTheta;
float rightTheta;

struct pair {
  int left;
  int right;
} wings;
// --------------------------------


void updatePosition(float throttle, float roll, float pitch, float yaw) {
  if (throttle == 0) {
    wings.left = zeroThrottleAngle;
    wings.right = zeroThrottleAngle;

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
  Serial.begin(9600);

  wingServos.left.attach(2);
  wingServos.right.attach(3);
  
  wingServos.left.write(90);
  wingServos.right.write(90);

  delay(1000);

  startTime = millis();

  // Arbitrary values
  throttle = 1;
  roll = -0.5;
  pitch = 0.6;
  yaw = 0;
}


void loop() {
  throttle = min(1, (float) (millis() - startTime) / 10000);
  
  updatePosition(throttle, roll, pitch, yaw);

  wingServos.left.write(90 - wings.left);
  wingServos.right.write(90 + wings.right);

  delay(delayTime);
}
