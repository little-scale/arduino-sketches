#include <Bounce2.h>

float bpm;
float delay_time;
int play_flag;
int pushButton_pin = 32;
int debounce_time = 10;

Bounce pushButton = Bounce();

void setup() {
  pinMode(pushButton_pin, INPUT_PULLUP);
  pushButton.attach(pushButton_pin);
  pushButton.interval(debounce_time);
  delay(3000);

}

void loop() {
  pushButton.update();

  if (pushButton.fell() == true) {
    play_flag = 1 - play_flag;
    
    if (play_flag == 1) {
      usbMIDI.sendRealTime(usbMIDI.Start);
    }
    else {
      usbMIDI.sendRealTime(usbMIDI.Stop);
    }
  }

  if (play_flag == 1) {
    bpm = analogRead(A14);
    bpm = map(bpm, 0, 1023, 50, 200);
    delay_time = (60000000 / bpm) / 24;
    usbMIDI.sendRealTime(usbMIDI.Clock);
    delayMicroseconds(delay_time);
  }
}
