/**************************
  Program for Bathroom occupied light
  At start up:
    Light on (as precaution) while PIR sensor initialises
  If motion detected:
    Light on for specified wait time (or until door opens, breaking the circuit)
    Flashes after specific time until further motion detected   (in case of false positives)
  (else) if motion not detected:
    Light off and check constantly for motion   (in case of false negatives)
    After 50s check after every sleep cycle (8s)  (shut down to save power)
 ****************************/

#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <math.h>

volatile int f_wdt = 1;
int LED = 13;             // the pin that the LED is atteched to
int PIR = 2;              // the pin that the sensor is atteched to
int wait =  round(15 * 60 / 8);       // wait time (min*60/8) if no motion after initial detection
int counter = 0;
bool Initialised = false;
bool Motion = false;
int i = 0;
/***************************************************
    Name:        ISR(WDT_vect)

    Returns:     Nothing.

    Parameters:  None.

    Description: Watchdog Interrupt Service. This
                 is executed when watchdog timed out.

 ***************************************************/
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

void setup() {
  pinMode(LED, OUTPUT);   // initalize LED as an output
  pinMode(PIR, INPUT);    // initialize sensor as an input

  power_adc_disable(); // ADC converter
  power_spi_disable(); // SPI
  power_usart0_disable();// Serial (USART)
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

void loop() {

  if (Initialised == false) //only run once, reset by openning the door
  {
    digitalWrite(LED, HIGH);      // turn LED ON
    //Serial.print("initialising");
    delay(4000);
    Initialised = true;
    //Serial.print("initialisd");
    while (i < 60)
    {
      digitalWrite(LED, !digitalRead(LED));
      delay(500);
      i++;
      if (digitalRead(PIR) == HIGH){break;}
    }
  }

  if (digitalRead(PIR) == HIGH)
  { // check if the sensor is HIGH
    digitalWrite(LED, HIGH);      // turn LED ON
    //Serial.print("motion");
    Motion = true;
    for (int i = 0; i < wait; ++i)
    {
      sleeping();
    }
  }
  else if (Motion == true)
  {
    digitalWrite(LED, !digitalRead(LED));
    delay(1000);
  }
  else
  {
    digitalWrite(LED, LOW);       // turn LED OFF
    if (counter > 5)
    {
      sleeping(); // check motion periodically, tune PIR for delay longer than 8s
    }
    counter = counter + 1;
  }

}
