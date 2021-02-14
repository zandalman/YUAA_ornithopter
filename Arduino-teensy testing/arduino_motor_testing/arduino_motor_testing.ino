#include <Servo.h>

Servo myservo;
static int servoPin=9;
int location=0;
boolean tick=false;
void setup() {
  // put your setup code here, to run once:
myservo.attach(servoPin);
myservo.write(90);

}

void loop() {
  // put your main code here, to run repeatedly:
  myservo.write(location);
  if(location>179){
    tick=true;
  }else if(location<1){
    tick=false;
  }
  if(tick){
    location--;
  }else{
    location++; 
  }
  delay(20);
}
