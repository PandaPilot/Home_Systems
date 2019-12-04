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

#define NUM_LEDS 8 // Number of leds on stick
#define LED_PIN 8 // Digital In (DI) of RGB Stick connected to pin 8 of the UNO
typedef struct {
  int RC_no;
  float Temperature;
} Data;

Data data = {0, 0.0}; // Used to store value received by the NRF24L01
String RC = "RC: ";
String Temp = " Temperature: ";
String out;
RF24 radio(9, 10); // NRF24L01 used SPI pins + Pin 9 and 10 on the UNO

const byte address[6] = "00001"; // Needs to be the same for communicating between 2 NRF24L01


void setup(void) {
  Serial.begin(9600);

  radio.begin(); // Start the NRF24L01

  radio.openReadingPipe(0, address); // Get NRF24L01 ready to receive

  radio.startListening(); // Listen to see if information received

  pinMode(LED_PIN, OUTPUT); // Set RGB Stick UNO pin to an OUTPUT
}

void loop(void) {

  if (radio.available())
  {
    radio.read(&data, sizeof(data)); // Read information from the NRF24L01
    Serial.println("Connected");
    switch (data.RC_no)
    {
      case 0:
        break;
      case 1:
      out = RC + data.RC_no + Temp + data.Temperature;
      Serial.print(out);
        break;
      case 2:
        break;
    }

    delay(10);
  }
  else
  {
    Serial.println("Not Connected");
    data = {0, 0.0};
  }
}
