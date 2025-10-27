#include <Servo.h>


#define PIN_LED   9   
#define PIN_TRIG  12  
#define PIN_ECHO  13 
#define PIN_SERVO 10


#define SND_VEL 346.0
#define INTERVAL 25        
#define PULSE_DURATION 10  
#define _DIST_MIN 180.0      
#define _DIST_MAX 360.0   

#define TIMEOUT ((INTERVAL / 2) * 1000.0)   
#define SCALE   (0.001 * 0.5 * SND_VEL)     

#define _EMA_ALPHA 0.3      

#define _TARGET_LOW  250.0
#define _TARGET_HIGH 290.0

#define _DUTY_MIN 500  
#define _DUTY_NEU 1500 
#define _DUTY_MAX 2700 

Servo myservo;

float dist_prev = _DIST_MIN;     
float dist_ema  = _DIST_MIN;    
unsigned long last_sampling_time = 0;
bool ema_initialized = false;

static inline float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  if (in_max == in_min) return out_min;
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // mm
}

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  myservo.attach(PIN_SERVO);
  myservo.writeMicroseconds(_DUTY_NEU);

  Serial.begin(57600);
}

void loop() {
  if (millis() < last_sampling_time + INTERVAL) return;
  last_sampling_time += INTERVAL;

  float dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  float dist_filtered;
  if (dist_raw >= _DIST_MIN && dist_raw <= _DIST_MAX) {
    dist_filtered = dist_raw;
    dist_prev = dist_raw;
  } else {
    dist_filtered = dist_prev;
  }

  if (!ema_initialized) {     
    dist_ema = dist_filtered;
    ema_initialized = true;
  } else {
    dist_ema = _EMA_ALPHA * dist_filtered + (1.0 - _EMA_ALPHA) * dist_ema;
  }
  float angle_deg;
  if (dist_ema <= _DIST_MIN)       angle_deg = 0.0;
  else if (dist_ema >= _DIST_MAX)  angle_deg = 180.0;
  else                             angle_deg = mapFloat(dist_ema, _DIST_MIN, _DIST_MAX, 0.0, 180.0);

  int duty = (int)mapFloat(angle_deg, 0.0, 180.0, _DUTY_MIN, _DUTY_MAX);
  myservo.writeMicroseconds(duty);

  if (dist_raw >= _DIST_MIN && dist_raw <= _DIST_MAX) digitalWrite(PIN_LED, LOW);  // ON
  else                                                digitalWrite(PIN_LED, HIGH); // OFF

  Serial.print("Min:");    Serial.print(_DIST_MIN);
  Serial.print(",Low:");   Serial.print(_TARGET_LOW);
  Serial.print(",dist:");  Serial.print(dist_raw);
  Serial.print(",ema:");   Serial.print(dist_ema);
  Serial.print(",Servo:"); Serial.print((int)angle_deg);
  Serial.print(",High:");  Serial.print(_TARGET_HIGH);
  Serial.print(",Max:");   Serial.print(_DIST_MAX);
  Serial.println("");
}
