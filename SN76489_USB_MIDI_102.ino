// SN76489 USB MIDI via Teensy board
// now with MIDI Input

/*
NEW PINOUT

==== DATA ====
Teensy PORTB 0 = Teensy Digital Pin 0 -->  SN76489 Pin 10 (Data 0)
Teensy PORTB 1 = Teensy Digital Pin 1 -->  SN76489 Pin 11 (Data 1)
Teensy PORTB 2 = Teensy Digital Pin 2 -->  SN76489 Pin 12 (Data 2)
Teensy PORTB 3 = Teensy Digital Pin 3 -->  SN76489 Pin 13 (Data 3)
Teensy PORTB 4 = Teensy Digital Pin 13 -->  SN76489 Pin 15 (Data 4)
Teensy PORTB 5 = Teensy Digital Pin 14 -->  SN76489 Pin 1 (Data 5)
Teensy PORTB 6 = Teensy Digital Pin 15 -->  SN76489 Pin 2 (Data 6)
Teensy PORTB 7 = Teensy Digital Pin 4 --> SN76489 Pin 3 (Data 7)

==== CONTROL AND MIDI INPUT ====
Teensy PORTD 0 = Teensy Digital Pin 5 --> SN76489 Pin 5 (Write Enable)
Teensy PORTD 2 = Teensy Digital Pin 7 --> Receive data from MIDI INPUT CIRCUIT 

==== POWER AND CRYSTAL ====
Teensy Ground --> SN76489 Pin 8 (ground)
Teensy Ground --> SN76489 Pin 6 (Output Enable)
Teensy Ground --> Xtal Osc Ground
Teensy 5V --> SN76489 Pin 16
Xtal Osc Signal --> SN76489 Pin 14 (Clock)

==== AUDIO OUTPUT ====
SN76489 Pin 7 --> Audio Output Signal
SN76489 Pin 8 --> Audio Output Ground

==== MIDI INPUT CIRCUIT (ONLY IF REQUIRED) ===
MIDI Input DIN 5 Pin 4 --> 4n28 Pin 1
MIDI Input DIN 5 Pin 5 --> 4n28 Pin 2
N4148 Diode (positive anode) --> 4n28 Pin 1
N4148 Diode (negative cathode) --> 4n28 Pin 2
4n28 Pin 6 --> 100k resistor (leg 1) 
100k resistor (leg 2) --> Teensy Ground
4n28 Pin 5 --> Teensy PORTD 2 = Teensy Digital Pin 7
4n28 Pin 5 --> 3.3k resistor (leg 1)
3.3k resistor (leg 1) --> Teensy 5V
4n28 pin 4 --> Teensy Ground

*/

long clock = 1843200; // crystal speed in MHZ

int wr = 5; 
int dT = 25;

int bendamount = 12; 

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
int pdatInt = 0;
byte pdat1 = 0;
byte pdat2 = 0;

int flag_mode = 1;

int flag_previous = 0; // keeps track of the previus MIDI byte type received 

/* flag_previous meanings: 
 -1 = note off status
 -2 = note off pitch
 0 = no action / waiting
 1 = note on status
 2 = pitch
 3 = cc status
 4 = cc number
 5 = bend status
 6 = bend lsb
 */

int pitchTable[] = 
{
  1008, 951, 898, 847, 800, 755, 713, 673, 635, 599, 566, 534, 504, 
  475, 449, 424, 400, 378, 356, 336, 317, 300, 283, 267, 252, 238, 
  224, 212, 200, 189, 178, 168, 159, 150, 141, 133, 126, 119, 112, 
  106, 100, 94, 89, 84, 79, 75, 71, 67, 63, 59, 56, 53, 50, 47, 45, 
  42, 40, 37, 35, 33, 31, 30, 28, 26, 25, 24, 22, 21, 20, 19, 18, 17, 
  16, 15, 14, 13, 13, 12, 11, 11, 10, 9, 9, 
  1017, 960, 906, 855, 807, 762, 719, 679, 641, 605, 571, 539, 508, 480, 
  453, 428, 404, 381, 360, 339, 320, 302, 285, 269, 254, 240, 226, 214, 
  202, 190, 180, 170, 160, 151, 143, 135, 127, 120, 113, 107, 101, 95, 
  90, 85, 80, 76, 71, 67, 64, 60, 57, 53, 50, 48, 45, 42, 40, 38, 36, 
  34, 32, 30, 28, 27, 25, 24, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 
  13, 12, 11, 11, 10, 9, 9
};

int bass_mode = 0;

int coarsePitch[] = 
{
  64, 64, 64, 64
};

int pitchTableOffset = 0;

// Arrays to hold running state data
byte bend_data[] = 
{
  64, 64, 64, 64
};

byte bend_MSB[] = 
{
  64, 64, 64, 64
};

byte pitchData[] = 
{
  0, 0, 0, 0
};

byte velocityData[] = 
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



// AM Modulation

byte amAccum[] = {
  0, 0, 0, 0
};

byte amSpeed[] = {
  10, 10, 10, 10
};

byte amModOn[] = {
  0, 0, 0, 0
};

byte amDepth[] = {
  0, 0, 0, 0
};

byte amFlip[] = {
  0, 0, 0, 0
};

// Vibrato Modulation

int vibAccum[] = {
  0, 0, 0, 0
};

byte vibSpeed[] = {
  10, 10, 10, 10
};

byte vibModOn[] = {
  0, 0, 0, 0
};

byte vibDepth[] = {
  0, 0, 0, 0
};

byte vibFlip[] = {
  0, 0, 0, 0
};

// setup


HardwareSerial Uart = HardwareSerial();

void setup() {
  Uart.begin(31250);
  
  usbMIDI.setHandleNoteOn(doNoteOn); 
  usbMIDI.setHandleControlChange(doCC); 
  usbMIDI.setHandleNoteOff(doNoteOff); 
  usbMIDI.setHandlePitchChange(doBend); 

  DDRB = DDRB | B11111111; // set the direction for PORTB

  pinMode(wr, OUTPUT); 


  
    for(int j = 0; j < 7; j ++) {
    doNoteOn(0, (j * 7) + 40, 100);
    doNoteOn(1, (j * 7) + 47, 100);
    doNoteOn(2, (j * 7) + 50, 100);
    doNoteOn(3, (j * 7) + 40, 100);
    for(int k = 0; k < 128; k++) {
    }
    delay(50 + (10 * j));
    doNoteOn(0, (j * 7) + 40, 0);
    doNoteOn(1, (j * 7) + 47, 0);
    doNoteOn(2, (j * 7) + 50, 0);
    doNoteOn(3, (j * 7) + 40, 0);
    for(int k = 0; k < 128; k++) {
    }
    delay(10);
  }

}


// Main Loop
void loop() {
  usbMIDI.read();
  doAM();
  doVib();
  if(Uart.available() > 0) {

    dataIn = Uart.read();
    if(dataIn < 0x80 && flag_previous == 0) {
      doMidiIn(rstat);
    }
    doMidiIn(dataIn);
  }
}




// Dealing with MIDI data

void doNoteOn(byte channel, byte pitch, byte velocity) {
  // NORMAL PULSE CHANNELS
  if(channel >= 0 && channel <= 2) {
    
    if(channel == 2 && bass_mode == 1) {
    {
      velocity = velocity / 3;
    }
    
    
    writeSN76489(B11100011); // clock by frequency
    
    if(velocity > 0) {
      int coarsePitchVal = -12 + (coarsePitch[channel] / 5); 
      pitchData[channel] = pitch;
      velocityData[channel + 1] = velocity;
      writeFrequency(pitch, channel);
      writeAmplitude(velocity, channel + 1);
    }

    else {
      velocityData[channel + 1] = 0;
      writeAmplitude(0, channel + 1);
    }
    
    channel = 17; 
  }
  
  else{
    
    
    if(velocity > 0) {
      int coarsePitchVal = -12 + (coarsePitch[channel] / 5); 
      pitchData[channel] = pitch;
      velocityData[channel] = velocity;
      writeFrequency(pitch, channel);
      writeAmplitude(velocity, channel);
    }

    else {
      velocityData[channel] = 0;
      writeAmplitude(0, channel);
    }
  }
  }

  // NOISE
  else if(channel == 3) {
    
    if(velocity > 0) {
      velocityData[channel] = velocity;
      writeSN76489(noiseLookup[pitch % 12]);
      writeAmplitude(velocity, channel);
    }

    else {
      velocityData[channel] = 0;
      writeAmplitude(0, channel);
    }
  }   
}


void doNoteOff(byte channel, byte pitch, byte velocity) {
    velocityData[channel] = 0;
    writeAmplitude(0, channel);
}



void doCC(byte channel, byte ccnumber, byte ccvalue) {

  if(ccnumber == 83 && channel == 0) {
    if(ccvalue > 63) {
      pitchTableOffset = 83;
    }
    else{
      pitchTableOffset = 0;
    }
  }

  // MIDI CC 42 = coarse tuning
  else if(ccnumber == 42 && channel >= 0 && channel <= 3) {
    coarsePitch[channel] = ccvalue; 
    writeFrequency(pitchData[channel], channel);
  }



  // MIDI CC 70 = AM On
  else if(ccnumber == 70 && channel >= 0 && channel <= 3) {
    amModOn[channel] = ccvalue >> 6; 
  }

  // MIDI CC 18 = AM Speed
  else if(ccnumber == 18 && channel >= 0 && channel <= 3) {
    amSpeed[channel] = ccvalue; 
  }

  // MIDI CC 19 = AM Depth
  else if(ccnumber == 19 && channel >= 0 && channel <= 3) {
    amDepth[channel] = ccvalue; 
  }

  // MIDI CC 71 = Vib On
  else if(ccnumber == 71 && channel >= 0 && channel <= 2) {
    vibModOn[channel] = ccvalue >> 6; 
  }

  // MIDI CC 20 = Vib Speed
  else if(ccnumber == 20 && channel >= 0 && channel <= 2) {
    vibSpeed[channel] = ccvalue; 
  }

  // MIDI CC 21 = Vib Depth
  else if(ccnumber == 21 && channel >= 0 && channel <= 2) {
    vibDepth[channel] = ccvalue; 
  }

  // MIDI CC 11 = expression
  else if(ccnumber == 11 && channel >= 0 && channel <= 3) {
    velocityData[channel] = ccvalue;
    writeAmplitude(ccvalue, channel);
  }
  
  else if(ccnumber == 81 && channel >= 0 && channel <= 3) {
    bendamount = ccvalue; 
  }

  // MIDI CC 78 = sample mode on
  
    // MIDI CC 86 = sample playback speed
  
      // MIDI CC 85 = bass mode
  else if(ccnumber == 85) {
    bass_mode = ccvalue >> 6;
  }
  
  }



void doBend(byte channel, unsigned int bend_usb) {
  bendMSB = bend_usb >> 7; 
  bendLSB = bend_usb % B01111111; 
  if(channel >=0 && channel <= 2) {
    bend_MSB[channel] = bendMSB;
    bend_data[channel] = bend_MSB[channel];
    writeFrequency(pitchData[channel], channel);
  }
}






////////////////////////////////////////////////////////
//
//    DEALING WITH THE SN76489
//
////////////////////////////////////////////////////////

void writeSN76489(byte data) {
  PORTB = data; 
  delayMicroseconds(dT);
  digitalWrite(wr, LOW);
  delayMicroseconds(dT);
  digitalWrite(wr, HIGH);
  delayMicroseconds(dT);
  
}




void writeAmplitude(byte velocity, byte channel) {
  // latch byte
  channel = channel << 5;
  velocity = 15 - (velocity >> 3);
  working_byte = B10010000 + channel + velocity;
  writeSN76489(working_byte);
}

void writeFrequency(byte pitch, byte channel) {
  // latch byte
  int pdaInt;
  float frequency; 
  frequency = 440.00 * pow(2, (pitch + map(bend_data[channel], 0, 127, bendamount * -1, bendamount) - 69.00) / 12.00);
  frequency = clock / (frequency * 32.00);
   
  
  pdatInt = frequency;
  if(pdatInt < 0) {
    pdatInt = 0;
  }
  else if(pdatInt > 1023) {
    pdatInt = 1023;
  }
  pdat1 = pdatInt % 16;
  channel = channel << 5;
  working_byte = B10000000 | channel | pdat1;
  writeSN76489(working_byte);

  // data byte
  pdat2 = (pdatInt >> 4) & B00111111;
  writeSN76489(pdat2);
}


void doVib() {
  for(int i = 0; i < 3; i ++) {
    if(velocityData[i] > 0 && vibModOn[i] == 1) {

      if(vibAccum[i] < vibSpeed[i] * 10) {
        vibAccum[i] = vibAccum[i] + 1;
      }

      else if(vibAccum[i] > vibSpeed[i] * 10) {
        vibAccum[i] = 0;
      }

      if(vibFlip[i] == 0 && vibAccum[i] == (vibSpeed[i] * 10)) {
        bend_data[i] = bend_MSB[i];
        writeFrequency(pitchData[i], channel);
        vibFlip[i] = 1;
        vibAccum[i] = 0;
      }

      else if(vibFlip[i] == 1 && vibAccum[i] == (vibSpeed[i] * 10)) {
        bend_data[i] = bend_MSB[i] + vibDepth[i];
        writeFrequency(pitchData[i], channel);
        vibFlip[i] = 0;
        vibAccum[i] = 0;
      }
    }
  }
}


void doAM() {

  for(int i = 0; i < 4; i ++) {
    if(velocityData[i] > 0 && amModOn[i] == 1) {

      if(amAccum[i] < amSpeed[i]) {
        amAccum[i] = amAccum[i] + 1;
      }

      else if(amAccum[i] > amSpeed[i]) {
        amAccum[i] = 0;
      }

      if(amFlip[i] == 0 && amAccum[i] == amSpeed[i]) {
        writeAmplitude(amDepth[i], i);
        amFlip[i] = 1;
        amAccum[i] = 0;
      }

      else if(amFlip[i] == 1 && amAccum[i] == amSpeed[i]) {
        writeAmplitude(velocityData[i], i);
        amFlip[i] = 0;
        amAccum[i] = 0;
      }
    }
  }
}




void doMidiIn(byte data) {
     
    // running status set
    
   if((data >= 0x80) && (data < 0xf0) && (flag_previous == 0)) {
     rstat = data;
   }
    
    // deal with note on
    if((data >= 0x90) && (data < 0xa0) && (flag_previous == 0)) {
      channel = data & B00001111;
      flag_previous = 1;
    }
    else if((data < 0x80) && (flag_previous == 1)) {      
      pitch = data;
      flag_previous = 2;
    }
    else if((data < 0x80) && (flag_previous == 2)) {
      velocity = data;
      doNoteOn(channel, pitch, velocity);
      flag_previous = 0;
    }
    // done with note on

    // deal with note off (as discrete status byte)
    else if((data >= 0x80) && (data < 0x90) && (flag_previous == 0)) {
      channel = data & B00001111;
      flag_previous = -1;
    }
    else if((data < 0x80) && (flag_previous == -1)) {
      pitch = data;
      flag_previous = -2;
    }
    else if((data < 0x80) && (flag_previous == -2)) {
      velocity = data;
      doNoteOff(channel, pitch, velocity);
      flag_previous = 0;
    }
    // done with note off (as discrete status byte)

    // deal with cc data
    else if((data >= 0xb0) && (data < 0xc0) && (flag_previous == 0)) {
      channel = data & B00001111;
      flag_previous = 3;
    }
    else if((data < 0x80) && (flag_previous == 3)) {
      ccnumber = data;
      flag_previous = 4;
    }
    else if((data < 0x80) && (flag_previous == 4)) {
      ccvalue = data;
        doCC(channel, ccnumber, ccvalue);
      flag_previous = 0;
    }
    // done with cc data

    // deal with bend data
    else if((data >= 0xe0) && (data < 0xf0) && (flag_previous == 0)) {
      channel = data & B00001111;
      flag_previous = 5;
    }
    else if((data < 0x80) && (flag_previous == 5)) {
      bendLSB = data;
      flag_previous = 6;
    }
    else if((data < 0x80) && (flag_previous == 6)) {
      bendMSB = data;
      doBend(channel, bendLSB + (bendMSB << 7));
      flag_previous = 0;
    }
    // done with bend data
    
}
