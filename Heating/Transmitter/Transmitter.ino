
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
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

#define PIR 8 // Arcade switch is connected to Pin 8 on NANO

int SentMessage[1] = {000}; // Used to store value before being sent through the NRF24L01

RF24 radio(9, 10); // NRF24L01 used SPI pins + Pin 9 and 10 on the NANO

const byte address_out[6] = "00001";// Needs to be the same for communicating between 2 NRF24L01
const byte address_in[6]  = "00011";

typedef struct {
  int RC_no;
  float Temperature;
  int Motion_no; // time count since last detection (no x 8 sec)
} Data;

Data data = {1,24.3,0};
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
  radio.openReadingPipe(1, address_in); // Get NRF24L01 ready to receive
  radio.startListening(); // Listen to see if information received
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
    Motion=true;
  }
  else if(Motion) 
  {
    data.Temperature = temp;
    data.Motion_no = data.Motion_no+1;
    radio.write(&data, sizeof(data)); // Send value through NRF24L01
  }
  radio.read
  sleeping();
}
