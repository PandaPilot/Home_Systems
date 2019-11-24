/*
* Ultrasonic Sensor HC-SR04 and Arduino Tutorial
*
* by Dejan Nedelkovski,
* www.HowToMechatronics.com
*
*/

// defines pins numbers
const int trigPin = 9;
const int echoPin = 10;
const int LEDPin = 7;

// defines variables
long duration;
int distance;
int distance_filtered=0;
long Max;

void setup() {
pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input
pinMode(LEDPin, OUTPUT);

Serial.begin(9600); // Starts the serial communication
}

void loop() {
// Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);

// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);

// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);

// Calculating the distance
distance= duration*0.034/2;
distance_filtered=0.0*distance_filtered+1*distance;
if (distance_filtered>=Max)
{
  Max=distance;
}
if (distance_filtered<=Max/2)
{
  digitalWrite(LEDPin, HIGH);
  //delay(10000);
}
else
{
  digitalWrite(LEDPin, LOW);
}

// Prints the distance on the Serial Monitor
Serial.print("Distance: ");
Serial.println(distance_filtered);
delay(1000);
}
