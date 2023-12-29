#define TINY_GSM_MODEM_SIM800

#define TINY_GSM_RX_BUFFER 256


#include <TinyGsmClient.h>
#include <SoftwareSerial.h>
#include <ArduinoHttpClient.h>




const char FIREBASE_HOST[]  = "vehicle-tracking-4a76d-default-rtdb.firebaseio.com";   ///////7 degiştirilecek
const String FIREBASE_AUTH  = "6NGxNkNWf2inWa1LrdktCtq4************";
const String FIREBASE_PATH  = "/key";
const int SSL_PORT          = 443;

char apn[]  = "internet";
char user[] = "";
char pass[] = "";

#define rxGSM 2 //D3
#define txGSM 3 //D4
SoftwareSerial sim800(rxGSM, txGSM);
TinyGsm modem(sim800);

TinyGsmClientSecure client1(modem);
HttpClient http_client = HttpClient(client1, FIREBASE_HOST, SSL_PORT);

unsigned long previousMillis = 0;
long interval = 10000;

void setup() {
  Serial.begin(9600);
  Serial.println("NodeMCU serial initialize");

  sim800.begin(9600);
  Serial.println("SIM800L serial initialize");
  
  Serial.println("Initializing modem...");
  modem.restart();
  String modemInfo = modem.getModemInfo();
  Serial.print("Modem: ");
  Serial.println(modemInfo);
  
  
}

void loop() {
  sim800.listen();
  sim800.println('AT+SAPBR=3,1,"Contype","GPRS"﻿');
  sim800.println('AT+CSTT="internet","",""﻿');
  sim800.println("AT+SAPBR=1,1﻿");
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN

  http_client.connect(FIREBASE_HOST, SSL_PORT);
}

void PostToFirebase(const char* method, const String & path , const String & data, HttpClient* http) {
  String response;
  int statusCode = 0;
  http->connectionKeepAlive(); // Currently, this is needed for HTTPS

  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
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
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN

  String contentType = "application/json";
  http->put(url, contentType, data);

  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  // read the status code and body of the response
  //statusCode-200 (OK) | statusCode -3 (TimeOut)
  statusCode = http->responseStatusCode();
  Serial.print("Status code: ");
  Serial.println(statusCode);
  response = http->responseBody();
  Serial.print("Response: ");
  Serial.println(response);
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN

  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  if (!http->connected()) {
    Serial.println();
    http->stop();// Shutdown
    Serial.println("HTTP POST disconnected");
  }
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
}
