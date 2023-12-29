#include <ArduinoJson.h>
#include <SoftwareSerial.h>

#define rxPin 2
#define txPin 3
SoftwareSerial SIM800(rxPin, txPin);

const String APN = "internet";
const String USER = "";
const String PASS = "";
int deger = 0;

const String FIREBASE_HOST = "https://deneme2-318ef-default-rtdb.europe-west1.firebasedatabase.app/";
const String FIREBASE_SECRET = "gw505Rf2kjJKFu7FbNf7cOc0zAKsUvzJ0Kyasj6k";
const String FIREBASE_KEY = "deger1"; // Firebase'den alınan benzersiz anahtar
#define USE_SSL true
#define DELAY_MS 200

void init_gsm();
void gprs_connect();
boolean gprs_disconnect();
boolean is_gprs_connected();
void post_to_firebase(String data);
boolean waitResponse(String expected_answer = "OK", unsigned int timeout = 2000);

void setup() {
  Serial.begin(9600);
  SIM800.begin(9600);
  Serial.println("SIM800 Başlatılıyor...");
  init_gsm();
}

void loop() {
  DynamicJsonDocument jsonDoc(64); // JSON nesnesi oluştur
  jsonDoc["sıcaklık"] = 25;     // Gerçek sıcaklık değeriyle değiştirin
  jsonDoc["deger11"] = 50;          // Gerçek nem değeriyle değiştirin

  String jsonString;
  serializeJson(jsonDoc, jsonString); // JSON nesnesini dizeye dönüştür

  if (!is_gprs_connected()) {
    gprs_connect();
  }

  post_to_firebase(jsonString);

  delay(100);
}

void init_gsm()
{
  if(deger==0){
        SIM800.println("AT");
        waitResponse("OK",100);
        
        SIM800.println("AT+CPIN?");
        waitResponse("+CPIN: READY",100);

      
        SIM800.println("AT+CFUN=1");
        waitResponse();
        delay(DELAY_MS);
      
        SIM800.println("AT+CMEE=2");
        waitResponse();
        delay(DELAY_MS);
        
        SIM800.println("AT+CBATCHK=1");
        waitResponse();
        delay(DELAY_MS);
      
        SIM800.println("AT+CREG?");
        waitResponse("+CREG: 0,");
        delay(DELAY_MS);
      
        SIM800.print("AT+CMGF=1\r");
        waitResponse("OK");
        delay(DELAY_MS);
  }
 deger++;
 Serial.println(deger);
}


void gprs_connect()
{
  if(deger==1){
      SIM800.println("AT+SAPBR=0,1");
      waitResponse("OK",6000);
      delay(DELAY_MS);
    
      SIM800.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
      waitResponse();
      delay(DELAY_MS);
    
      SIM800.println("AT+SAPBR=3,1,\"APN\","+APN);
      waitResponse();
      delay(DELAY_MS);
    
      if(USER != ""){
        SIM800.println("AT+SAPBR=3,1,\"USER\","+USER);
        waitResponse();
        delay(DELAY_MS);
      }
    
      if(PASS != ""){
        SIM800.println("AT+SAPBR=3,1,\"PASS\","+PASS);
        waitResponse();
        delay(DELAY_MS);
      }
    
      SIM800.println("AT+SAPBR=1,1");
      waitResponse("OK", 3000);
      delay(DELAY_MS);
    
      SIM800.println("AT+SAPBR=2,1");
      waitResponse("OK");
      delay(DELAY_MS);
  }
deger++;
}

boolean gprs_disconnect()
{

  SIM800.println("AT+CGATT=0");
  waitResponse("OK",6000);

  return true;
}


boolean is_gprs_connected()
{
  SIM800.println("AT+CGATT?");
  if(waitResponse("+CGATT: 1",6000) == 1) { 
    
    return false; }

  return true;
}

void post_to_firebase(String data) {
  String firebasePath = FIREBASE_HOST + FIREBASE_KEY + ".json?auth=" + FIREBASE_SECRET;

  SIM800.println("AT+HTTPINIT");
  waitResponse();
  delay(10);

  if (USE_SSL == true) {
    SIM800.println("AT+HTTPSSL=1");
    waitResponse();
    delay(10);
  }

  SIM800.println("AT+HTTPPARA=\"CID\",1");
  waitResponse();
  delay(10);

  SIM800.println("AT+HTTPPARA=\"URL\"," + firebasePath);
  waitResponse();
  delay(10);

  SIM800.println("AT+HTTPPARA=\"REDIR\",1");
  waitResponse();
  delay(10);

  SIM800.println("AT+HTTPPARA=\"CONTENT\",\"application/json\"");
  waitResponse();
  delay(10);

  SIM800.println("AT+HTTPDATA=" + String(data.length()) + ",1000");
  waitResponse("DOWNLOAD");

  SIM800.println(data);
  waitResponse();
  delay(10);

  SIM800.println("AT+HTTPACTION=1");

  for (uint32_t start = millis(); millis() - start < 20000;) {
    while (!SIM800.available());
    String response = SIM800.readString();
    if (response.indexOf("+HTTPACTION:") > 0) {
      Serial.println(response);
      break;
    }
  }

  delay(10);

  SIM800.println("AT+HTTPREAD");
  waitResponse("OK");
  delay(10);

  SIM800.println("AT+HTTPTERM");
  waitResponse("OK", 1000);
  delay(10);
}


boolean waitResponse(String expected_answer, unsigned int timeout) {
  uint8_t x = 0, answer = 0;
  String response = ""; // Boş String tanımla
  unsigned long previous;

  //Clean the input buffer
  while (SIM800.available() > 0)
    SIM800.read();

  previous = millis();
  do {

    if (SIM800.available() != 0) {
      char c = SIM800.read();
      response.concat(c);
      x++;
      //checks if the (response == expected_answer)
      if (response.indexOf(expected_answer) > 0) {
        answer = 1;
      }
    }
  } while ((answer == 0) && ((millis() - previous) < timeout));


  Serial.println(response);
  return answer;
}
