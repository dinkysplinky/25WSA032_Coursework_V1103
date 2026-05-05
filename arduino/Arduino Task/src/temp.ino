// Loovee @ 2015-8-26
#include <Arduino.h>
#include <math.h>
const int B = 4275000; // B value of the thermistor
int R0 = 100000; // R0 = 100k
const int pinTempSensor = A0; // Grove - Temperature Sensor connect to A0
float temperature;
unsigned long current_time;

const int reading_interval_period = 100;//define a sample collection interval 
const float sampling_freq = 1000/ reading_interval_period;

unsigned long prev_reading_time = 0;
const int array_size = 100;
float reading_array[array_size];
int i;//defines the sample number of the data in the array

float X_real[array_size];            // dft real components
float X_imag[array_size];            // dft imaginary components

void collect_temperature_data() {//define a new function to collect data from the sensor and calculate temp 

  while(current_time <= (array_size + 1) * reading_interval_period){//determine when to collect data using millis clock 
    current_time = millis();
     if(prev_reading_time + reading_interval_period < current_time){//check if enough time has passed to take a reading 
      int a = analogRead(pinTempSensor);
      float R = 1023.0/a-1.0;
      R = R0*R;
      temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15; // convert to temperature via datasheet
      reading_array[i] = temperature;
      Serial.println(reading_array[i]);
      i++;
      prev_reading_time = current_time;}}//reset prev time for next loop
    
    for (int j = 0; j < array_size; j++) {//prints out the whole array 
        Serial.print("Reading ");
        Serial.print(j+1);
        Serial.print(": ");
        Serial.println(reading_array[j]);}
}

void apply_dft() {//define a new function to apply dft to the collected temp data
    for (int k = 0; k < array_size; k++) {          // For each frequency bin
    X_real[k] = 0.0;
    X_imag[k] = 0.0;

    for (int n = 0; n < array_size; n++) {        // Sum over all samples
      float angle = 2.0 * PI * k * n / array_size;
      X_real[k] +=  reading_array[n] * cos(angle);
      X_imag[k] += -reading_array[n] * sin(angle);
    }
  }

    // --- Compute and print the magnitude of each frequency bin ---
  //float sampleRate = 1000.0;  // Hz — must match your actual sample interval
  float freqResolution = reading_interval_period / array_size;

  Serial.println("Bin\tFrequency(Hz)\tMagnitude");
  for (int k = 0; k < array_size / 2; k++) {   // Only first N/2 bins are meaningful
    float magnitude = sqrt(X_real[k] * X_real[k] + X_imag[k] * X_imag[k]);
    float frequency = k * freqResolution;
    Serial.print(k);
    Serial.print("\t");
    Serial.print(frequency);
    Serial.print("\t\t");
    Serial.println(magnitude);
  }


}


void setup()
{
  Serial.begin(9600);
  Serial.println("Serial setup complete");
}

void loop()
{
  current_time = millis();

  collect_temperature_data();

  Serial.println("Finished data collection");

  apply_dft();

  delay(500000);
 }
