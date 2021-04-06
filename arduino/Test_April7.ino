#include <Servo.h>
#include <PPMReader.h>
#define EPSILON 0.0001

//Don't forget to install PPMReader!

float amplitude = 20; // degrees
float frequency = 2; // frequency in Hz
int leftServoPin = 12; // Servo pin number
int rightServoPin = 12; // Servo pin number

Servo leftWing;
Servo rightWing;

int startTime = 0;

//RC stuff
int interruptPin = 3;
int channelAmount = 4;
PPMReader ppm(interruptPin, channelAmount);
// channels!
int rhChannel =  1;
int rvChannel = 2;
int lvChannel = 3;
int lhChannel = 4;

class RCController
{
  int values[4];
  int defaultValue = 1500;
  int maxValue = 2000;
  
  float deadzone = 20; //in raw sensor readings, ignore this many units on each side of neutral

  int throttleChannel = lvChannel;
  int rollChannel = rhChannel;
  int pitchChannel = rvChannel;
  int yawChannel = lhChannel;

  public:
  RCController(int interruptPin) {}

  void setup()
  {    
  }

  void read()
  {
    for (int channel = 0; channel < channelAmount; ++channel) {
        values[channel] = ppm.latestValidChannelValue(channel + 1, 0);
        Serial.print(String(values[channel]) + " ");
    }
    Serial.println();
  }

  float normalize(int joystickVal)
  {
    float deadzoneReading = (abs(joystickVal - defaultValue) < deadzone ? 1500 : joystickVal); //deadzone
    float normalizedReading = abs(deadzoneReading - defaultValue) / (maxValue - defaultValue); //convert to [0, 1]
    return normalizedReading;
  }

  float getThrottle() 
  {
    return normalize(values[throttleChannel]);
  }
  
  int getYaw()
  {
    return normalize(values[yawChannel]);
  }

  int getPitch()
  {
    return normalize(values[pitchChannel]);
  }

  int getRoll()
  {
    return normalize(values[rollChannel]);
  }
};

RCController rc(interruptPin);

void setup() {
  Serial.begin(9600);
  startTime = millis();
  leftWing.attach(leftServoPin);
  rightWing.attach(rightServoPin);
}

void loop() {
  if(rc.getThrottle() > EPSILON)
  {
   int nextPos = amplitude * sin(frequency * 2*PI * (millis() - startTime)/1000) + 90; // Replace with function
   Serial.print((millis() - startTime)/1000);
   Serial.print(", ");
   Serial.println(nextPos);
   leftWing.write(nextPos);
   rightWing.write(-nextPos);
  }
  delay(15);
}
