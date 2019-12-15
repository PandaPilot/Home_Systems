
#include "nRF24L01.h" // NRF24L01 library created by TMRh20 https://github.com/TMRh20/RF24
#include "RF24.h"
#include "SPI.h"

typedef struct {
  int RC_no;
  float Temperature;
  float Target;
  int Motion_no; // time count since last detection (no x 8 sec)
} Data;

int RelayPin = 7;

Data data[6] = {0, 10.0, 0.0, -1}; // Used to store value received by the NRF24L01
Data control = {0, 10.0, 0.0, 32767};

int Send = 1;
String RC = "RC: ";
String Temp = " Temperature: ";
String Target = " Target: ";
String Motion = " Motion Count: ";
String out;
String controlled;

RF24 radio(9, 10); // NRF24L01 used SPI pins + Pin 9 and 10 on the UNO

const uint64_t rAddress[] = {0x7878787878LL, 0xB3B4B5B6F1LL, 0xB3B4B5B6CDLL, 0xB3B4B5B6A3LL, 0xB3B4B5B60FLL, 0xB3B4B5B605LL };

const uint64_t address_1 = rAddress[1]; // Needs to be the same for communicating between 2 NRF24L01
const uint64_t address_2 = rAddress[2]; // Needs to be the same for communicating between 2 NRF24L01
//const byte address_3[6] = "00003"; // Needs to be the same for communicating between 2 NRF24L01
const uint64_t address_out = rAddress[0];

void setup(void) {
  Serial.begin(9600);
  pinMode(RelayPin, OUTPUT);
  radio.begin(); // Start the NRF24L01
  radio.setChannel(108);
  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1, address_1); // Get NRF24L01 ready to receive
  radio.openReadingPipe(2, address_2); // Get NRF24L01 ready to receive
  //radio.openReadingPipe(3, address_3); // Get NRF24L01 ready to receive
  radio.openWritingPipe(address_out);

  radio.startListening(); // Listen to see if information received

}

void loop(void) {
  //radio.startListening();
  if (radio.available())
  {
    radio.read(&data[0], sizeof(data[0])); // Read information from the NRF24L01
    out = RC + data[0].RC_no + Temp + data[0].Temperature + Motion + data[0].Motion_no;

    for (int i = 0; i < 5; i += 1) // on count 1 to 5
    {
      if (i + 1 == data[0].RC_no)
      {
        data[i + 1] = data[0];
        
        /* controlled temp zone selector */
        if (((data[i + 1].Motion_no > -1) && (data[i + 1].RC_no != control.RC_no)) && (control.Motion_no > (data[i + 1].Motion_no + 15))) // if data valid and isnt the current tx and current zone larger than buffer
        {
          control = data[i + 1];
          Serial.print("Data in ");
        }
        else if (data[i + 1].RC_no == control.RC_no)
        {
          control = data[i + 1];
          Serial.print("Data in ");
        }
        //if (data[i + 1].Motion_no > -1) {Serial.println("truth 1");}
        //if (data[i + 1].RC_no != control.RC_no) {Serial.println("truth 2");}
        //if (control.Motion_no > (data[i + 1].Motion_no + 15)) {Serial.println("truth 3");}
      }
      Serial.println(i+1);
    }



    if (control.Temperature < control.Target)
    {
      digitalWrite(RelayPin, HIGH);
    }
    else if (control.Temperature > control.Target + 0.5)
    {
      digitalWrite(RelayPin, LOW);
    }

    Serial.println(out);
    controlled = RC + control.RC_no + Temp + control.Temperature + Target + control.Target + Motion + control.Motion_no;
    Serial.println(controlled);
    
    if (control.Motion_no == 0)
    {
      delay(250);
    }
    else if (control.Motion_no > round(16 / 4))
    {
      Serial.println("TX");
      radio.stopListening();
      radio.write(&Send, sizeof(Send));
      delay(250);
      radio.write(&Send, sizeof(Send));
      delay(250);
      radio.write(&Send, sizeof(Send));
      delay(250);
      radio.startListening();
    }
    else
    {
      Serial.println("away");
      delay(250);
    }

    delay(4000);
  }
  else
  {
    Serial.println("Not Connected");
    control = {0, 10.0, 0.0, 32767};
    delay(4000);
  }


}
