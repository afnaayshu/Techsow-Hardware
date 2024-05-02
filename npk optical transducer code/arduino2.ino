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

// Timer variables for sensor reading
unsigned long sensorReadingTimer = 0;
const unsigned long sensorReadingInterval = 5000;  // 5 seconds

// Timer variables for LED reading intervals
unsigned long redLEDTimer = 0;
unsigned long blueLEDTimer = 0;
unsigned long greenLEDTimer = 0;
const unsigned long readingInterval = 300;  // 300 milliseconds

void setup() {
  // Set the LED pins as outputs
  pinMode(redLEDPin, OUTPUT);
  pinMode(blueLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);

  // Start serial communication with the ESP32 at 115200 baud
  Serial.begin(115200);
  Serial.println();
}

void loop() {
  // Check if a request to read the sensor has been received
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 'r') {
      readSensor = true;
      sensorReadingTimer = millis();  // Reset the timer
      Serial.println("Received 'R' command");
    }
  }

  if (readSensor) {
    // Check if the sensor reading interval has elapsed
    if (millis() - sensorReadingTimer >= sensorReadingInterval) {
      // Read the sensor value with the red LED
      float nitrogen = readSensorValueOverTime(redLEDPin, numReadings);

      // Read the sensor value with the blue LED
      float phosphorus = readSensorValueOverTime(blueLEDPin, numReadings);

      // Read the sensor value with the green LED
      float potassium = readSensorValueOverTime(greenLEDPin, numReadings);

      // Send the start character to indicate the beginning of the data packet
      Serial.print('$');

      // Send the NPK data
      Serial.print("N:");
      Serial.print(nitrogen);
      Serial.print(",P:");
      Serial.print(phosphorus);
      Serial.print(",K:");
      Serial.println(potassium);

      // Send the end character to indicate the end of the data packet
      Serial.print('#');

      readSensor = false;  // Reset the flag after reading the sensor
    }
  }
}

float readSensorValueOverTime(int ledPin, int numReadings) {
  long sum = 0;
  turnOnLED(ledPin);
  unsigned long startTime = millis();

  // Take multiple readings over 3 seconds
  for (int i = 0; i < numReadings; i++) {
    unsigned long currentTime = millis();
    if (ledPin == redLEDPin && currentTime - redLEDTimer >= readingInterval) {
      sum += analogRead(sensorPin);
      redLEDTimer = currentTime;
    } else if (ledPin == blueLEDPin && currentTime - blueLEDTimer >= readingInterval) {
      sum += analogRead(sensorPin);
      blueLEDTimer = currentTime;
    } else if (ledPin == greenLEDPin && currentTime - greenLEDTimer >= readingInterval) {
      sum += analogRead(sensorPin);
      greenLEDTimer = currentTime;
    }

    // Exit the loop if the 5-second interval has elapsed
    if (millis() - startTime >= 5000) {
      break;
    }
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
