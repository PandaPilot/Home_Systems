
/**#include <SPI.h>
  #include <nRF24L01.h>
  #include <RF24.h>

  float state[2] = {0.0, 23.4}; //on/off (1,0) and temp
  RF24 radio(7, 8); // CE, CSN
  const byte address[6] = "00002";
  void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();
  }
  void loop() {

  state[1] = state[1] + 0.1;
  if (state[0] == 0.0)
  {
    state[0] = 1.0;
  }
  else
  {
    state[0] = 0.0;
  }

  radio.write(&state, sizeof(state));
  Serial.println(state[0]);
  Serial.print(state[1]);
  delay(1000);
  }**/

#include "nRF24L01.h" //NRF24L01 library created by TMRh20 https://github.com/TMRh20/RF24
#include "RF24.h"
#include "SPI.h"

#define SwitchPin 8 // Arcade switch is connected to Pin 8 on NANO
#define RelayPin 7
int SentMessage[1] = {000}; // Used to store value before being sent through the NRF24L01

RF24 radio(9, 10); // NRF24L01 used SPI pins + Pin 9 and 10 on the NANO

const byte address[6] = "00001";// Needs to be the same for communicating between 2 NRF24L01

typedef struct {
  int RC_no;
  float Temperature;
} Data;

Data data={1,24.3};
float temp=24.5;
void setup(void) {

  pinMode(SwitchPin, INPUT_PULLUP); // Define the arcade switch NANO pin as an Input using Internal Pullup
  pinMode(RelayPin, OUTPUT);
  digitalWrite(SwitchPin, HIGH); // Set Pin to HIGH at beginning
  Serial.begin(9600);
  radio.begin(); // Start the NRF24L01
  radio.openWritingPipe(address); // Get NRF24L01 ready to transmit
}

void loop(void) {

  if (digitalRead(SwitchPin) == LOW) { // If Switch is Activated
    data.Temperature = temp;
    Serial.println("on");
    digitalWrite(RelayPin, HIGH);
    radio.write(&data, sizeof(data)); // Send value through NRF24L01
  }
  else {
    SentMessage[0] = 000;
    radio.write(SentMessage, 1);
    Serial.println("off");
    digitalWrite(RelayPin, LOW);
  }
}
