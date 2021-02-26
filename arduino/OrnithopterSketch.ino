#include <Servo.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#define BNO055_SAMPLERATE_DELAY_MS (100)
#define EPSILON (0.00001)

unsigned long sensorTimeout = 30 * 1000; //30 seconds before we give up on detecting the gyro
float deltat = 0.015; // in sec // used in loop, and for wing motion implemented w/ discrete timesteps

// pull these class definitions out into separate files?
class Wings
{
  Servo wings[2]; // left is 0, right is 1
  int pins[2];
  int positions[2]; //stores last command

  float amp = 45/2; //from zero to highest point
  int zeroPos = 90; //wings are parallel to ground at this angle
  int neutralPos = zeroPos + 15; //send this when throttle is zero
  
  int maxFreq = 7; //Hz

  // method of determining rising/falling is very iffy
  // if position was most recently within errorFromMax of maxPos, it's falling
  // if position was most recently within errorFromMax of minPos, it's rising
  // use this to determine where on the sin curve the wing is
  bool rising[2] = {true, true}; // confirm this -- wings go up first? 
  int errorFromMax = 5; //this might be a little high

  public:
  Wings(int lPin, int rPin)
  {
    this->pins[0] = lPin;
    this->pins[1] = rPin;
  }
  void setup()
  {
    // check for existence?
    wings[0].attach(pins[0]);
    wings[1].attach(pins[1]);
  }

  //TODO -- takes 150ms to run, what's going on
  //TODO maybe pull calculations into loop, not wings class? 
  //TODO steering of all kinds
  void writePos(float throttle, float roll = 0, float pitch = 0, float yaw = 0) // all inputs should be [0,1]
  {
    if((abs(throttle) < EPSILON))
    {
      wings[0].write(neutralPos);
      wings[1].write(neutralPos);
      return;
    }

    float nextPos[2];
          
    float freq = maxFreq * throttle;

    // we have a formula for wing position as "f(x) = amp * sin(freq * t) + neutral"
    // but we can't change the frequency without regard for current position
    // so we create a new sin function where g(0) = the current position
    // it's the same as f(x), but with some offset in the sin function
    // then we evaluate g(deltat) to find the next desired position
    for(int i = 0; i <= 1; i++)
    {
      float offset; // in secs

      if(abs(positions[i] - (neutralPos + amp)) < errorFromMax) rising[i] = false;
      if(abs(positions[i] - (neutralPos - amp)) < errorFromMax) rising[i] = true;

      // TODO add check for in range
      // calculate offset so g(0) = current position
      if(rising)
      {
        offset = asin((positions[i]-neutralPos)/amp)/(2*PI);
      }
      else
      {
        offset = asin(-(positions[i]-neutralPos)/amp)/(2*PI) + 0.5;
      }
  
      nextPos[i] = amp * sin(freq * 2*PI * deltat + 2*PI*offset) + neutralPos;
      positions[i] = nextPos[i];
    }

    // pulled out into separate loop to make more simultaneous
    for(int i = 0; i <= 1; i++)
    {
      wings[i].write(nextPos[i]);
    }
  }
  
};

class RCController
{
  //TODO make this an array? maybe even an array of stick structs ?
  int leftVertical;
  int leftHorizontal;
  int rightVertical;
  int rightHorizontal; 
   
  int leftVerticalPin;
  int leftHorizontalPin;
  int rightVerticalPin;
  int rightHorizontalPin;

  float deadzone = 18; //in raw sensor readings, ignore this many units on each side of neutral

  float leftVerticalDefault = 1483;
  float leftHorizontalDefault = 1483;
  float rightVerticalDefault = 1487;
  float rightHorizontalDefault = 1500;

  float leftVerticalMin = 999;
  float leftHorizontalMin = 999;
  float rightVerticalMin = 979;
  float rightHorizontalMin = 1010;

  float leftVerticalMax = 1996;
  float leftHorizontalMax = 1989;
  float rightVerticalMax = 1986;
  float rightHorizontalMax = 1983;

  public:
  RCController(int leftVerticalPin, int leftHorizontalPin, int rightVerticalPin, int rightHorizontalPin) {
    this->leftVerticalPin = leftVerticalPin;
    this->leftHorizontalPin = leftHorizontalPin;
    this->rightVerticalPin = rightVerticalPin;
    this->rightHorizontalPin = rightHorizontalPin;
  }

  void setup()
  {    
    pinMode(leftVerticalPin, INPUT);
    pinMode(leftHorizontalPin, INPUT);
    pinMode(rightVerticalPin, INPUT);
    pinMode(rightHorizontalPin, INPUT);
  }

  void read()
  {
    leftVertical = pulseIn(leftVerticalPin, HIGH);
    leftHorizontal = pulseIn(leftHorizontalPin, HIGH);
    rightVertical = pulseIn(rightVerticalPin, HIGH);
    rightHorizontal = pulseIn(rightHorizontalPin, HIGH);
  }

  float getThrottle()
  {
    float deadzoneReading = (abs(rightVertical - rightVerticalDefault) < deadzone ? rightVerticalDefault : rightVertical); //deadzone
    float normalizedReading = abs(deadzoneReading - rightVerticalDefault) / (rightVerticalMax - rightVerticalDefault); //convert to [0, 1]
    return normalizedReading;
  }
  
  int getYaw()
  {
    return 0;
  }

  int getPitch()
  {
    return 0;
  }

  int getRoll()
  {
    return 0;
  }
};

class Gyroscope
{
  int gPin; 
  Adafruit_BNO055 bno;
  imu::Vector<3> euler;
  imu::Vector<3> accel;

  public:
  Gyroscope(int gPin) 
  {
    this->gPin = gPin;
  }

  void setup()
  {
    bno = Adafruit_BNO055(55);
    if(!bno.begin()) // using I2C or SPI?
    {
      unsigned long startTime = millis();
      while(millis() - startTime < sensorTimeout)
      {
        Serial.print("No BNO055 detected.");
        delay(10);
      }
    }
  }

  void read()
  {
    euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  }

  float getXRot()
  {
    return euler.x();
  }  
  float getYRot()
  {
    return euler.y();
  }
  float getZRot()
  {
    return euler.z();
  }
  
  float getXAccel()
  {
    return accel.x();
  }  
  float getYAccel()
  {
    return accel.y();
  } 
  float getZAccel()
  {
    return accel.z();
  }  
};

// Totally random pins for my testing setup
Wings wings(9, 10);
RCController rc(2, 3, 4, 5); // goes to channels 1, 2, 3, 4 on RC receiver, and TODO: definitely does NOT correspond to the correct sticks on the RC controller. 
Gyroscope gyro(7);

void setup() {
  Serial.begin(115200);
  
  wings.setup();
  gyro.setup();
}

void loop() {
  //read
  rc.read();
  
  gyro.read();
  /*Serial.println(gyro.getXRot());
  Serial.println(gyro.getYRot());
  Serial.println(gyro.getZRot());
  Serial.println(gyro.getXAccel());
  Serial.println(gyro.getYAccel());
  Serial.println(gyro.getZAccel());*/

  //update
  // move control code here
  
  //write
  wings.writePos(rc.getThrottle());

  delay(deltat * 1000);
}
