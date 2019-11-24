#include <dht.h>

dht DHT;

#define DHT11_PIN 4



int ThermistorPin = 0;
int RelayPin = 3;
int Vo;
float R1 = 10000;
float logR2, R2, T, Tc, Tf;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

void setup() {
Serial.begin(9600);
pinMode(RelayPin, OUTPUT);
}

void loop() {
  float chk = DHT.read11(DHT11_PIN);
  Serial.print("Temperature = ");
  Serial.println(DHT.temperature);

  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  Tc = T - 273.15;
  //Tf = (Tc * 9.0)/ 5.0 + 32.0; 

  if (Tc>27.5)
  {
      digitalWrite(RelayPin, HIGH);
  }
  else
  {
    digitalWrite(RelayPin, LOW);
  }

  delay(1500);
}
