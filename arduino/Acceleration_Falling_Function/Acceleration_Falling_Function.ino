//things to code: isFalling, isFallingThreshold, getRotation, 
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#define DEFAULT_FALLING_ACCEL (9.5)

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);
imu::Vector<3> originalOrientation;

bool isFalling(){ //creates new sensor event, returns 0 for not falling, 1 for falling
  //create sensor event
  sensors_event_t event;
  bno.getEvent(&event);
  imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

  float linearAcceleration = pow(pow(accel.x(),2)+pow(accel.y(),2)+pow(accel.z(),2),0.5);
  
  return(DEFAULT_FALLING_ACCEL<linearAcceleration);
}

bool isFallingThreshold(float accelerationThreshold){ //creates new sensor event, return 0 for not falling, 1 for falling, based on acceleration threshold for falling
  //create sensor event
  sensors_event_t event;
  bno.getEvent(&event);
  imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

  float linearAcceleration = pow(pow(accel.x(),2)+pow(accel.y(),2)+pow(accel.z(),2),0.5);
  
  return(accelerationThreshold<linearAcceleration);
}

imu::Vector<3> getEulerOrientation(){ //returns the euler orientation vector
  //create sensor event
  sensors_event_t event;
  bno.getEvent(&event);
  return(bno.getVector(Adafruit_BNO055::VECTOR_EULER));
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Orientation Sensor Test");

  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

  delay(1000);

  bno.setExtCrystalUse(true);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("isFalling():");
  Serial.println(isFalling());

  float threshold=4;
  Serial.print("isFallingThreshold:");
  Serial.println(isFallingThreshold(threshold));

  imu::Vector<3> orientation=getEulerOrientation();
  Serial.print("X:");
  Serial.print(orientation.x());
  Serial.print("\tY:");
  Serial.print(orientation.y());
  Serial.print("\tZ:");
  Serial.println(orientation.z());

  
}
