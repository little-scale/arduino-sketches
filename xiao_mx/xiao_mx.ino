#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>

const int LED = 13;

Adafruit_USBD_MIDI usb_midi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

byte data;
byte cc_previous[5];
byte sw_previous[6];




void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  analogReadResolution(6);
  
  for(int i = 0; i < 6; i ++) {
    pinMode(5 + i, INPUT_PULLUP); 
  }

}

void loop() {
  MIDI.read();
  readPots();
  readSw(); 
}


void readPots() {
  for (int i = 0; i < 5; i ++) {
    data = analogRead(A0 + i) << 1;
    if (data != cc_previous[i]) {
      cc_previous[i] = data;
      MIDI.sendControlChange(1 + i, data, 1);
      delay(3); 
    }
  }
}

void readSw() {
  for (int i = 0; i < 6; i ++) {
    data = digitalRead(5 + i); 
    if(data != sw_previous[i]) {
      sw_previous[i] = data; 
      MIDI.sendNoteOn(60 + i, (1 - data) * 127, 1); 
      digitalWrite(LED, data); 
      delay(3); 
    }
  }
}
