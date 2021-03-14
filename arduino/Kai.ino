#include <Servo.h>

const int maxFrequency = 5;
const int minLevelAngle = -10;
const int maxLevelAngle 10;
const int rollPhaseShift = 45;
const int minPitchAngle = -5;
const int maxPitchAngle = 5;
const int yawAngle = 5;

int startTime;

int nextPos(int elapsedTime, float throttle, float roll, float pitch, float yaw) {
  int levelAmplitude = maxLevelAngle - minLevelAngle;
  int zeroOffset = 90 + minPitchAngle;
  float frequency = throttle * maxFrequency;
  float normalizedTime = elapsedTime / 1000;
  int phaseShift = roll * rollPhaseShift / 720;
  int pitchTerm = maxPitchAngle - pitch * (maxPitchAngle - minPitchAngle)
  int yawTerm = yaw * yawAngle;

  int leftPos = levelAmplitude * sin(frequency * 2*PI * (normalizedTime + phaseShift)) + zeroOffset + pitchTerm + yawTerm;
  int rightPos = levelAmplitude * sin(frequency * 2*PI * (normalizedTime - phaseShift)) + zeroOffset + pitchTerm - yawTerm;
}

void setup() {
  Serial.begin(9600);
  startTime = millis();
  wing.attach(servoPin);
}

// void loop() {
//   int nextPos = amplitude * sin(frequency * 2*PI * (millis() - startTime)/1000) + 90; // replace with function
// //  Serial.print((millis() - startTime)/1000);
// //  Serial.print(", ");
// //  Serial.println(nextPos);
// //  wing.write(nextPos);
// //  delay(15);
// }
