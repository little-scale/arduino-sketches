#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Audio.h>
#include <SD.h>
#include <SerialFlash.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET 4
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// GUItool: begin automatically generated code
AudioInputUSB            usb2;           //xy=212,207
AudioInputAnalog         adc1;           //xy=212,304
AudioMixer4              mixer1;         //xy=365,220
AudioMixer4              mixer2;         //xy=371,320
AudioAnalyzeFFT1024      fft1024_1;      //xy=527,220
AudioAnalyzeFFT1024      fft1024_2;      //xy=527,320
AudioAnalyzePeak         peak1;          //xy=529,390
AudioConnection          patchCord1(usb2, 0, mixer1, 0);
AudioConnection          patchCord2(usb2, 1, mixer2, 0);
AudioConnection          patchCord3(adc1, 0, mixer1, 1);
AudioConnection          patchCord4(adc1, 0, mixer2, 1);
AudioConnection          patchCord5(mixer1, fft1024_1);
AudioConnection          patchCord6(mixer1, peak1);
AudioConnection          patchCord7(mixer2, fft1024_2);
// GUItool: end automatically generated code



int data; 
int counter; 

void setup() {
  AudioMemory(32);
  analogReference(INTERNAL); 

  mixer1.gain(1, 0); 
  mixer1.gain(1, 0); 
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for(;;); 
  }
  display.display();
 }

void loop() {
  if (fft1024_1.available() == true) {
    display.clearDisplay(); 
    for (int i = 0; i < 128; i ++) {
      data = constrain(fft1024_1.read(i) * 256.0, 0, 31);
      display.drawLine(i, 16 + (data / 2), i, 16 - (data / 2), SSD1306_WHITE); 
    }
    display.display(); 
  }
}
