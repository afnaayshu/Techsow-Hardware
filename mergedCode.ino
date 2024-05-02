#include <WiFi.h>
#include <AsyncUDP.h>
#include <Arduino.h>
#include "DataParser.h"
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "Techsow"
#define WIFI_PASSWORD "12345678"
#define API_KEY "AIzaSyBqQqhv_TUGM79CvQyHQmp-vvekm61l1MY"
#define DATABASE_URL "https://techsow-5d67d.asia-southeast1.firebasedatabase.app/"
#define RX2 16
#define TX2 17

const int udpPort = 59430;

int in1 = 27;
int in2 = 26;
int ena = 14;
int in3 = 25;
int in4 = 33;
int enb = 32;

int Speed = 100;
int Right_speed = 50;
int Left_speed = 50;

AsyncUDP udp;
String tempCode = "z";
String code = "z";
FirebaseData fdbo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;
bool signupOK = false;
int n = 0;
int p = 0;
int k = 0;

unsigned long RoverStartTime = 0; 

bool readSensor = false;
unsigned long readSensorStartTime = 0;
const unsigned long readSensorDuration = 15000;  // 15 seconds

DataParser dataParser;

void sendStartCommand() {
  Serial2.write('r');
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(ena, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enb, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("SignUp OK");
    signupOK = true;
    // sendStartCommand();
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Serial2.available() > 0) {
    String data = Serial2.readString();
    parseData(data);
  }
  if (readSensor) {
    if (millis() - readSensorStartTime >= readSensorDuration) {
      Serial.print(n);
      Serial.print(p);
      Serial.println(k);
      readSensor = false;
    }
  }
  if (udp.listen(udpPort)) {
    udp.onPacket([](AsyncUDPPacket packet) {
      String IncomingData = (char*)packet.data();
      Serial.print("Received Data: ");
      Serial.println(IncomingData);
      dataParser.parseData(IncomingData, ',');
      Left_speed = Speed;
      Right_speed = Speed;
      code = dataParser.getField(0);
    });
    if (code != tempCode) {
      if (code == "f") {
        forward(Left_speed, Right_speed);
        Serial.println("fwd");
      } else if (code == "b") {
        backward(Left_speed, Right_speed);
        Serial.println("bck");
      } else if (code == "l") {
        left(Left_speed, Right_speed);
        Serial.println("left");
      } else if (code == "r") {
        right(Left_speed, Right_speed);
        Serial.println("right");
      } else if (code == "s") {
        Stop();
        Serial.println("stop");
      } if (code == "a") {
        Stop();
        readSensor = true;
        readSensorStartTime = millis();
        sendStartCommand();
      }
      tempCode = code;
    } 
  }
}

void parseData(String data) {
  Serial.print("Raw data received from Arduino: ");
  Serial.println(data);

  int n_index = data.indexOf("N:");
  if (n_index != -1) {
    n = data.substring(n_index + 2, data.indexOf(",", n_index)).toInt();
    Serial.print("Parsed nitrogen value: ");
    Serial.println(n);
  }
  int p_index = data.indexOf("P:");
  if (p_index != -1) {
    p = data.substring(p_index + 2, data.indexOf(",", p_index + 2)).toInt();
    Serial.print("Parsed phosphorus value: ");
    Serial.println(p);
  }
  int k_index = data.indexOf("K:");
  if (k_index != -1) {
    k = data.substring(k_index + 2).toInt();
    Serial.print("Parsed potassium value: ");
    Serial.println(k);
  }

  if (Firebase.ready() && signupOK) {
    sendDataPrevMillis = millis();
    Firebase.RTDB.setInt(&fdbo, "/NPK/nitrogen", n);
    Firebase.RTDB.setInt(&fdbo, "/NPK/phosphorus", p);
    Firebase.RTDB.setInt(&fdbo, "/NPK/potassium", k);
    Serial.println("Data sent to Firebase.");
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
