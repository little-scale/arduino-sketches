int LED = 13;
int count; 
int pitch1; 
int pitch2; 
int velocity = 127; 
int channel = 1; 

void setup() {
  usbMIDI.setHandleStart(myStart);
  usbMIDI.setHandleStop(myStop);
  usbMIDI.setHandleContinue(myContinue);
  usbMIDI.setHandleClock(myClock); 
  pinMode(LED, OUTPUT); 
}

void loop() {
  usbMIDI.read();
}

void myStart() {
  count = 0; 
}

void myStop() {
  digitalWrite(LED, LOW); 
}

void myContinue() {
  count = 0; 
}

void myClock() {
  digitalWrite(LED, 1 - (count / 12)); 

  if(count % 12 == 0) {
    pitch1 = analogRead(A14); 
    pitch1 = map(pitch1, 0, 1023, 48, 72); 
    usbMIDI.sendNoteOn(pitch1, velocity, channel); 
  }

  else if(count % 12 == 3) {
    usbMIDI.sendNoteOff(pitch1, 0, channel); 
  }

  else if(count % 12 == 6) {
    pitch2 = analogRead(A15); 
    pitch2 = map(pitch2, 0, 1023, 48, 72); 
    usbMIDI.sendNoteOn(pitch2, velocity, channel); 
  }

  else if(count % 12 == 9) {
    usbMIDI.sendNoteOff(pitch2, 0, channel); 
  }

  count++; 
  
  if(count > 23) {
    count = 0;
  }
}
