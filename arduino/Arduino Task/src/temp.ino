// Loovee @ 2015-8-26
#include <Arduino.h>
#include <math.h>
const int B = 4275000; // B value of the thermistor
const int R0 = 100000; // R0 = 100k
const int pinTempSensor = A0; // Grove - Temperature Sensor connect to A0
float temperature;

unsigned long prev_reading_time = 0;
const int array_size = 10;
float reading_array[array_size];
int i;//defines the place number of the data in the array

void collect_temperature_data() {//define a new function to collect data from the sensor and calculate temp 
  int a = analogRead(pinTempSensor);
  float R = 1023.0/a-1.0;
  R = R0*R;
  temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15; // convert to temperature via datasheet
  reading_array[i] = temperature;
  Serial.println(reading_array[i]);
  i++;
}



void setup()
{
  Serial.begin(9600);
}

void loop()
{
  unsigned long current_time = millis();
  const int reading_interval_period = 1000;
  if(current_time <= (array_size + 1) * reading_interval_period){//determine when to collect data using millis clock 
    if(prev_reading_time + reading_interval_period < current_time){//check if enough time has passed to take a reading 
    collect_temperature_data();
    prev_reading_time = current_time;//reset prev time for next loop
  }  
 }
 else {//print out the araray one data point at a time
    for (int j = 0; j < array_size; j++) {
        Serial.print("Reading ");
        Serial.print(j+1);
        Serial.print(": ");
        Serial.println(reading_array[j]);}
        delay(50000);//pause the loop for reading
 }
}