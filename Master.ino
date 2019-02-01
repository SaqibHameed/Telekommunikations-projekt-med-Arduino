#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// WiFi
const char* ssid = "TP-LINK_8E7A"; // Insert SSID for the accesspoint
const char* password = "41308561"; // Insert Wifi key/password

// UDP
WiFiUDP UDP;
int Port = 8888;
char Packet[255];

// BT
byte reading;
const int rx = D4;
const int tx = D3;
SoftwareSerial BTSerial(tx, rx);

// LED's
const int red = D8;
const int green = D6;
const int blue = D7;

int redVal = 255;
int greenVal = 255;
int blueVal = 255;

// Variables
boolean NL = true;
boolean MsState = true;
unsigned long startTime;
int interval;
int offTime = 10000;

//------------------------------------------------------------------------------------------------------------------------------
void setup() {
  delay(1000);

  Serial.begin(9600);

  // Setting up BT
  BTSerial.begin(9600);

  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());

  // Setting up UDP
  UDP.begin(Port);
  Serial.print("Setting up UDP on port number: ");
  Serial.println(Port);

  // LED's
  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(green, OUTPUT);
}
//------------------------------------------------------------------------------------------------------------------------------
void loop() {

  // UDP
  int packetSize = UDP.parsePacket();

  if (packetSize) {
    // receive incoming UDP packets
    Serial.print("Received packet from ip: ");
    Serial.println(UDP.remoteIP());
    Serial.print("on port: ");
    Serial.println(UDP.remotePort());

    if (UDP.peek() != 'L' && UDP.peek() != 'M') {
      redVal = UDP.parseInt();
      greenVal = UDP.parseInt();
      blueVal = UDP.parseInt();
      setColor(redVal, greenVal, blueVal);
      if (redVal == 0 && greenVal == 0 && blueVal == 0) {
        redVal = 255;
        greenVal = 255;
        blueVal = 255;
      }
      // UDP retur
      UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
      UDP.write("L1");
      UDP.endPacket();
    }

    int bufferSize = UDP.read(Packet, 255);
    if (bufferSize > 0) {
      Packet[bufferSize] = 0;
      if (Packet[0] == 'L') {
        if (Packet[1] == '1') {
          setColor(redVal, greenVal, blueVal);
          // UDP retur
          UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
          UDP.write("L1");
          UDP.endPacket();
        }
        else if (Packet[1] == '0') {
          setColor(0, 0, 0);
          // UDP retur
          UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
          UDP.write("L0");
          UDP.endPacket();
        }
      }
      else if (Packet[0] == 'M') {
        if (Packet[1] == '1') {
          MsState = true;
          // UDP retur
          UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
          UDP.write("M1");
          UDP.endPacket();
        }
        else if (Packet[1] == '0') {
          MsState = false;
          // UDP retur
          UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
          UDP.write("M0");
          UDP.endPacket();
        }
      }
      Serial.print("UDP packet contents: ");
      Serial.println(Packet);
    }
  }

  // Bluetooth
  if (BTSerial.available()) {
    reading = BTSerial.read();
    Serial.write(reading);
    if (reading == '1' && MsState == true) {
      setColor(redVal, greenVal, blueVal);
      startTime = millis();
    }
  }
  interval = millis() - startTime;

  if (interval == offTime) {
    setColor(0, 0, 0);
  }

  // Serial
  if (Serial.available()) {
    sendToBT(Serial.read());
  }
}

//------------------------------------------------------------------------------------------------------------------------------
// FUNCTIONS!!!
void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(red, redValue);
  analogWrite(green, greenValue);
  analogWrite(blue, blueValue);
}

void sendToBT(char command) {
  if (command != 10 && command != 13) {
    BTSerial.write(command);
  }
  if (NL) {
    Serial.print("\n>");
    NL = false;
  }
  Serial.write(command);
  if (command == 10) {
    NL = true;
  }
}
