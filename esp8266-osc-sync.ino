#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>

char ssid[] = "*";          // your network SSID (name)
char pass[] = "*";                    // your network password

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
const IPAddress outIp(10,40,10,105);        // remote IP (not needed for receive)
const unsigned int outPort = 9999;          // remote port (not needed for receive)
const unsigned int localPort = 8888;        // local port to listen for UDP packets (here's where we send the packets)

OSCErrorCode error;

const int clock_pin = 2; 
const int start_pin = 0; 
const int trigger_length = 3;

void setup() {
  Serial.begin(115200);

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

  pinMode(clock_pin, OUTPUT); 
  pinMode(start_pin, OUTPUT); 

}

void loop() {
  OSCBundle bundle;
  int size = Udp.parsePacket();

  if (size > 0) {
    while (size--) {
      bundle.fill(Udp.read());
    }
    if (!bundle.hasError()) {
      bundle.dispatch("/clock", doClock);
      bundle.dispatch("/start", doStart);
    } else {
      error = bundle.getError();
      Serial.print("error: ");
      Serial.println(error);
    }
  }
}

void doClock(OSCMessage &msg) {
  Serial.print("ping"); 
  int clock_value = msg.getInt(0);
  digitalWrite(clock_pin, HIGH);
  Serial.print("/clock: ");
  Serial.println(clock_value);
  delay(trigger_length); 
  digitalWrite(clock_pin, LOW);
}

void doStart(OSCMessage &msg) {
  int start_value = msg.getInt(0);
  digitalWrite(start_pin, start_value);
  Serial.print("/start: ");
  Serial.println(start_value);
}
