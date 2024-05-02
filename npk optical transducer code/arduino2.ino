// Define the pins for the LEDs
const int redLEDPin = 9;
const int blueLEDPin = 10;
const int greenLEDPin = 11;

// Define the pin for the LDR sensor
const int sensorPin = A0;

// Number of readings to take for each LED
const int numReadings = 10;

// Flag to indicate if the sensor reading should be performed
bool readSensor = false;

void setup() {
  // Set the LED pins as outputs
  pinMode(redLEDPin, OUTPUT);
  pinMode(blueLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);

  // Start serial communication with the ESP32
  Serial.begin(115200);
}

void loop() {
  // Check if a request to read the sensor has been received
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 'r' || command == 'R') {
      readSensor = true;
    }
  }

  if (readSensor) {
    // Read the sensor value with the red LED on for 3 seconds
    float nitrogen = readSensorValueOverTime(redLEDPin, numReadings);

    // Read the sensor value with the blue LED on for 3 seconds
    float phosphorus = readSensorValueOverTime(blueLEDPin, numReadings);

    // Read the sensor value with the green LED on for 3 seconds
    float potassium = readSensorValueOverTime(greenLEDPin, numReadings);

    // Send the NPK data
    // Send the NPK data
    String npkData = "N:" + String(nitrogen) + ",P:" + String(phosphorus) + ",K:" + String(potassium);
    Serial.println(npkData);

    readSensor = false;  // Reset the flag after reading the sensor
  }
}

float readSensorValueOverTime(int ledPin, int numReadings) {
  long sum = 0;
  turnOnLED(ledPin);

  // Take multiple readings over 3 seconds
  for (int i = 0; i < numReadings; i++) {
    sum += analogRead(sensorPin);
    delay(500);  // Wait for 300ms between readings (3 seconds / 10 readings)
  }

  turnOffLED(ledPin);
  float average = sum / (float)numReadings;

  // Map the average sensor value to the corresponding NPK level
  return mapSensorValueToNPK(average);
}

void turnOnLED(int ledPin) {
  digitalWrite(ledPin, HIGH);
}

void turnOffLED(int ledPin) {
  digitalWrite(ledPin, LOW);
}

float mapSensorValueToNPK(float sensorValue) {
  float npkValue = map(sensorValue, 0, 1023, 0.0, 100.0);
  return npkValue;
}
