git/*
 * 도전과제 1: 주차장 차단기 구현
 * 작성자: [Tên của bạn]
 * 설명:
 *  - 초음파 센서로 차량 접근 감지
 *  - sigmoid 함수를 이용하여 서보모터를 부드럽게 제어
 *  - 차량이 감지되면 차단기를 올리고, 지나가면 내림
 */

#include <Servo.h>
#include <math.h>

#define PIN_SERVO 10  
#define TRIG_PIN 2     
#define ECHO_PIN 3     

Servo gate;            
bool gateOpen = false; 

unsigned long MOVING_TIME = 2000;

const int THRESHOLD_OPEN = 10;  
const int THRESHOLD_CLOSE = 20;

float sigmoid(float x) {
  return 1.0 / (1.0 + exp(-x));
}

float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); 
  if (duration == 0) return -1.0; 
  float distance = duration * 0.034 / 2.0;
  return distance;
}

void moveServoSmooth(int fromAngle, int toAngle) {
  unsigned long start = millis();
  while (millis() - start <= MOVING_TIME) {
    float progress = (float)(millis() - start) / MOVING_TIME; 
    float x = progress * 10 - 5;   
    float s = sigmoid(x);         
    int angle = fromAngle + (toAngle - fromAngle) * s;
    gate.write(angle);
    delay(15);
  }
  gate.write(toAngle);
}

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  gate.attach(PIN_SERVO);
  gate.write(0);
  Serial.println("Parking gate system started");
}

void loop() {
  float distance = getDistance();
  if (distance < 0) return;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (!gateOpen && distance <= THRESHOLD_OPEN) {
    Serial.println("Car detected → Open gate");
    moveServoSmooth(0, 90);
    gateOpen = true;
    delay(1000); // xe qua
  }

  if (gateOpen && distance >= THRESHOLD_CLOSE) {
    Serial.println("Car gone → Close gate");
    moveServoSmooth(90, 0);
    gateOpen = false;
    delay(1000);
  }

  delay(100);
}
