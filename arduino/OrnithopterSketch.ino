#include <Servo.h>
#include <Adafruit_LSM6DSO32.h>

unsigned long sensorTimeout = 100; // or whatever
float deltat = 0.015;

// pull these class definitions out into separate files?
class Wings
{
  Servo lWing;
  Servo rWing;
  int lPin;
  int rPin;
  float lPosition;
  float rPosition;

  float amp = 45/2;
  float neutralPos = 90; //TODO
  float minPos = neutralPos - amp;
  float maxPos = neutralPos + amp;
  
  int maxFreq = 7; //Hz

  bool rising = true; //maybe not the best way to go about this?
  int errorFromMax = 5; //this might be a little high

  public:
  Wings(int lPin, int rPin)
  {
    this->lPin = lPin;
    this->rPin = rPin;
  }
  void setup()
  {
    // check for existence?
    lWing.attach(lPin);
    rWing.attach(rPin);
  }

  //TODO -- this function takes 150ms to run, so sin lookup tables!
  void writePos(float throttle = 0, float steer = 0) //can just run "writePos()" to stop wings
  {
    lPosition = lWing.read();
    rPosition = rWing.read();
    float currentPos = lPosition; //eventually make this currentLPos etc. 
    
    float t;
    if(abs(currentPos - maxPos) < errorFromMax) rising = false;
    if(abs(currentPos - minPos) < errorFromMax) rising = true;

    if(rising)
    {
      t = asin((currentPos-neutralPos)/amp)/(2*PI);
    }
    else
    {
      t = asin(-(currentPos-neutralPos)/amp)/(2*PI) + 0.5;
    }

    float freq = maxFreq * throttle;
    float nextPos = amp * sin(freq * 2*PI * deltat + 2*PI*t) + neutralPos; //maybe wrong units for things, freq / 1000? 

    Serial.print(nextPos);
    Serial.print(" at frequency ");
    Serial.println(freq);
    
    //TODO steering of all kinds
    lWing.write(nextPos);
    rWing.write(nextPos);
  }
  
};

class RCController
{
  //TODO make this a struct
  int leftVertical;
  int leftHorizontal;
  int rightVertical;
  int rightHorizontal; 
   
  int leftVerticalPin;
  int leftHorizontalPin;
  int rightVerticalPin;
  int rightHorizontalPin;

  float deadzone = 16; //in raw sensor readings, ignore 16 units on each side of neutral
  
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
  
  int getSteer()
  {
    /*float normalizedReading = abs(leftHorizontalPin - defaultReading) / (maxReading - defaultReading); //convert to [0, 1]
    normalizedReading = (normalizedReading < 0.1 ? 0 : normalizedReading); //deadzone
    return normalizedReading;*/
    return 0;
  }
};

class Gyroscope
{
  int gPin; // NONSENSE
  Adafruit_LSM6DSO32 dso32;
  sensors_event_t accel; //m/s^2
  sensors_event_t gyro; //rad/s
  sensors_event_t temp; //C

  public:
  Gyroscope(int gPin) 
  {
    this->gPin = gPin;
  }

  void setup()
  {
    if(!dso32.begin_I2C()) // using I2C or SPI?
    {
      unsigned long startTime = millis();
      while(millis() - startTime < sensorTimeout)
      {
        delay(10);
      }
    }
    // if we want to customize range/rates, change here      
  }

  void read()
  {
    dso32.getEvent(&accel, &gyro, &temp);
  }

  float getXRot()
  {
    return gyro.gyro.x;
  }  
  float getYRot()
  {
    return gyro.gyro.y;
  }
  float getZRot()
  {
    return gyro.gyro.z;
  }
  
  float getXAccel()
  {
    return accel.acceleration.x;
  }  
  float getYAccel()
  {
    return accel.acceleration.y;
  } 
  float getZAccel()
  {
    return accel.acceleration.z;
  }  

  //TODO write some read functions
  // e.g., return gyro.gyro.x or accel.acceleration.z
};

// NONSENSE NUMBERS
Wings wings(9, 10);
RCController rc(2, 3, 4, 5); // goes to channels 1, 2, 3
Gyroscope gyro(7);

void setup() {
  Serial.begin(115200);
  
  wings.setup();
  gyro.setup();
}

void loop() {
  //read
  rc.read();
  //Serial.println(rc.getThrottle());
  //Serial.println(rc.getSteer());
  
  //gyro.read();
  /*Serial.println(gyro.getXRot());
  Serial.println(gyro.getYRot());
  Serial.println(gyro.getZRot());
  Serial.println(gyro.getXAccel());
  Serial.println(gyro.getYAccel());
  Serial.println(gyro.getZAccel());*/
  
  //write
  if(rc.getThrottle() > 1e-3)
  {
    wings.writePos(rc.getThrottle());
  }


  delay(100);
}
