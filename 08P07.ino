#define PIN_LED   9
#define PIN_TRIG  12
#define PIN_ECHO  13

#define SND_VEL 346.0       
#define INTERVAL 25          
#define PULSE_DURATION 10    
#define _DIST_MIN 100.0     
#define _DIST_MAX 300.0      
#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

unsigned long last_sampling_time = 0;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);
  Serial.begin(57600);
}

void loop() {
  if (millis() - last_sampling_time < INTERVAL)
    return;
  last_sampling_time = millis();

  float distance = USS_measure(PIN_TRIG, PIN_ECHO);


  if (distance < _DIST_MIN) distance = _DIST_MIN;
  if (distance > _DIST_MAX) distance = _DIST_MAX;

  int led_value = calcLedBrightness(distance);
  analogWrite(PIN_LED, led_value);

  Serial.print("distance:"); Serial.print(distance);
  Serial.print(",led:"); Serial.println(led_value);
}

float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // mm
}

int calcLedBrightness(float distance) {
  int brightness = 0;

  if (distance <= 200)
    brightness = map(distance, 100, 200, 0, 255);
  else 
    brightness = map(distance, 200, 300, 255, 0);


  if (distance == 150 || distance == 250)
    brightness = 128;  

  return brightness;
}