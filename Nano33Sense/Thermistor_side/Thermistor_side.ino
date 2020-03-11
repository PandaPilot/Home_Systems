
#include <ArduinoBLE.h>
#include <Arduino_HTS221.h>

BLEService tempService("1101");
BLECharCharacteristic tempChar("2101", BLERead | BLENotify);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(A0, OUTPUT);
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  if (!HTS.begin()) {
    Serial.println("Failed to initialize humidity temperature sensor!");
    while (1);
  }
  pinMode(LED_BUILTIN, OUTPUT);

  BLE.setLocalName("Temperature");
  BLE.setAdvertisedService(tempService);
  tempService.addCharacteristic(tempChar);
  BLE.addService(tempService);

  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  // put your main code here, to run repeatedly:
  BLEDevice central = BLE.central();

  if (central)
  {
    Serial.print("Connected to central: ");
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, HIGH);

    while (central.connected()) {
      Serial.println(central.address());
      float temp = HTS.readTemperature();
      Serial.print("Temperature: ");
      Serial.println(temp);
      tempChar.writeValue(temp);
      delay(200);

    }
  }
  digitalWrite(LED_BUILTIN, LOW);
  Serial.print("Disconnected from central: ");
  Serial.println(central.address());
}
