#include <ArduinoBLE.h>
#include <Arduino_HTS221.h> //Include library to read Temperature and Humidity
#include <Arduino_APDS9960.h> //Include library for colour, proximity and gesture recognition


float Temperature, Humidity;
int Proximity;

void setup(){
  Serial.begin(9600); //Serial monitor to display all sensor values

  if (!HTS.begin()) //Initialize Temperature and Humidity sensor
  { 
    Serial.println("Failed to initialize Temperature and Humidity Sensor!"); 
    while (1);
    }
  if (!APDS.begin()) //Initialize Colour, Proximity and Gesture sensor
  { 
    Serial.println("Failed to initialize Colour, Proximity and Gesture Sensor!"); 
    while (1);
  }

  if (!BLE.begin()) 
  {
    Serial.println("starting BLE failed!");
    while (1);
  }
    Serial.println("BLE Central scan");

    // start scanning for peripherals
  BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768a1214");
 }



void loop() {
  // check if a peripheral has been discovered
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    // discovered a peripheral, print out address, local name, and advertised service
    Serial.print("Found ");
    Serial.print(peripheral.address());
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.print(peripheral.advertisedServiceUuid());
    Serial.println();
    // print the RSSI
    Serial.print("RSSI: ");
    Serial.println(peripheral.rssi());
    
    if (peripheral.localName() != "LED") 
    {
      return;
    }

    // stop scanning
    BLE.stopScan();

    controlLed(peripheral);

    // peripheral disconnected, start scanning again
    BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768a1214");



    Serial.println();
  }
  
  //Read Temperature value
  Temperature = HTS.readTemperature();
  Serial.print("Temperature = ");Serial.println(Temperature);
  delay (200);

  //Read Humidity value
  Humidity = HTS.readHumidity();
  Serial.print("Humidity = ");Serial.println(Humidity);
  delay (200);

  //Proximity value
  if (APDS.proximityAvailable()) {
    Proximity = APDS.readProximity();
    Serial.print("Proximity = ");Serial.println(Proximity); 
    }
  delay (200);

  Serial.println("_____________________________________________________"); 
  delay(1000);
}

void controlLed(BLEDevice peripheral) {
  // connect to the peripheral
  Serial.println("Connecting ...");

  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }

  // discover peripheral attributes
  Serial.println("Discovering attributes ...");
  if (peripheral.discoverAttributes()) {
    Serial.println("Attributes discovered");
  } else {
    Serial.println("Attribute discovery failed!");
    peripheral.disconnect();
    return;
  }

  // retrieve the LED characteristic
  BLECharacteristic ledCharacteristic = peripheral.characteristic("19b10001-e8f2-537e-4f6c-d104768a1214");

  if (!ledCharacteristic) {
    Serial.println("Peripheral does not have LED characteristic!");
    peripheral.disconnect();
    return;
  } else if (!ledCharacteristic.canWrite()) {
    Serial.println("Peripheral does not have a writable LED characteristic!");
    peripheral.disconnect();
    return;
  }

  while (peripheral.connected()) {
    // while the peripheral is connected

    // read the button pin
    int buttonState = digitalRead(buttonPin);

    if (oldButtonState != buttonState) {
      // button changed
      oldButtonState = buttonState;

      if (buttonState) {
        Serial.println("button pressed");

        // button is pressed, write 0x01 to turn the LED on
        ledCharacteristic.writeValue((byte)0x01);
      } else {
        Serial.println("button released");

        // button is released, write 0x00 to turn the LED off
        ledCharacteristic.writeValue((byte)0x00);
      }
    }
  }

  Serial.println("Peripheral disconnected");
}
