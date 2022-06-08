// Uses the Teensy Audio Library and the Liquid Crystal I2C Library

#include <LiquidCrystal_I2C.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputUSB            usb2;           //xy=212,207
AudioInputAnalog         adc1;           //xy=212,304
AudioMixer4              mixer1;         //xy=365,220
AudioMixer4              mixer2;         //xy=371,320
AudioAnalyzeFFT1024      fft1024_1;      //xy=522,220
AudioAnalyzeFFT1024      fft1024_2;      //xy=523,320
AudioConnection          patchCord1(usb2, 0, mixer1, 0);
AudioConnection          patchCord2(usb2, 1, mixer2, 0);
AudioConnection          patchCord3(adc1, 0, mixer1, 1);
AudioConnection          patchCord4(adc1, 0, mixer2, 1);
AudioConnection          patchCord5(mixer1, fft1024_1);
AudioConnection          patchCord6(mixer2, fft1024_2);
// GUItool: end automatically generated code

int data;
int bin_offset = 1; 



uint8_t zero[] = {
  B00000, B00000, B00000, B00000, B00000, B00000, B00000, B00000
};

uint8_t one[] = {
  B00000, B00000, B00000, B00000, B00000, B00000, B00000, B11111
};

uint8_t two[] = {
  B00000, B00000, B00000, B00000, B00000, B00000, B11111, B11111
};

uint8_t three[] = {
  B00000, B00000, B00000, B00000, B00000, B11111, B11111, B11111
};

uint8_t four[] = {
  B00000, B00000, B00000, B00000, B11111, B11111, B11111, B11111
};

uint8_t five[] = {
  B00000, B00000, B00000, B11111, B11111, B11111, B11111, B11111
};

uint8_t six[] = {
  B00000, B00000, B11111, B11111, B11111, B11111, B11111, B11111
};

uint8_t seven[] = {
  B00000, B11111, B11111, B11111, B11111, B11111, B11111, B11111
};

uint8_t eight[] = {
  B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111
};




LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup()
{
  AudioMemory(32);
  analogReference(INTERNAL); 
  lcd.init();
  lcd.backlight();

  lcd.createChar(0, one);
  lcd.createChar(1, two);
  lcd.createChar(2, three);
  lcd.createChar(3, four);
  lcd.createChar(4, five);
  lcd.createChar(5, six);
  lcd.createChar(6, seven);
  lcd.createChar(7, eight);
  lcd.home();
  lcd.clear();

  mixer1.gain(1, 1.5);
  mixer2.gain(1, 1.5);  

  Serial.begin(57600);
  Serial.print("ping");
}

void loop()
{
  if (fft1024_1.available() == true) {
    lcd.setCursor(0, 0);
    for (int i = 0; i < 16; i ++) {
      data = constrain(fft1024_1.read(i) * 64.0, 0, 7);
      lcd.write(data);
    }
  }

  if (fft1024_2.available() == true) {
    lcd.setCursor(0, 1);
    for (int i = 0; i < 16; i ++) {
      data = constrain(fft1024_2.read(i) * 64.0, 0, 7); 
      lcd.write(data);
    }
  }
}
