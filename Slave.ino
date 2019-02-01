#include<SoftwareSerial.h>

// Pins
const int Rx = D2;
const int Tx = D1;
const int Ms = D8;

// Variables
byte Value = 0;
char input = ' ';
boolean NL = true;
byte State;

// BT
SoftwareSerial BTSerial(Tx, Rx); // TX,RX

//------------------------------------------------------------------------------------------------------------------------------
void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  pinMode(Ms, INPUT);

  //Setting up BT module
  BTSerial.begin(9600);
}

//------------------------------------------------------------------------------------------------------------------------------
void loop() {

  // MS
  Value = digitalRead(Ms);
  if (State != Value) {
    State = Value;
    BTSerial.print(State);
    Serial.println(State);
  }

  // BT
  if (BTSerial.available()) {
    input = BTSerial.read();
    Serial.write(input);
  }

  // Serial
  if (Serial.available()) {
    sendToBT(Serial.read());
  }
}

//------------------------------------------------------------------------------------------------------------------------------
// FUNCTIONS!!!
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
