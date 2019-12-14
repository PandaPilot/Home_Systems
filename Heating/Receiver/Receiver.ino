
#include "nRF24L01.h" // NRF24L01 library created by TMRh20 https://github.com/TMRh20/RF24
#include "RF24.h"
#include "SPI.h"

typedef struct {
  int RC_no;
  float Temperature;
  float Target;
  int Motion_no; // time count since last detection (no x 8 sec)
} Data;

Data data = {0, 0.0, 0.0, 0}; // Used to store value received by the NRF24L01
String RC = "RC: ";
String Temp = " Temperature: ";
String Target = " Target: ";
String out;

RF24 radio(9, 10); // NRF24L01 used SPI pins + Pin 9 and 10 on the UNO

const byte address_1[6] = "00001"; // Needs to be the same for communicating between 2 NRF24L01
const byte address_2[6] = "00002"; // Needs to be the same for communicating between 2 NRF24L01
//const byte address_3[6] = "00003"; // Needs to be the same for communicating between 2 NRF24L01
const byte address_out[6] = "00011";

void setup(void) {
  Serial.begin(9600);

  radio.begin(); // Start the NRF24L01
  radio.setPALevel(RF24_PA_HIGH);
  radio.openReadingPipe(1, address_1); // Get NRF24L01 ready to receive
  radio.openReadingPipe(1, address_2); // Get NRF24L01 ready to receive
  //radio.openReadingPipe(3, address_3); // Get NRF24L01 ready to receive
  radio.openWritingPipe(address_out);

  radio.startListening(); // Listen to see if information received

}

void loop(void) {
  //radio.startListening();
  if (radio.available(address_1))
  {
    radio.read(&data, sizeof(data)); // Read information from the NRF24L01
    out = RC + data.RC_no + Temp + data.Temperature;
    Serial.println(out);
    if (data.Motion_no == 0)
    {
      Serial.println("motion");
      delay(250);
    }
    else if (data.Motion_no > round(16 / 8))
    {
      radio.stopListening();
      radio.write(1, sizeof(1));
      delay(250);
      radio.startListening();
    }
    else
    {
      Serial.println("away");
      delay(250);
    }

    delay(8250);
  }
  else
  {
    Serial.println("1 Not Connected");
    data = {0, 0.0, 0.0, 0};
  }

  if (radio.available(address_2))
  {
    radio.read(&data, sizeof(data)); // Read information from the NRF24L01
    out = RC + data.RC_no + Temp + data.Temperature;
    Serial.println(out);
    if (data.Motion_no == 0)
    {
      Serial.println("motion");
      delay(250);
    }
    else if (data.Motion_no > round(16 / 8))
    {
      radio.stopListening();
      radio.write(1, sizeof(1));
      delay(250);
      radio.startListening();
    }
    else
    {
      Serial.println("away");
      delay(250);
    }

    delay(8250);
  }
  else
  {
    Serial.println("2 Not Connected");
    data = {0, 0.0, 0.0, 0};
  }
  
  if (data.RC_no == 0)
  {
    delay(8250);
  }

}
