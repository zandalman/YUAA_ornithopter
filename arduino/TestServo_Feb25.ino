#include <Servo.h>

float amplitude = 20; // degrees
float frequency = 2; //Hz

Servo wing;

int startTime = 0;

void setup() {
  Serial.begin(115200);
  startTime = millis();
  wing.attach(2);  // change me!
}

void loop() {
  int nextPos = amplitude * sin(frequency * 2*PI * (millis() - startTime)/1000) + 90;
  Serial.print((millis() - startTime)/1000);
  Serial.print(", ");
  Serial.println(nextPos);
  wing.write(nextPos);
  delay(15);
}
