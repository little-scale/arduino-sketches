

int count;
int beat;
int led = 13;
int sync_pin = 0;

void setup() {
  usbMIDI.setHandleClock(myClock);
  usbMIDI.setHandleStart(myStart);
  usbMIDI.setHandleContinue(myContinue);
  usbMIDI.setHandleStop(myStop);

  pinMode(led, OUTPUT);
  pinMode(sync_pin, OUTPUT);
}

void loop() {
  usbMIDI.read();
}

void myClock() {
  digitalWrite(led, 1 - (count / 12)); 
  digitalWrite(sync_pin, 1 - ((count / 6) & 1)); 
  count++;

  if (count > 23) {
    count = 0;
  }
  
}

void myStart() {
  count = 0; 

}
void myContinue() {
  count = 0; 
}
void myStop() {
  digitalWrite(led, LOW); 
}
