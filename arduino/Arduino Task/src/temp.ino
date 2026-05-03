// Loovee @ 2015-8-26
#include <Arduino.h>
#include <math.h>
const int B = 4275000; // B value of the thermistor
const int R0 = 100000; // R0 = 100k
const int pinTempSensor = A0; // Grove - Temperature Sensor connect to A0
float temperature;
unsigned long current_time;

unsigned long prev_reading_time = 0;
const int array_size = 15;
float reading_array[array_size];
int i;//defines the sample number of the data in the array

void collect_temperature_data() {//define a new function to collect data from the sensor and calculate temp 
  const int reading_interval_period = 1000;

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



void setup()
{
  Serial.begin(9600);
}

void loop()
{
  current_time = millis();
  collect_temperature_data();
  Serial.println("Finished data collection");
 }
