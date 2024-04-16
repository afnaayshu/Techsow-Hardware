#include <WiFi.h>

// WiFi credentials
const char* ssid = "Techsow";
const char* password = "12345678";

// Port for the WiFi server
const int serverPort = 80;

// Create a WiFiServer instance
WiFiServer server(serverPort);

// Create a WiFiClient instance to handle incoming connections
WiFiClient client;

int in1 = 35; // The pin number for the first input of the left motor driver
int in2 = 33; // The pin number for the second input of the left motor driver
int ena = 27; // The pin number for the enable pin of the left motor driver
int in3 = 25; // The pin number for the first input of the right motor driver
int in4 = 14; // The pin number for the second input of the right motor driver
int enb = 14; // The pin number for the enable pin of the right motor driver

int Speed = 50;
int Right_speed = 0;
int Left_speed = 0;

void setup() {
  Serial.begin(115200);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(ena, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enb, OUTPUT);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Start the WiFi server
  server.begin();
  Serial.println("Server started");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  client = server.available();
  if (client) {
    Serial.println("New client connected");

    // Read the data sent by the client
    while (client.connected() && client.available()) {
      char c = client.read();

      // Process the received data
      if (c == 'f') {
        forward(Left_speed, Right_speed);
        Serial.println("Moving forward");
      } else if (c == 'b') {
        backward(Left_speed, Right_speed);
        Serial.println("Moving backward");
      } else if (c == 'l') {
        left(Left_speed, Right_speed);
        Serial.println("Turning left");
      } else if (c == 'r') {
        right(Left_speed, Right_speed);
        Serial.println("Turning right");
      } else if (c == 's') {
        Stop();
        Serial.println("Stopping");
      } else if (c == 'd') {
        int speed = client.parseInt(); // Read the speed value sent by the client
        if (speed >= 0 && speed <= 255) {
          Left_speed = speed;
          Right_speed = speed;
          Speed = speed;
          Serial.print("Speed set to: ");
          Serial.println(speed);
        } else {
          Serial.println("Invalid speed value");
        }
      }
    }

    // Close the connection
    client.stop();
    Serial.println("Client disconnected");
  }
}

void forward(int left_speed, int right_speed) {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(ena, left_speed);
  analogWrite(enb, right_speed);
}

void backward(int left_speed, int right_speed) {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(ena, left_speed);
  analogWrite(enb, right_speed);
}

void left(int left_speed, int right_speed) {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(ena, left_speed);
  analogWrite(enb, right_speed);
}

void right(int left_speed, int right_speed) {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(ena, left_speed);
  analogWrite(enb, right_speed);
}

void Stop() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  analogWrite(ena, 0);
  analogWrite(enb, 0);
}
