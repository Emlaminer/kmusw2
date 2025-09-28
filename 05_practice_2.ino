// 실습 3: GPIO를 통한 LED 제어
// Pin7에 연결된 LED를 제어

const int ledPin = 7;

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  digitalWrite(ledPin, HIGH);
  delay(1000);

  
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPin, HIGH);
    delay(100);   
    digitalWrite(ledPin, LOW);
    delay(100);   
  }

  digitalWrite(ledPin, LOW);

  while (1) {}
}
