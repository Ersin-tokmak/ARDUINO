
#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_RX_BUFFER 256

#include <ArduinoHttpClient.h>
#include <TinyGsmClient.h>
#include <SoftwareSerial.h>




const char FIREBASE_HOST[] = "deneme2-318ef-default-rtdb.europe-west1.firebasedatabase.app";
const String FIREBASE_AUTH = "gw505Rf2kjJKFu7FbNf7cOc0zAKsUvzJ0Kyasj6k";
const String FIREBASE_PATH = "";
const int SSL_PORT = 443;

char apn[] = "internet";
char user[] = "";
char pass[] = "";

SoftwareSerial gsmSerial(2,3); 
TinyGsm modem(gsmSerial);

TinyGsmClientSecure gsm_client_secure_modem(modem, 0);
HttpClient http_client = HttpClient(gsm_client_secure_modem, FIREBASE_HOST, SSL_PORT);

unsigned long previousMillis = 0;
long interval = 10000;

void setup() {
  Serial.begin(9600);
  Serial.println("serial initialize");

  gsmSerial.begin(9600); 
  Serial.println("GSM serial initialize");

  Serial.println("Initializing modem...");
  modem.restart();
  String modemInfo = modem.getModemInfo();
  Serial.print("Modem: ");
  Serial.println(modemInfo);

  http_client.setHttpResponseTimeout(9*1000);
}

void loop() {
  
  Serial.print(F("bağlanıuyorr "));
  Serial.print(apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    Serial.println(" bağlanamadı");
    delay(1000);
    return;
  }
  Serial.println(" OK");

  http_client.connect(FIREBASE_HOST, SSL_PORT);

  // Send the fixed coordinates to Firebase
  String pil1 = "10"; // Örnek bir enlem (latitude)
  String pil2 = "55"; // Örnek bir boylam (longitude)

  String gpsData = "{";
  gpsData += "\"pil1\":" + pil1 + ",";
  gpsData += "\"pil2\":" + pil2 + "";
  gpsData += "}";

  PostToFirebase("PATCH", FIREBASE_PATH, gpsData, &http_client);
}

void PostToFirebase(const char* method, const String & path , const String & data, HttpClient* http) {
  String response;
  int statusCode = 0;
  http->connectionKeepAlive();

  String url;
  if (path[0] != '/') {
    url = "/";
  }
  url += path + ".json";
  url += "?auth=" + FIREBASE_AUTH;
  Serial.print("POST:");
  Serial.println(url);
  Serial.print("Data:");
  Serial.println(data);

  String contentType = "application/json";
  http->put(url, contentType, data);

  statusCode = http->responseStatusCode();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  response = http->responseBody();
  Serial.print("Response: ");
  Serial.println(response);

  if (!http->connected()) {
    Serial.println();
    http->stop();
    Serial.println("HTTP POST disconnected");
  }
}
