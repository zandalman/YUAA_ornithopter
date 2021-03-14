#include <Servo.h>

const int maxFrequency = 5;
const int minLevelAngle = -10;
const int maxLevelAngle = 10;
const int rollPhaseShift = 45;
const int minPitchAngle = -5;
const int maxPitchAngle = 5;
const int yawAngle = 5;

int startTime;

struct {int left, right;} wings = {0, 0};

void updatePosition(float elapsedTime, float throttle, float roll, float pitch, float yaw) {
  int levelAmplitude = maxLevelAngle - minLevelAngle;
  int zeroOffset = 90 + minPitchAngle;
  float frequency = throttle * maxFrequency;
  float normalizedTime = elapsedTime / 1000;
  float phaseShift = roll * rollPhaseShift / 720;
  float pitchTerm = maxPitchAngle - pitch * (maxPitchAngle - minPitchAngle);
  float yawTerm = yaw * yawAngle;

  wings.left = levelAmplitude * sin(frequency * 2 * PI * (elapsedTime + phaseShift)) + zeroOffset + pitchTerm + yawTerm;
  wings.right = levelAmplitude * sin(frequency * 2 * PI * (elapsedTime - phaseShift)) + zeroOffset + pitchTerm - yawTerm;
}

void setup() {
  Serial.begin(9600);
  startTime = millis();
//  wing.attach(servoPin);
}

void loop() {
  updatePosition((millis() - startTime) / 1000, 0.5, 0.25, 0.75, 0.25);
  Serial.print(wings.left);
  Serial.print(", ");
  Serial.print(wings.right);
  Serial.print("\n");
  delay(15);
}
