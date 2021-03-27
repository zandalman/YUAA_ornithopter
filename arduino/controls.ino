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
  }
  
  float deltaTheta = throttle * maxFrequency * ((float) delayTime / 1000) * 2 * PI;
  levelTheta += deltaTheta;

  // left if roll < 0; level if roll == 0; roll if pitch > 0
  if (pitch < 0) {
    leftTheta = levelTheta + roll * maxRollPhaseShift * PI / 180;
    rightTheta = levelTheta;
  } else if (pitch == 0) {
    leftTheta = levelTheta;
    rightTheta = levelTheta;
  } else {
    leftTheta = levelTheta;
    rightTheta = levelTheta - roll * maxRollPhaseShift * PI / 180;
  }

  float amplitude = (float) (maxLevelAngle - minLevelAngle) / 2;

  wings.left = amplitude * sin(leftTheta) - pitch * maxPitchAngle + yaw * maxYawAngle;
  wings.right = amplitude * sin(rightTheta) - pitch * maxPitchAngle - yaw * maxYawAngle;
}


void setup() {
  Serial.begin(9600);
  startTime = millis();

  levelTheta = 0;

  // Arbitrary values
  throttle = 0.5;
  roll = -0.34;
  pitch = 0.75;
  yaw = -0.6;
}


void loop() {
  updatePosition(throttle, roll, pitch, yaw);
  
  delay(delayTime);
}
