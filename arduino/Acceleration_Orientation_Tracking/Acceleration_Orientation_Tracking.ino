//things to code: isFalling, isFallingThreshold, getRotation, 
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);
imu::Vector<3> originalOrientation;
int defaultOrientation[3]={0,90,0};
imu::Vector<3> getEulerOrientation(){ //returns the euler orientation vector
  //create sensor event
  sensors_event_t event;
  bno.getEvent(&event);
  return(bno.getVector(Adafruit_BNO055::VECTOR_EULER));
}

float eulerToDegreeDifference(imu::Vector<3> standard){
  imu::Vector<3> newRotation = getEulerOrientation();
  int xdiff=(int)(standard.x()-newRotation.x())%180;
  int ydiff=(int)(standard.y()-newRotation.y())%180;
  int zdiff=(int)(standard.z()-newRotation.z())%180;
  
  float linearAcceleration = pow(pow(xdiff,2)+pow(ydiff,2)+pow(zdiff,2),0.5);
  if(linearAcceleration>90){
    return linearAcceleration-180;
  }else{
    return linearAcceleration;
  }
}

int getRollDiff(imu::Vector<3> standard, int isDefault=0){//returns the difference (in degrees) of the y euler angle
  imu::Vector<3> newRotation = getEulerOrientation();
  //convert angles to be between -180 and 180
  int adjNew=newRotation.y();
  int adjOld=standard.y();
  if(isDefault){
    adjOld=defaultOrientation[1];
  }
  
  int zdiff=(int)(adjNew-adjOld);

  

  if(zdiff>180)
    zdiff-=360;
  if(zdiff<-180)
    zdiff+=360;
  return zdiff;
}

int getPitchDiff(imu::Vector<3> standard, int isDefault=0){//returns the difference (in degrees) of the y euler angle
  imu::Vector<3> newRotation = getEulerOrientation();
  //convert angles to be between -180 and 180
  int adjNew=newRotation.y();
  int adjOld=standard.y();

  if(isDefault){
    adjOld=defaultOrientation[2];
  }
  
  int xdiff=(int)(adjNew-adjOld);
  if(xdiff>180)
    xdiff-=360;
  if(xdiff<-180)
    xdiff+=360;
  return xdiff;
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
  bno.setExtCrystalUse(true);

  //default orientation
  Serial.print("X:");
  Serial.print(defaultOrientation[0]);
  Serial.print("\tY:");
  Serial.print(defaultOrientation[1]);
  Serial.print("\tZ:");
  Serial.println(defaultOrientation[2]);

  Serial.print("Getting upright orientation data....");
  delay(1000);
  originalOrientation=getEulerOrientation();
  Serial.println("...Done!");
  
}

void loop() {
  // put your main code here, to run repeatedly:

  imu::Vector<3> orientation=getEulerOrientation();
  ///*
  Serial.print("X:");
  Serial.print(orientation.x());
  Serial.print("\tY:");
  Serial.print(orientation.y());
  Serial.print("\tZ:");
  Serial.println(orientation.z());
  //*/

  //original orientation
  /*
  Serial.print("X:");
  Serial.print(originalOrientation.x());
  Serial.print("\tY:");
  Serial.print(originalOrientation.y());
  Serial.print("\tZ:");
  Serial.println(originalOrientation.z());
  //*/
  //pitch and roll stuff
  /*
  Serial.print("Pitch Difference:");
  int pitchDifference=getPitchDiff(originalOrientation, 1);
  Serial.print(pitchDifference);
  Serial.print("\Roll Difference:");

  int rollDifference= getRollDiff(originalOrientation, 1);
  Serial.println(rollDifference);
  //*/
  //Serial.println(eulerToDegreeDifference(originalOrientation));

  
}
