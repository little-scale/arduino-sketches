#include <OSCMessage.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include <OSCMessage.h>

EthernetUDP Udp;

IPAddress ip(192, 168, 1, 100);
IPAddress outIp(192, 168, 1, 9);
const unsigned int outPort = 3333;

int nres = 9; // reset pin on Wiz820io

int footswitch = 0;
byte current_value;
byte previous_value;
int debounce = 100; 

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

void setup() {
  // initialise the Wiz820io
  pinMode(nres, OUTPUT);
  digitalWrite(nres, LOW);
  delay(250);
  digitalWrite(nres, HIGH);
  delay(1000);

  // setup footswitch pin
  pinMode(footswitch, INPUT_PULLUP);

  // start Ethernet connection
  Ethernet.begin(mac, ip);
  Udp.begin(4444);

}

void loop() {
  OSCMessage autocross("/atem/transition/auto");
  current_value = digitalRead(footswitch);
  if (current_value != previous_value) {
    delay(debounce);
    previous_value = current_value;
    if (current_value == 1) {
      Udp.beginPacket(outIp, outPort);
      autocross.send(Udp); // send the bytes to the SLIP stream
      Udp.endPacket(); // mark the end of the OSC Packet
      autocross.empty(); // free space occupied by message
    }
    
  }
}
