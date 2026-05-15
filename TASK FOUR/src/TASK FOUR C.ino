/* 
DOCUMENTATION
This code is an altered version of my code from task 2. 
From the original I deleted the nyquist constraint and moving average
calculations, as well as stopping storing the calculated frequencies and magnitude
to free up valuable SRAM and allow for an array size of 180 samples. I also changed
the main struture of the code to carry out the apply_dft() and send_data_to_pc()
functions in the same one so that the global arrays were no longer necessary.
*/





// Loovee @ 2015-8-26
#include <Arduino.h>
#include <math.h>
const int B = 4275; // B value of the thermistor
int R0 = 100000; // R0 = 100k
const int pinTempSensor = A0; // Grove - Temperature Sensor connect to A0

int reading_interval_period = 100;//define a sample collection interval
float sampling_freq = 1000/ reading_interval_period;
const int array_size = 180;

int i;//defines the sample number of the data in the array
float reading_array[array_size];


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
            //Serial.print(i);
            //Serial.print(": ");
            //Serial.println(tempurature);
        }
    }
      Serial.println("Finished data collection");
}

void compute_dft_and_send() {
    sampling_freq = 1000.0 / reading_interval_period;
    float sample_period = reading_interval_period / 1000.0;

    // (Optional) DC removal — note your original computed `mean` but never
    // actually subtracted it, since the DFT sum used reading_array[n], not xn.
    float mean = 0.0;
    for (int i = 0; i < array_size; i++) mean += reading_array[i];
    mean /= array_size;

    Serial.println(F("Time,Temperature,Frequency,Magnitude"));

    for (int k = 0; k < array_size; k++) {
        float X_real = 0.0;
        float X_imag = 0.0;
        for (int n = 0; n < array_size; n++) {
            float angle = 2.0 * PI * k * n / array_size;
            float xn    = reading_array[n] - mean;       // now actually used
            X_real +=  xn * cos(angle);
            X_imag += -xn * sin(angle);
        }
        float magnitude_k = sqrt(X_real * X_real + X_imag * X_imag);
        float frequency_k = ((float)k * sampling_freq) / array_size;

        Serial.print(k * sample_period, 4);
        Serial.print(F(","));
        Serial.print(reading_array[k], 2);
        Serial.print(F(","));
        Serial.print(frequency_k, 4);
        Serial.print(F(","));
        Serial.println(magnitude_k, 4);
    }
}




void setup()
{
  Serial.begin(9600);
  Serial.println("Serial setup complete");
}

void loop(){
  collect_temperature_data();

  compute_dft_and_send();

 }