const byte octave_down = 36;
const byte octave_up = 33;
const byte buttons[8] = {
  1, 4, 7, 10, 12, 26, 29, 32
};

const byte debounce = 10;

const byte scale[] = {
  0, 2, 4, 5, 7, 9, 11, 12
};

byte octave = 5;
byte const channel = 1;
byte const velocity = 127;

byte current[10];
byte previous[10];


void setup() {
  for (int i = 0; i < 8; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
  }
  pinMode(octave_down, INPUT_PULLUP);
  pinMode(octave_up, INPUT_PULLUP);
}

void loop() {
  for (int i = 0; i < 8; i ++) {
    current[i] = digitalReadFast(buttons[i]);
    if (current[i] != previous[i]) {
      previous[i] = current[i];
      usbMIDI.sendNoteOn((octave * 12) + scale[i], (1 - current[i]) * velocity, channel);
      delay(debounce);
    }
  } 

  current[8] = digitalReadFast(octave_down);
  if (current[8] != previous[8]) {
    previous[8] = current[8];
    if (!current[8]) {
      octave = (octave - 1) % 10;
      delay(debounce);
    }
  }

  current[9] = digitalReadFast(octave_up);
  if (current[9] != previous[9]) {
    previous[9] = current[9];
    if (!current[9]) {
      octave = (octave + 1) % 10;
      delay(debounce);
    }
  }

}
