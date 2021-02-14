#include <Servo.h>

Servo myservo;
static int servoPin=9;
int location=60;
boolean tick=false;
void setup() {
  // put your setup code here, to run once:
myservo.attach(servoPin);
//.write(90);

}

void loop() {
  // put your main code here, to run repeatedly:
  /*myservo.write(location);
  if(location>120){
    tick=true;
  }else if(location<60){
    tick=false;
  }
  if(tick){
    location--;
  }else{
    location++; 
  }
  delay(10);*/
  myservo.write(60);
  delay(100);
  myservo.write(120);
  delay(100);
  
}
