#include <HCSR04.h>

float data;
byte current_value;
byte previous_value;
const int channel = 1;
const int controller = 1;

UltraSonicDistanceSensor distanceSensor(0, 1);

void setup () {

}

void loop () {
  usbMIDI.read(); 
  
  data = distanceSensor.measureDistanceCm();

  if (data >= 2.0 && data <= 32.0) {

    current_value = map(data, 2.0, 32.0, 0, 127);

    if (current_value != previous_value) {
      previous_value = current_value;
      usbMIDI.sendControlChange(controller, current_value, channel);
    }

    delay(5);
  }
}
