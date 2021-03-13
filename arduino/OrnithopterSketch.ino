#include <Servo.h>
#include <PPMReader.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#define BNO055_SAMPLERATE_DELAY_MS (100)
#define EPSILON (0.00001)

unsigned long sensorTimeout = 30; //30 ms before we give up on detecting the gyro
float deltat = 0.015; // in sec // used in loop, and for wing motion implemented w/ discrete timesteps

int interruptPin = 3;
int channelAmount = 4;
PPMReader ppm(interruptPin, channelAmount); // I want to move this into RCController; how would I separate this into a constructor without weird pointer things?

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
  int values[4];
  int defaultValue = 1500;
  int maxValue = 2000;
  
  float deadzone = 20; //in raw sensor readings, ignore this many units on each side of neutral

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

  float getThrottle() //channel 2
  {
    float deadzoneReading = (abs(values[2] - defaultValue) < deadzone ? 1500 : values[2]); //deadzone
    float normalizedReading = abs(deadzoneReading - defaultValue) / (maxValue - defaultValue); //convert to [0, 1]
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
RCController rc(interruptPin);
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
