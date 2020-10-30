#include <Servo.h>
#include <Adafruit_LSM6DSO32.h>

unsigned long sensorTimeout = 100; // or whatever

// pull these class definitions out into separate files?
class Wings
{
  Servo lWing;
  Servo rWing;
  int lPin;
  int rPin;
  int lPosition;
  int rPosition;

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
};

class RCController
{
  // I don't know if this is legit
  int roll;
  int throttle; 
  int rollPin;
  int throttlePin;
  
  public:
  RCController(int rollPin, int throttlePin) {
    this->rollPin = rollPin;
    this->throttlePin = throttlePin;
  }

  void setup()
  {    
    pinMode(rollPin, INPUT);
    pinMode(throttlePin, INPUT);
  }

  void read()
  {
    roll = pulseIn(rollPin, HIGH);
    throttle = pulseIn(throttlePin, HIGH);
  }

  //TODO write some read functions
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

  //TODO write some read functions
  // e.g., return gyro.gyro.x or accel.acceleration.z
};

// NONSENSE NUMBERS
Wings wings(0, 1);
RCController rc();
Gyroscope gyro(2);

void setup() {
  wings.setup();
  gyro.setup();
}

void loop() {
  
}
