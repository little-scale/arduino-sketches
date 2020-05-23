int LED = 13;

void setup() {
  usbMIDI.setHandleStart(myStart);
  usbMIDI.setHandleStop(myStop);
  usbMIDI.setHandleContinue(myContinue);
  pinMode(LED, OUTPUT); 
}

void loop() {
  usbMIDI.read();
}

void myStart() {
  digitalWrite(LED, HIGH); 
}

void myStop() {
  digitalWrite(LED, LOW); 
}

void myContinue() {
  digitalWrite(LED, HIGH); 
}
