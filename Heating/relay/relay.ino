
int RelayPin = 12;

void setup() {
  Serial.begin(9600);
  pinMode(RelayPin, OUTPUT);
}

void loop() {


  digitalWrite(RelayPin, HIGH);
  delay(5000);
  digitalWrite(RelayPin, LOW);


  delay(5000);
}
