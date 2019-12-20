/**
  Living Room: Radio 1
  Bedroom    : Radio 2
**/
const int RC_number = 2;

float Target_temp = 21;


#include "nRF24L01.h" //NRF24L01 library created by TMRh20 https://github.com/TMRh20/RF24
#include "RF24.h"
#include "SPI.h"
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#define RedLED 7
#define PIR 8 // Arcade switch is connected to Pin 8 on NANO
#define ThermPin A0

RF24 radio(9, 10); // NRF24L01 used SPI pins + Pin 9 and 10 on the NANO

const uint64_t rAddress[] = {0x7878787878LL, 0xB3B4B5B6F1LL, 0xB3B4B5B6CDLL, 0xB3B4B5B6A3LL, 0xB3B4B5B60FLL, 0xB3B4B5B605LL };

const uint64_t address_out = rAddress[RC_number];// Needs to be the same for communicating between 2 NRF24L01
const uint64_t address_in = rAddress[0];

typedef struct {
  int RC_no;
  float Temperature;
  float Target;
  int Motion_no; // time count since last detection (no x 4.5 sec)
} Data;

Data data = {RC_number, 0, Target_temp};
int Return = 0;

String RC = " RC: ";
String Temp = " Temperature: ";
String Target = " Target: ";
String out;
bool Motion = false;

volatile int f_wdt = 1;

int Vo;
float R1 = 10000;
float logR2, R2, T, Tc;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

ISR(WDT_vect)
{
  if (f_wdt == 0)
  {
    f_wdt = 1;
  }
}
void sleeping()
{
  sleep_enable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_mode();
}


void setup(void) {

  Serial.begin(9600);
  pinMode(RedLED, OUTPUT);
  radio.begin(); // Start the NRF24L01
  radio.setChannel(108);
  radio.openWritingPipe(address_out); // Get NRF24L01 ready to transmit
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.openReadingPipe(1, address_in); // Get NRF24L01 ready to receive
  radio.stopListening(); // Listen to see if information received
  //power_adc_disable(); // ADC converter
  //power_usart0_disable();// Serial (USART)
  power_timer1_disable();// Timer 1
  power_timer2_disable();// Timer 2
  power_twi_disable(); // TWI (I2C)

  /*** Setup the WDT ***/

  /* Clear the reset flag. */
  MCUSR &= ~(1 << WDRF);

  /* In order to change WDE or the prescaler, we need to
     set WDCE (This will allow updates for 4 clock cycles).
  */
  WDTCSR |= (1 << WDCE) | (1 << WDE);

  /* set new watchdog timeout prescaler value */
  WDTCSR = 1 << WDP3; /* 4.0 seconds */

  /* Enable the WD interrupt (note no reset). */
  WDTCSR |= _BV(WDIE);
}

void loop(void) {
  Vo = analogRead(ThermPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  Tc = T - 273.15;
  data.Temperature = Tc;

  if (digitalRead(PIR) == HIGH)
  { // If Switch is Activated

    data.Motion_no = 0;
    Serial.println("motion");
    radio.write(&data, sizeof(data)); // Send value through NRF24L01
    Motion = true;
    delay(250);
    sleeping();
  }
  else if (Motion)
  {

    data.Motion_no = data.Motion_no + 1;
    Serial.println("no motion");
    radio.write(&data, sizeof(data)); // Send value through NRF24L01
    delay(500);
    radio.startListening();
    delay(500);
    radio.read(&Return, sizeof(Return)); // Read information from the NRF24L01
    delay(500);
    radio.read(&Return, sizeof(Return)); // Read information from the NRF24L01
    delay(500);
    radio.read(&Return, sizeof(Return)); // Read information from the NRF24L01
    if (Return == 1)
    {
      Motion = false;
    }
    else
    {
      Return = 0;
    }

    radio.stopListening();
    delay(2000);
  }
  if (Motion && Tc < Target_temp)
  { 
    digitalWrite(RedLED, HIGH);
  }
  else
  {
    digitalWrite(RedLED, LOW);
  }
  
  out = Return + RC + data.RC_no + Temp + data.Temperature + Target + data.Target;
  Serial.println(out);
  delay(250);

}
