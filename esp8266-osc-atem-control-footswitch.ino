#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

char ssid[] = "OliveNet";          // your network SSID (name)
char pass[] = "kirapoops";                    // your network password

int footswitch = 13;
byte current_value;
byte previous_value;
int debounce = 10;
int blue = 2; 

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
const IPAddress outIp(192, 168, 1, 10);     // remote IP of your computer
const unsigned int outPort = 9999;          // remote port to receive OSC
const unsigned int localPort = 2390;        // local port to listen for OSC packets (not used/tested)

void setup() {
  Serial.begin(115200);

  pinMode(footswitch, INPUT_PULLUP);
  pinMode(blue, OUTPUT); 

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());

}

void loop() {
  OSCMessage msg("/atem/transition/cut");
  current_value = digitalRead(footswitch);
  if (current_value != previous_value) {
    digitalWrite(blue, current_value); 
    delay(debounce);
    previous_value = current_value;
    if (current_value == 0) {
      Udp.beginPacket(outIp, outPort);
      msg.send(Udp); // send the bytes to the SLIP stream
      Udp.endPacket(); // mark the end of the OSC Packet
      msg.empty(); // free space occupied by message
    }

  }
}
