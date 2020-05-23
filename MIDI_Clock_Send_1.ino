float bpm = 110.0; 
float delay_time = (60000000 / bpm) / 24; 


void setup() {
  delay(3000); 
  usbMIDI.sendRealTime(usbMIDI.Start);
}

void loop() {
  usbMIDI.sendRealTime(usbMIDI.Clock);
  delayMicroseconds(delay_time); 
}
