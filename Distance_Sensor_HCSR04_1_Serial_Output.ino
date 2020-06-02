#include <HCSR04.h>

float data; 

UltraSonicDistanceSensor distanceSensor(0, 1); 

void setup () {
    Serial.begin(9600);  }

void loop () {
    data = distanceSensor.measureDistanceCm(); 

    if(data > 2.0) {
      Serial.println(data);
      delay(100);
    }
}
