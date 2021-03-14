#include <Servo.h>

const int maxFrequency = 5;
const int minLevelAngle = -10;
const int maxLevelAngle 10;
const int rollPhaseShift = 45;
const int minPitchAngle = -5;
const int maxPitchAngle = 5;
const int yawAngle = 5;

int startTime;

int leftPos(int elapsedTime, float throttle, float roll, float pitch, float yaw) {
  int levelAmplitude = maxLevelAngle - minLevelAngle;
  int zeroOffset = 90 + minPitchAngle;
  float frequency = throttle * maxFrequency;
  float normalizedTime = elapsedTime / 1000;
  float phaseShift = roll * rollPhaseShift / 720;
  float pitchTerm = maxPitchAngle - pitch * (maxPitchAngle - minPitchAngle)
  float yawTerm = yaw * yawAngle;

  int leftPos = levelAmplitude * sin(frequency * 2*PI * (normalizedTime + phaseShift)) + zeroOffset + pitchTerm + yawTerm;
  return leftPos;
}

int rightPos(int elapsedTime, float throttle, float roll, float pitch, float yaw) {
  int levelAmplitude = maxLevelAngle - minLevelAngle;
  int zeroOffset = 90 + minPitchAngle;
  float frequency = throttle * maxFrequency;
  float normalizedTime = elapsedTime / 1000;
  float phaseShift = roll * rollPhaseShift / 720;
  float pitchTerm = maxPitchAngle - pitch * (maxPitchAngle - minPitchAngle)
  float yawTerm = yaw * yawAngle;
  
  int rightPos = levelAmplitude * sin(frequency * maxFrequency * 2*PI * (normalizedTime - phaseShift)) + zeroOffset + pitchTerm - yawTerm;
  return rightPos;
}
