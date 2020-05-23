float bpm; 
float delay_time; 

void setup() {
  delay(3000); 
  usbMIDI.sendRealTime(usbMIDI.Start);
}

void loop() {
  bpm = analogRead(A14); 
  bpm = map(bpm, 0, 1023, 50, 200); 
  delay_time = (60000000 / bpm) / 24; 
  usbMIDI.sendRealTime(usbMIDI.Clock);
  delayMicroseconds(delay_time); 
}
