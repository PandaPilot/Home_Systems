/**
  Living Room: Radio 1 = 00001
  Bedroom    : Radio 2 = 00002
**/
const int RC_number = 1;

const float Target_temp = 21;


#include "nRF24L01.h" //NRF24L01 library created by TMRh20 https://github.com/TMRh20/RF24
#include "RF24.h"
#include "SPI.h"
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#define PIR 8 // Arcade switch is connected to Pin 8 on NANO

RF24 radio(9, 10); // NRF24L01 used SPI pins + Pin 9 and 10 on the NANO

const byte address_in[6]  = "00011";
const byte address_out[6] = "00001";// Needs to be the same for communicating between 2 NRF24L01


typedef struct {
  int RC_no;
  float Temperature;
  float Target;
  int Motion_no; // time count since last detection (no x 8 sec)
} Data;

Data data = {RC_number, 24.3, Target_temp};
int Return = 0;

String RC = " RC: ";
String Temp = " Temperature: ";
String Target = " Target: ";
String out;
float temp = 24.5;
bool Motion = false;

volatile int f_wdt = 1;

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
  radio.begin(); // Start the NRF24L01
  radio.openWritingPipe(address_out); // Get NRF24L01 ready to transmit

  radio.setPALevel(RF24_PA_HIGH);
  radio.openReadingPipe(1, address_in); // Get NRF24L01 ready to receive
  radio.stopListening(); // Listen to see if information received
  power_adc_disable(); // ADC converter
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
  WDTCSR = 1 << WDP0 | 1 << WDP3; /* 8.0 seconds */

  /* Enable the WD interrupt (note no reset). */
  WDTCSR |= _BV(WDIE);
}

void loop(void) {

  if (digitalRead(PIR) == HIGH)
  { // If Switch is Activated
    data.Temperature = temp;
    data.Motion_no = 0;
    //Serial.println("on");
    radio.write(&data, sizeof(data)); // Send value through NRF24L01
    Motion = true;
    delay(250);
    sleeping();
  }
  else if (Motion)
  {
    data.Temperature = temp;
    data.Motion_no = data.Motion_no + 1;
    Serial.println("no motion");
    radio.write(&data, sizeof(data)); // Send value through NRF24L01
    delay(4000);
    radio.startListening();
    delay(2000);
    if (radio.available(address_in))//
    {
      radio.read(&Return, sizeof(Return)); // Read information from the NRF24L01
      delay(2000);
      
      if (Return == RC_number)
      {
        Motion = false;
      }
      Return = 0;
    }
    radio.stopListening();
  }
  out = Return + RC + data.RC_no + Temp + data.Temperature + Target + data.Target;
  //Serial.println(out);
  delay(50);

}
