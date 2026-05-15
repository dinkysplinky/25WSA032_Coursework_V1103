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

//thresholds, ring buffer + idle counter
const float STABLE_THRESHOLD   = 2.0;
const float FLUCTUATION_SPIKE  = 8.0;
const int   IDLE_CYCLES_FOR_PD = 5;
const int   MA_WINDOW          = 10;
const float RATE_MIN_HZ        = 0.5;
const float RATE_MAX_HZ        = 4.0;

int   idle_count = 0;
float diff_history[MA_WINDOW];
int   diff_count = 0;
int   diff_index = 0;

//memory: 3 x float[64] + float[10] ~ 808 B (~40% of UNO SRAM).


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
    float sample_period = reading_interval_period / 1000.0; //ssss

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

//sum of |T[n] - T[n-1]| - stability metric
float compute_total_difference() {
    float total = 0.0;
    for (int i = 1; i < array_size; i++) total += fabs(reading_array[i] - reading_array[i - 1]);
    return total;
}

//moving average over last MA_WINDOW total_diff values - trend predictor
float update_moving_average(float latest_diff) {
    diff_history[diff_index] = latest_diff;
    diff_index = (diff_index + 1) % MA_WINDOW;
    if (diff_count < MA_WINDOW) diff_count++;
    float sum = 0.0;
    for (int i = 0; i < diff_count; i++) sum += diff_history[i];
    return sum / diff_count;
}

//peak bin of magnitude[] (skip DC) -> dominant frequency in Hz
float find_dominant_frequency() {
    int   peak_k   = 1;
    float peak_mag = 0.0;
    for (int k = 1; k < array_size / 2; k++) {
        if (magnitude[k] > peak_mag) { peak_mag = magnitude[k]; peak_k = k; }
    }
    return frequency[peak_k];
}

//rate >= 2 * dom_freq, between RATE_MIN_HZ and RATE_MAX_HZ 
void apply_nyquist_constraint() {
    float dominant_freq        = find_dominant_frequency();
    float current_hz = 1000.0 / reading_interval_period;
    float nyquist    = 2.0 * dominant_freq;
    float target     = (nyquist > current_hz) ? nyquist : current_hz;
    if (target < RATE_MIN_HZ) target = RATE_MIN_HZ;
    if (target > RATE_MAX_HZ) target = RATE_MAX_HZ;
    reading_interval_period = (int)(1000.0 / target + 0.5);
    sampling_freq           = target;
}

int decide_power_mode() {
    float total_diff     = compute_total_difference();
    float predicted_diff = update_moving_average(total_diff);

    if (total_diff > FLUCTUATION_SPIKE) {
        idle_count = 0;
        return ACTIVE;
    }
    if (predicted_diff < STABLE_THRESHOLD) {
        idle_count++;
        if (idle_count >= IDLE_CYCLES_FOR_PD) return POWER_DOWN;
        return IDLE;
    }
    idle_count = 0;

    // [original centroid-based fallback]
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

    apply_nyquist_constraint();   
 }