
#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13


#define SND_VEL 346.0    
#define INTERVAL 25       
#define PULSE_DURATION 10  
#define _DIST_MIN 100    
#define _DIST_MAX 300     

#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)    

#define _EMA_ALPHA 0.5    
#define N 5                

unsigned long last_sampling_time;   
float dist_prev = _DIST_MAX;       
float dist_ema;                    
float dist_samples[N];              
int sample_index = 0;               

void setup() {
  // Initialize GPIO pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  Serial.begin(57600);
}

void loop() {
  float dist_raw;
  
  if (millis() < last_sampling_time + INTERVAL)
    return;
    
 
  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);
  
  
  if ((dist_raw == 0.0) || (dist_raw > _DIST_MAX)) {
      dist_raw = _DIST_MAX + 10.0;  
      digitalWrite(PIN_LED, 1);       
  } else if (dist_raw < _DIST_MIN) {
      dist_raw = _DIST_MIN - 10.0;   
      digitalWrite(PIN_LED, 1);     
  } else {    // In desired range
      digitalWrite(PIN_LED, 0);       
  }

  
  dist_samples[sample_index] = dist_raw;
  sample_index = (sample_index + 1) % N; 

  float dist_median = calculateMedian(dist_samples);

  
  dist_ema = _EMA_ALPHA * dist_raw + (1 - _EMA_ALPHA) * dist_ema;

  
  Serial.print("Min: "); Serial.print(_DIST_MIN);
  Serial.print(", Raw: "); Serial.print(dist_raw);
  Serial.print(", Median: "); Serial.print(dist_median);
  Serial.print(", EMA: "); Serial.print(dist_ema);
  Serial.print(", Max: "); Serial.print(_DIST_MAX);
  Serial.println("");

 
  last_sampling_time += INTERVAL;
}

float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}

float calculateMedian(float *samples) {
    
    float temp[N];
    memcpy(temp, samples, N * sizeof(float));
    
   
    for (int i = 0; i < N - 1; i++) {
        for (int j = 0; j < N - i - 1; j++) {
            if (temp[j] > temp[j + 1]) {
                // Swap
                float t = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = t;
            }
        }
    }

    return temp[N / 2]; 
}
