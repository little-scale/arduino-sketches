#include <Bounce2.h>
#include <HCSR04.h>


float data;
byte note;
const int channel = 1;
const int velocity = 127;

Bounce pushButton = Bounce();

int pushButton_pin = 32;
int debounceTime = 10;

UltraSonicDistanceSensor distanceSensor(0, 1);

void setup () {
  pinMode(pushButton_pin, INPUT_PULLUP);
  pushButton.attach(pushButton_pin);
  pushButton.interval(debounceTime);

}

void loop () {
  usbMIDI.read();
  pushButton.update();

  if (pushButton.fell() == true) {
    data = distanceSensor.measureDistanceCm();
    note = map(data, 2.0, 32.0, 60, 72);
    usbMIDI.sendNoteOn(note, velocity, channel);
  }
  
  if (pushButton.rose() == true) {
    usbMIDI.sendNoteOff(note, 0, channel);
  }
}
