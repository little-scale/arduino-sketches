#include <mtof.h>

// SN76489 USB MIDI via Teensy board

/*  Pin 0 to SN76489 write enable 
 *  Pin 3 to SN76489 clock input
 *  Pin 4 - 11 to SN76489 D0 to D7
 *  
 *  Teensy 5V to SN76489 5V
 *  Teensy ground to SN76489 ground and output enable
 *  
 */
int wr = 0;
int dT = 25;

byte data; // general working byte for serially-received data
byte dataIn;
byte rstat;
byte working_byte = 0; // working byte for the SN76489 stuff

byte channel;
byte pitch;
byte velocity;
byte ccnumber;
byte ccvalue;
byte bendMSB;
byte bendLSB;
byte white; 
int flag_mode = 1;

int flag_previous = 0; // keeps track of the previus MIDI byte type received

const long input_clock = 3000000;
const long clock_resolution = 4; 
const int clock_pin = 3; 

int output_port[] = {
  4, 5, 6, 7, 8, 9, 10, 11
};

int bass_mode = 0;

// Arrays to hold running state data
float bend_data[] =
{
  0.0, 0.0, 0.0, 0.0
};

byte pitchData[] =
{
  0, 0, 0, 0
};


byte noiseLookup[] = {
  B11100000, // C
  B11100000, // C#
  B11100001, // D
  B11100001, // D#
  B11100010, // E
  B11100100, // F
  B11100101, // F#
  B11100110, // G
  B11100110, // G#
  B11100011, // A
  B11100011, // A#
  B11100111, // B
};



void setup() {
  pinMode(clock_pin, OUTPUT); 
  analogWriteResolution(clock_resolution); 
  analogWriteFrequency(clock_pin, input_clock); 
  analogWrite(clock_pin, 12);
  
  
  usbMIDI.setHandleNoteOn(doNoteOn);
  usbMIDI.setHandleControlChange(doCC);
  usbMIDI.setHandleNoteOff(doNoteOff);
  usbMIDI.setHandlePitchChange(doBend);

  // DDRB = DDRB | B00001111; // set the direction for PORTB
  // DDRD = DDRD | B00001111;

  for (int i = 0; i < 8; i ++) {
    pinMode(output_port[i], OUTPUT);
  }

  pinMode(wr, OUTPUT);

  for (int j = 0; j < 7; j ++) {
    doNoteOn(1, (j * 7) + 40, 100);
    doNoteOn(2, (j * 7) + 47, 100);
    doNoteOn(3, (j * 7) + 50, 100);
    doNoteOn(4, (j * 7) + 40, 100);
    for (int k = 0; k < 128; k++) {
    }
    delay(50 + (10 * j));
    doNoteOn(1, (j * 7) + 40, 0);
    doNoteOn(2, (j * 7) + 47, 0);
    doNoteOn(3, (j * 7) + 50, 0);
    doNoteOn(4, (j * 7) + 40, 0);
    for (int k = 0; k < 128; k++) {
    }
    delay(10);
  }
}

void loop() {
  usbMIDI.read();
}

void doNoteOn(byte channel, byte pitch, byte velocity) {
  channel = channel - 1;
  // NORMAL PULSE CHANNELS
  if (channel >= 0 && channel <= 2) {

    if (channel == 2 && bass_mode == 1) {
      velocity = velocity / 3;
      writeSN76489(B11100011 | white); // clock by frequency
      if (velocity > 0) {
        pitchData[channel] = pitch;
        writeFrequency(pitch, channel);
        writeAmplitude(velocity, channel + 1);
      }
      else {
        writeAmplitude(0, channel + 1);
      }
      channel = 17;
    }
    else {
      if (velocity > 0) {
        pitchData[channel] = pitch;
        writeFrequency(pitch, channel);
        writeAmplitude(velocity, channel);
      }

      else {
        writeAmplitude(0, channel);
      }
    }
  }
  else if (channel == 3) {
    if (velocity > 0) {
      writeSN76489(noiseLookup[pitch % 12]);
      writeAmplitude(velocity, channel);
    }

    else {
      writeAmplitude(0, channel);
    }
  }
}


void doNoteOff(byte channel, byte pitch, byte velocity) {
  channel = channel - 1;
  if (channel == 2 && bass_mode == 1) {
    writeAmplitude(0, channel + 1);
  }
  else {
    writeAmplitude(0, channel);
  }
}

void doCC(byte channel, byte ccnumber, byte ccvalue) {
  if (ccnumber == 85) {
    bass_mode = ccvalue >> 6;
  }

  else if (ccnumber == 86) {
    if(ccvalue > 63) {
      white = B00000100;
    }
    else {
      white = 0; 
    }
  }
}

void doBend(byte channel, int bend_usb) {
  channel = channel - 1;
  if (channel >= 0 && channel <= 2) {
    bend_data[channel] = map(bend_usb, -8192, 8191, -12.0, 12.0);
    writeFrequency(pitchData[channel], channel);
  }
}

void writeSN76489(byte data) {
  for (int i = 0; i < 8; i ++) {
    digitalWriteFast(output_port[i], bitRead(data, i));
  }
  delayMicroseconds(dT);
  digitalWrite(wr, LOW);
  delayMicroseconds(dT);
  digitalWrite(wr, HIGH);
  delayMicroseconds(dT);

}

void writeAmplitude(byte velocity, byte channel) {
  channel = channel << 5;
  velocity = 15 - (velocity >> 3);
  working_byte = B10010000 + channel + velocity;
  writeSN76489(working_byte);
}

void writeFrequency(byte pitch, byte channel) {
  int pitch_data;
  byte pitch_data_high;
  byte pitch_data_low;

  pitch_data = input_clock / mtof.toFrequency(pitch + bend_data[channel]) / 32;
  pitch_data = constrain(pitch_data, 0, 1023);

  pitch_data_low = pitch_data % 16;
  channel = channel << 5;
  pitch_data_low = B10000000 | channel | pitch_data_low;
  writeSN76489(pitch_data_low);

  pitch_data_high = (pitch_data >> 4) & B00111111;
  writeSN76489(pitch_data_high);
}
