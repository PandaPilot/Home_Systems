#include <ArduinoBLE.h>

// variables for button
const int buttonPin = 2;
int oldButtonState = LOW;

float temp = 100;

void setup() {
  Serial.begin(9600);

  // configure the button pin as input
  pinMode(buttonPin, INPUT);

  // initialize the BLE hardware
  BLE.begin();

  Serial.println("BLE Central - LED control");

  // start scanning for peripherals
  BLE.scanForUuid("1101");

}
String address = BLE.address();
void loop() {
  // check if a peripheral has been discovered

  Serial.print("Local address is: ");
  Serial.println(address);
  BLE.scanForUuid("1101");

  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    // discovered a peripheral, print out address, local name, and advertised service
    Serial.print("Found ");
    Serial.print(peripheral.address());
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.print(peripheral.advertisedServiceUuid());
    Serial.print("' ");
    Serial.println();
    if (peripheral.connect()) {
      Serial.println("Connected");
    } else {
      Serial.println("Failed to connect!");
      return;
    }
    BLECharacteristic TempCharacteristic = peripheral.characteristic("2101");

    Serial.println("Subscribing to simple key characteristic ...");
    if (!TempCharacteristic) {
      Serial.println("no simple key characteristic found!");
      return;
    } else if (!TempCharacteristic.canSubscribe()) {
      Serial.println("simple key characteristic is not subscribable!");
      return;
    } else if (!TempCharacteristic.subscribe()) {
      Serial.println("subscription failed!");
      return;
    }
    //TempCharacteristic.readValue(temp);
    Serial.print("Temp: ");
    Serial.println(temp);
    // stop scanning
    BLE.stopScan();
    // peripheral disconnected, start scanning again
    delay(100);
  }

}
