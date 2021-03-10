#include <Servo.h>

float amplitude = 20; // degrees
float frequency = 2; // frequency in Hz
int servoPin = 12; // Servo pin number

Servo wing;

int startTime = 0;

// To-do for Kai
// Add nextPos function
// Inputs are throttle (0-1), roll (ms), pitch (degrees), and yaw (degrees)
// Outputs are wing positions (degrees)

void setup() {
  Serial.begin(9600);
  startTime = millis();
  wing.attach(servoPin);
}

void loop() {
  int nextPos = amplitude * sin(frequency * 2*PI * (millis() - startTime)/1000) + 90; // Replace with function
  Serial.print((millis() - startTime)/1000);
  Serial.print(", ");
  Serial.println(nextPos);
  wing.write(nextPos);
  delay(15);
}
