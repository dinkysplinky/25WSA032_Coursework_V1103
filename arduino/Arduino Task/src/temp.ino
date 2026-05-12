// Loovee @ 2015-8-26
#include <Arduino.h>
#include <math.h>
const int B = 4275; // B value of the thermistor
int R0 = 100000; // R0 = 100k
const int pinTempSensor = A0; // Grove - Temperature Sensor connect to A0

int reading_interval_period = 100;//define a sample collection interval 
float sampling_freq = 1000/ reading_interval_period;
const int array_size = 64;

int i;//defines the sample number of the data in the array
float reading_array[array_size];
float magnitude[array_size];            
float frequency[array_size];    

const float F_MAX = 0.6;

int mode;

#define ACTIVE 0 
#define IDLE 1
#define POWER_DOWN 2



void collect_temperature_data(){//define a new function to collect data from the sensor and calculate temp 

    unsigned long prev = millis() - reading_interval_period;
    int i = 0;
 
    while (i < array_size) {
        unsigned long now = millis();
        if (now - prev >= (unsigned long)reading_interval_period) {
            int   a = analogRead(pinTempSensor);
            float R = 1023.0 / a - 1.0;
            R = R0 * R;
            float tempurature = 1.0 / (log(R / R0) / B + 1.0 / 298.15) - 273.15;
            reading_array[i++] = tempurature;
            prev = now;
            Serial.print(i);
            Serial.print(": ");
            Serial.println(tempurature);
        }
    }
      Serial.println("Finished data collection");
}

float* apply_dft() {//define a new function to apply dft to the collected temp data
    sampling_freq = 1000/ reading_interval_period;
    float mean = 0.0;
    for (int i = 0; i < array_size; i++) mean += reading_array[i];
    mean /= array_size;
    for (int k = 0; k < array_size; k++) {
        float X_real = 0.0;
        float X_imag = 0.0;
 
        for (int n = 0; n < array_size; n++) {                       // Eq. 3.1
            float angle = 2.0 * PI * k * n / array_size;
            float xn    = reading_array[n] - mean;   
            X_real +=  reading_array[n] * cos(angle);       // Eq. 3.3
            X_imag += -reading_array[n] * sin(angle);       // Eq. 3.4
        }
 
        magnitude[k] = sqrt(X_real * X_real + X_imag * X_imag);               // Eq. 3.5
        frequency[k] = ((float)k * sampling_freq) / array_size;      // Eq. 3.2
    }
    return frequency;
}

void send_data_to_pc() {
    float sample_period = reading_interval_period / 1000.0;   // ssss
 
    Serial.println(F("Time,Temperature,Frequency,Magnitude"));
    for (int i = 0; i < array_size; i++) {
        Serial.print(i * sample_period, 4);
        Serial.print(F(","));
        Serial.print(reading_array[i], 2);
        Serial.print(F(","));
        Serial.print(frequency[i], 4);
        Serial.print(F(","));
        Serial.println(magnitude[i], 4);
    }
}

int decide_power_mode() {
    float w = 0.0;
    float m = 0.0;
    float bins_used = 0;
 
    for (int k = 1; k < array_size / 2; k++) {
        w += frequency[k] * magnitude[k];
        m += magnitude[k];
        bins_used++;
    }
 
    float avg = (m > 0.0) ? (w / m) : 0.0;
    Serial.print(F("Centroid over [0, "));
    Serial.print(F_MAX, 2);
    Serial.print(F("] Hz across "));
    Serial.print(bins_used);
    Serial.print(F(" bins = "));
    Serial.print(avg, 4);
    Serial.println(F(" Hz"));
 
    if (avg > 0.5)      return ACTIVE;
    else if (avg > 0.1) return IDLE;
    else                return POWER_DOWN;
}


void setup()
{
  Serial.begin(9600);
  mode = ACTIVE;
  Serial.println("Serial setup complete");
}

void loop(){
  collect_temperature_data();

  apply_dft();

  send_data_to_pc();

  mode = decide_power_mode();

  switch (mode) {
        case ACTIVE:    
        Serial.println(F("Mode: ACTIVE"));
        reading_interval_period = 100;     
        break;

        case IDLE:      
        Serial.println(F("Mode: IDLE"));
        reading_interval_period = 500;       
        break;
        
        case POWER_DOWN:
        Serial.println(F("Mode: POWER_DOWN"));
        reading_interval_period = 3000; 
        break;
    }
 }
