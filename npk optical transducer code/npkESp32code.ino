#include<WiFi.h>
#include<Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "Techsow"
#define WIFI_PASSWORD "12345678"
#define API_KEY "AIzaSyBqQqhv_TUGM79CvQyHQmp-vvekm61l1MY"
#define DATABASE_URL "https://techsow-5d67d.asia-southeast1.firebasedatabase.app/"

#define RX2 6
#define TX2 7

FirebaseData fdbo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned sendDataPrevMillis = 0;
bool signupOK = false;
int n=0;
int p=0;
int k=0;
// Flag to indicate if the sensor reading should be performed
bool readSensor = false;

void sendStartCommand() {
  Serial2.write('R');
}

void setup(){
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
  Serial.print("Connecting to Wifi");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print("."); delay(300);
  }
  Serial.println();
  Serial.println("Connected with IP : ");
  Serial.println(WiFi.localIP());
  Serial.println();

config.api_key = API_KEY;
config.database_url = DATABASE_URL;
if(Firebase.signUp(&config,&auth,"","")){
  Serial.println("SignUp OK");
  signupOK = true;
  sendStartCommand();
}else{
  Serial.printf("%s\n",config.signer.signupError.message.c_str());
}
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config,&auth);
  Firebase.reconnectWiFi(true);
}

void loop()
{
  if (readSensor) {
  if (Serial2.available()) {
    String data = Serial2.readStringUntil('\n');
    parseData(data);
  }}
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    Firebase.RTDB.setInt(&fdbo, "/NPK/nitrogen", n);
    Firebase.RTDB.setInt(&fdbo, "/NPK/phosphorus", p);
    Firebase.RTDB.setInt(&fdbo, "/NPK/potassium", k);
    Serial.println("Data sent to Firebase.");
}
  
}

void parseData(String data) {
  int index = data.indexOf('N');
  if (index != -1) {
    n = data.substring(index + 1, data.indexOf(',')).toInt();
  }

  index = data.indexOf('P');
  if (index != -1) {
    p = data.substring(index + 1, data.indexOf(',', index + 1)).toInt();
  }

  index = data.indexOf('K');
  if (index != -1) {
    k = data.substring(index + 1).toInt();
  }
}
