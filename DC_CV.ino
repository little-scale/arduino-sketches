const int PWM1 = 35;
const int PWM2 = 36;
const int GATE1 = 0;
const int GATE2 = 1;
float value1;
float value2;
float value1_previous;
float value2_previous;


#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputUSB            usb2;           //xy=207.66667556762695,85.5555591583252
AudioAmplifier           amp2;           //xy=366.6666611565484,275.55554495917426
AudioSynthWaveformDc     dc2;            //xy=367.77777099609375,319.99997329711914
AudioOutputAnalogStereo  dacs1;          //xy=369.11113357543945,85.55555725097656
AudioAmplifier           amp1;           //xy=369.33334732055664,134.22221565246582
AudioSynthWaveformDc     dc1;            //xy=369.33334732055664,173.22221565246582
AudioMixer4              mixer2;         //xy=504.4444160461426,327.77772331237793
AudioMixer4              mixer1;         //xy=506.3333435058594,179.2222080230713
AudioAnalyzePeak         peak1;          //xy=642.0000648498535,178.88890266418457
AudioAnalyzePeak         peak2;          //xy=644.444408416748,326.6666898727417
AudioConnection          patchCord1(usb2, 0, dacs1, 0);
AudioConnection          patchCord2(usb2, 0, amp1, 0);
AudioConnection          patchCord3(usb2, 1, dacs1, 1);
AudioConnection          patchCord4(usb2, 1, amp2, 0);
AudioConnection          patchCord5(amp2, 0, mixer2, 0);
AudioConnection          patchCord6(dc2, 0, mixer2, 1);
AudioConnection          patchCord7(amp1, 0, mixer1, 0);
AudioConnection          patchCord8(dc1, 0, mixer1, 1);
AudioConnection          patchCord9(mixer2, peak2);
AudioConnection          patchCord10(mixer1, peak1);
// GUItool: end automatically generated code






void setup() {
  AudioMemory(32);
  analogReference(EXTERNAL);
  
  pinMode (GATE1, OUTPUT);
  pinMode (GATE2, OUTPUT);

  dc1.amplitude(0.5);
  amp1.gain(0.5);
  mixer1.gain(0, 1.0);
  mixer1.gain(1, 1.0);

  dc2.amplitude(0.5);
  amp2.gain(0.5);
  mixer2.gain(0, 1.0);
  mixer2.gain(1, 1.0);
  
}

void loop() {
  if (peak1.available() == true) {
    value1 = peak1.read();
    analogWrite(PWM1, value1 * 255.0);
    digitalWriteFast(GATE1, value1 * 2); 
  }

  if (peak2.available() == true) {
    value2 = peak2.read();
    analogWrite(PWM2, value2 * 255.0);
    digitalWriteFast(GATE2, value2 * 2); 
  }

  if (value1_previous != value1 || value2_previous != value2) {
    value1_previous = value1;
    value2_previous = value2;
    Serial.print(value1 * 10.0); 
    Serial.print(","); 
    Serial.println(value2 * 10.0); 
  }
}
