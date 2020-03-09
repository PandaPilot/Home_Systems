// RelayPin A0;
#include <ArduinoBLE.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(A0, OUTPUT);
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  BLE.advertise();
digitalWrite(A0, HIGH);
delay(1000);
//digitalWrite(A0, LOW);
delay(1000);
}
