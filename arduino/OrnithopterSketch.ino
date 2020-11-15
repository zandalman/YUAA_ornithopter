#include <Servo.h>
#include <Adafruit_LSM6DSO32.h>

unsigned long sensorTimeout = 100; // or whatever
int flying = false;
int startTime = 0;

// pull these class definitions out into separate files?
class Wings
{
  Servo lWing;
  Servo rWing;
  int lPin;
  int rPin;
  int lPosition;
  int rPosition;
  
  int neutralPos = 1500;
  int minPos = 20;
  int maxPos = 30; // so we flap through an angle of 60 degrees

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
  void writePos(int t, float amp = 1, float freq = 5)
  {
    // amp: [0, 1] -> [minPos, maxPos]
    int maxAngle = (maxPos - minPos) * amp + minPos;
    //TODO deal with direction of servo movement
    lWing.write(maxAngle * sin(2*PI*t/1000 * freq));
    rWing.write(maxAngle * sin(2*PI*t/1000 * freq));
  }
  
};

class RCController
{
  // I don't know if this is legit
  int freq;
  int throttle; 
  int steer; 
  int freqPin;
  int throttlePin;
  int steerPin;
  
  public:
  RCController(int freqPin, int throttlePin, int steerPin) {
    this->freqPin = freqPin;
    this->throttlePin = throttlePin;
    this->steerPin = steerPin;
  }

  void setup()
  {    
    pinMode(freqPin, INPUT);
    pinMode(throttlePin, INPUT);
    pinMode(steerPin, INPUT);
  }

  void read()
  {
    freq = pulseIn(freqPin, HIGH);
    throttle = pulseIn(throttlePin, HIGH);
    steer = pulseIn(steerPin, HIGH);
  }

  int getThrottle()
  {
    return throttle;
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
RCController rc(2, 3, 4);
Gyroscope gyro(2);

void setup() {
  Serial.begin(115200);
  
  wings.setup();
  gyro.setup();
}

void loop() {
  //read
  rc.read(); // TODO: scale to 1!
  gyro.read();
  if(!flying and rc.getThrottle() > 1e-3)
  {
    startTime = millis();
    flying = true;
  }

  //write
  if(rc.getThrottle() > 1e-3)
  {
    wings.writePos(millis() - startTime, rc.getThrottle());
  }
}
// testing link
