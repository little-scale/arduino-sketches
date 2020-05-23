int LED = 13;
int count; 

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
  count++; 
  
  if(count > 23) {
    count = 0;
  }
}
