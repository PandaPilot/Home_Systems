/**#include <nRF24L01.h>
  #include <RF24.h>
  #include <printf.h>
  #include <RF24_config.h>
  #include <SPI.h>

  int RelayPin = 7;

  RF24 radio(9, 10); // CE, CSN
  const byte address[6] = "00002";
  float state1[2] = {0.0, 23.4}; //on/off (1,0) and temp

  void setup()
  {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
  pinMode(RelayPin, OUTPUT);
  }

  void loop()
  {
  if (radio.available())
  {
    radio.read(&state1, sizeof(state1));
    //Serial.println(state1[0]);
    Serial.println(state1[1]);
    digitalWrite(RelayPin, LOW);
    delay(1000);
  }
  else
  {
    digitalWrite(RelayPin, HIGH);
  }

  }**/
#include "nRF24L01.h" // NRF24L01 library created by TMRh20 https://github.com/TMRh20/RF24
#include "RF24.h"
#include "SPI.h"

typedef struct {
  int RC_no;
  float Temperature;
  int Motion_no; // time count since last detection (no x 8 sec)
} Data;

Data data = {0, 0.0}; // Used to store value received by the NRF24L01
String RC = "RC: ";
String Temp = " Temperature: ";
String out;
RF24 radio(9, 10); // NRF24L01 used SPI pins + Pin 9 and 10 on the UNO

const byte address_1[6] = "00001"; // Needs to be the same for communicating between 2 NRF24L01
const byte address_2[6] = "00002"; // Needs to be the same for communicating between 2 NRF24L01
const byte address_3[6] = "00003"; // Needs to be the same for communicating between 2 NRF24L01

void setup(void) {
  Serial.begin(9600);

  radio.begin(); // Start the NRF24L01

  radio.openReadingPipe(1, address_1); // Get NRF24L01 ready to receive
  radio.openReadingPipe(2, address_2); // Get NRF24L01 ready to receive
  radio.openReadingPipe(3, address_3); // Get NRF24L01 ready to receive

  radio.startListening(); // Listen to see if information received

}

void loop(void) {

  if (radio.available(address_1))
  {
    radio.read(&data, sizeof(data)); // Read information from the NRF24L01
    out = RC + data.RC_no + Temp + data.Temperature;
    Serial.println(out);
    if (data.Motion_no==0){Serial.println("motion");}
    else {Serial.println("away");}
    delay(8500);
  }
  else
  {
    Serial.println("Not Connected");
    data = {0, 0.0};
    delay(8500);
  }
  
}
