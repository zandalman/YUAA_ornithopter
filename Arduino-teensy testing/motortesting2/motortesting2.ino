#include <Servo.h>

Servo myservo;
static int servoPin=3;
static int led=12;
int location=1000;
boolean tick=false;
void setup() {
  // put your setup code here, to run once:
  pinMode(led, OUTPUT);
  pinMode(servoPin,OUTPUT);
  myservo.attach(servoPin);
  myservo.writeMicroseconds(1000);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
 myservo.write(location);
  if(location>2000){
    tick=true;
    digitalWrite(led,HIGH);
  }else if(location<1000){
    tick=false;
    digitalWrite(led, LOW);
  }
  if(tick){
    location=location-10;
  }else{
    location=location+10; 
  }
  delay(20);
  Serial.println(location);
}
