#include "SR04.h"
#include <Servo.h>

#define SERVO_PIN 8
#define SERVO_PIN2 7

Servo servo;
Servo servo2;

const int trigPin = 9;
const int echoPin = 10;

SR04 sr04 = SR04(echoPin,trigPin);

bool capturing = false;
bool receivedCategory = false;
long distance;

void setup() {
  servo.attach(SERVO_PIN);
  servo2.attach(SERVO_PIN2);

  Serial.begin(9600);
}


void loop() {
  distance = sr04.Distance();

  if (distance < 8 && !capturing) {
    Serial.println("record");
    capturing = true;
  }
  
  String garbageType = "";
  if (Serial.available() > 0) {
    garbageType = Serial.readString();
    Serial.println(garbageType);
    receivedCategory = true;
  }


  if(capturing && receivedCategory) {
    Serial.println("captured");
    if (garbageType == "trash" || garbageType == "Trash"){
      trash();

    } else if (garbageType == "metal can" || garbageType == "Metal can" || garbageType == "Metal Can"){
      metalCan();

    }else if (garbageType == "electronic" || garbageType == "Electronic"){
      electronic();
      
    }else if (garbageType == "compost" || garbageType == "Compost"){
      compost();
      
    }else if (garbageType == "plastic" || garbageType == "Plastic"){
      plastic();
      
    }

    capturing = false;
    receivedCategory = false;
  }


  delay(1000);

}

void trash() {
  Serial.print("Received trash");
  Serial.println(distance);
  delay(1000);
  servo.write(0);
  delay(2000);
  servo.write(-180);
  servo2.write(50);
  delay(1000);
  
  servo.write(360);
  delay(1000);
  servo2.write(90);
}

void metalCan() {
  Serial.print("Received metal can");
  Serial.println(distance);
  delay(1000);
  servo.write(0);
  delay(1000);
  servo.write(180);
  servo2.write(50);
  delay(1000);
  delay(1000);
  servo2.write(90);
}

void electronic() {
  Serial.print("Received electronic");
  Serial.println(distance);
  delay(1000);
  servo.write(0);
  delay(666);
  servo.write(180);
  servo2.write(50);
  delay(1000);
  delay(1000);
  servo2.write(90);
}

void compost() {
  Serial.print("Received compost");
  Serial.println(distance);
  delay(1000);
  servo.write(0);
  delay(333);
  servo.write(180);
  servo2.write(50);
  delay(1000);
  delay(1000);
  servo2.write(90);
}

void plastic() {
  Serial.print("Received plastic");
  Serial.println(distance);
  delay(1000);
  servo2.write(50);
  delay(1000);
  delay(1000);
  servo2.write(90);
}
