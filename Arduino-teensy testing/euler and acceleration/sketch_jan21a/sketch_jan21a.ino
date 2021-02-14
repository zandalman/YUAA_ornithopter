#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
  
Adafruit_BNO055 bno = Adafruit_BNO055(55);
 
void setup(void) 
{
  Serial.begin(9600);
  Serial.println("Orientation Sensor Test"); Serial.println("");
  
  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  delay(1000);
    
  bno.setExtCrystalUse(true);
}
 
void loop(void) 
{
  sensors_event_t event;
  bno.getEvent(&event);
  
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  
  /* Display the floating point data */
  Serial.print("Euler\tX: ");
  Serial.print(euler.x(), 4);
  Serial.print("\tY: ");
  Serial.print(euler.y(), 4);
  Serial.print("\tZ: ");
  Serial.print(euler.z(), 4);
  Serial.print("Lin Accel\tX: ");
  Serial.print(accel.x(), 4);
  Serial.print("\tY: ");
  Serial.print(accel.y(), 4);
  Serial.print("\tZ: ");
  Serial.print(accel.z(), 4);
  Serial.println("");
  
  delay(100);
}

