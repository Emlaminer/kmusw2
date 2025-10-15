#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13

#define SND_VEL 346.0     
#define INTERVAL 25       
#define PULSE_DURATION 10 
#define _DIST_MIN 100    
#define _DIST_MAX 300     
#define _EMA_ALPHA 0.06 //n=3-> 0.5 , n = 10 -> 0.18 , n = 30 -> 0.06

#define TIMEOUT ((INTERVAL / 2) * 1000.0) 
#define SCALE (0.001 * 0.5 * SND_VEL)  

unsigned long last_sampling_time = 0;
float dist_prev = _DIST_MAX; 
float dist_ema = _DIST_MAX; 

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  Serial.begin(57600);
}

void loop() {
  float dist_raw, dist_filtered;

  if (millis() < last_sampling_time + INTERVAL)
    return;

  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  if ((dist_raw == 0.0) || (dist_raw > _DIST_MAX)) {
      dist_filtered = dist_prev;  // giữ giá trị trước đó
  } else if (dist_raw < _DIST_MIN) {
      dist_filtered = dist_prev;
  } else {
      dist_filtered = dist_raw;
      dist_prev = dist_raw;
  }

  dist_ema = _EMA_ALPHA * dist_filtered + (1 - _EMA_ALPHA) * dist_ema;

  int led_value = map(dist_ema, _DIST_MIN, _DIST_MAX, 255, 0);
  led_value = constrain(led_value, 0, 255);
  analogWrite(PIN_LED, led_value);

  Serial.print("raw:");      Serial.print(dist_raw);
  Serial.print(",filtered:"); Serial.print(dist_filtered);
  Serial.print(",ema:");     Serial.print(dist_ema);
  Serial.print(",led:");     Serial.println(led_value);

  last_sampling_time += INTERVAL;
}

float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}