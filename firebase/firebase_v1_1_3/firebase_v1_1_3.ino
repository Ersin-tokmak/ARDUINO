#include <SoftwareSerial.h>
#include <string.h>
#include <FirebaseArduino.h>

#define FIREBASE_HOST "https://deneme2-318ef-default-rtdb.europe-west1.firebasedatabase.app/"
#define FIREBASE_AUTH "gw505Rf2kjJKFu7FbNf7cOc0zAKsUvzJ0Kyasj6k"

#define SIM800_TX_PIN 3  // SIM800 TX Pin
#define SIM800_RX_PIN 2  // SIM800 RX Pin

SoftwareSerial sim800(SIM800_TX_PIN, SIM800_RX_PIN);

FirebaseData firebaseData;
FirebaseJson json;

void setup()
{
  Serial.begin(9600);
  sim800.begin(9600);

  Serial.println("Connecting to Firebase");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Serial.println("Connected to Firebase");

  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

  Serial.println("Connected...");
}

void loop()
{
  int Sdata = random(0, 1023);
  Serial.println(Sdata);
  delay(100);
  json.set("/Reading1", Sdata);
  json.set("/Reading2", Sdata - 100);
  json.set("/Reading3", Sdata - 59);

  if (sendDataToFirebase(json)) {
    Serial.println("Data uploaded to Firebase successfully");
  } else {
    Serial.println("Failed to upload data to Firebase");
  }

  delay(60000);  // Upload data every minute
}

bool sendDataToFirebase(FirebaseJson &json)
{
  String data;
  json.toString(data);

  sim800.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
  delay(1000);
  sim800.println("AT+SAPBR=3,1,\"APN\",\"internet\"");
  delay(1000);
  sim800.println("AT+SAPBR=1,1");
  delay(2000);
  sim800.println("AT+HTTPINIT");
  delay(1000);
  sim800.println("AT+HTTPPARA=\"URL\",\"https://deneme2-318ef-default-rtdb.europe-west1.firebasedatabase.app/sensorData.json\"");
  delay(1000);
  sim800.println("AT+HTTPPARA=\"CONTENT\",\"application/json\"");
  delay(1000);
  sim800.println("AT+HTTPDATA=" + String(data.length()) + ",10000");
  delay(1000);
  sim800.println(data);
  delay(1000);
  sim800.println("AT+HTTPACTION=1");
  delay(5000);
  sim800.println("AT+HTTPREAD");
  delay(1000);
  sim800.println("AT+HTTPTERM");
  delay(1000);
  sim800.println("AT+SAPBR=0,1");
  delay(2000);

  if (sim800.find("OK")) {
    return true;
  } else {
    return false;
  }
}
