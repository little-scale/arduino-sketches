#include <MIDI.h>        // access to serial (5 pin DIN) MIDI
#include <USBHost_t36.h> // access to USB MIDI devices (plugged into 2nd USB port)

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI1);

USBHost myusb;
USBHub hub1(myusb);

MIDIDevice midi01(myusb);

elapsedMillis ledOnMillis;

void setup() {
  pinMode(13, OUTPUT); // LED pin
  digitalWrite(13, LOW);
  MIDI1.begin(MIDI_CHANNEL_OMNI);

  delay(1500);
  delay(10);
  myusb.begin();
}

void loop() {
  bool activity = false;

  if (MIDI1.read()) { // 5 pin DIN to USB Host
    sendToHost(MIDI1.getType(), MIDI1.getData1(), MIDI1.getData2(), MIDI1.getChannel(), MIDI1.getSysExArray());
    activity = true;
  }

  if (midi01.read()) { // USB Host to 5 pin DIN
    uint8_t type =       midi01.getType();
    uint8_t data1 =      midi01.getData1();
    uint8_t data2 =      midi01.getData2();
    uint8_t channel =    midi01.getChannel();
    const uint8_t *sys = midi01.getSysExArray();
    sendToMIDI(type, data1, data2, channel, sys);

    activity = true;
  }

  if (activity) {
    digitalWriteFast(13, HIGH); // LED on
    ledOnMillis = 0;
  }
  if (ledOnMillis > 15) {
    digitalWriteFast(13, LOW);  // LED off
  }

}

void sendToMIDI(byte type, byte data1, byte data2, byte channel, const uint8_t *sysexarray)
{
  if (type != midi::SystemExclusive) {
    MIDI1.send(type, data1, data2, channel);
  } else {
    unsigned int SysExLength = data1 + data2 * 256;
    MIDI1.sendSysEx(SysExLength, sysexarray, true);
  }
}

void sendToHost(byte type, byte data1, byte data2, byte channel, const uint8_t *sysexarray)
{
  if (type != midi::SystemExclusive) {
    midi01.send(type, data1, data2, channel);
  } else {
    unsigned int SysExLength = data1 + data2 * 256;
    midi01.sendSysEx(SysExLength, sysexarray, true);
  }
}
