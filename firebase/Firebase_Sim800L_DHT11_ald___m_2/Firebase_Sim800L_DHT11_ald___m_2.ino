#define TINY_GSM_MODEM_SIM800

#include <SoftwareSerial.h>
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

#define rxPin 2
#define txPin 3
SoftwareSerial SIM800(rxPin, txPin);

const char FIREBASE_HOST[] = "deneme2-318ef-default-rtdb.europe-west1.firebasedatabase.app";
const String FIREBASE_AUTH = "gw505Rf2kjJKFu7FbNf7cOc0zAKsUvzJ0Kyasj6k";
const String FIREBASE_PATH = "valueee";
const int SSL_PORT = 443;
#define DELAY_MS 200

char apn[] = "internet";
char user[] = "";
char pass[] = "";

TinyGsm modem(SIM800);
TinyGsmClientSecure gsm_client_secure_modem(modem, 0);
HttpClient http_client = HttpClient(gsm_client_secure_modem, FIREBASE_HOST, SSL_PORT);

unsigned long previousMillis = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Device serial initialized");

  SIM800.begin(9600);
  Serial.println("SIM800L serial initialized");

  Serial.println("Initializing modem...");
  init_gsm();
  
  String modemInfo = modem.getModemInfo();
  Serial.print("Modem: ");
  Serial.println(modemInfo);
  
  http_client.setHttpResponseTimeout(5 * 1000);
}

void loop() {
  String data = "{\"temprature\":\"25\",\"deger5\":\"50\"}"; // Sabit değerler (25°C sıcaklık, 50% nem)
  Serial.println(data);
  
  if (!is_gprs_connected()) {
    gprs_connect();
  }
  
  post_to_firebase(data);
  
  delay(1000);
}

void init_gsm() {
  SIM800.println("AT");
  waitResponse("OK");
        
  SIM800.println("AT+CPIN?");
  waitResponse("+CPIN: READY");
        
  SIM800.println("AT+CFUN=1");
  
  delay(DELAY_MS);
        
  SIM800.println("AT+CSQ"); 
  delay(DELAY_MS);
      
  SIM800.println("AT+CMEE=2");
 
  delay(DELAY_MS);
        
  SIM800.println("AT+CBATCHK=1");
  
  delay(DELAY_MS);
      
  SIM800.println("AT+CREG?");
  waitResponse("+CREG: 0,");
  delay(DELAY_MS);
      
  SIM800.print("AT+CMGF=1\r");
  waitResponse("OK");
  delay(DELAY_MS);
}

void gprs_connect() {
  SIM800.println("AT+SAPBR=0,1");
  delay(DELAY_MS);
    
  SIM800.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  
  delay(DELAY_MS);
    
  SIM800.println("AT+SAPBR=3,1,\"APN\",\"" + String(apn) + "\"");

  delay(DELAY_MS);
    
  if (String(user) != "") {
    SIM800.println("AT+SAPBR=3,1,\"USER\",\"" + String(user) + "\"");
   
    delay(DELAY_MS);
  }
    
  if (String(pass) != "") {
    SIM800.println("AT+SAPBR=3,1,\"PASS\",\"" + String(pass) + "\"");
    
    delay(DELAY_MS);
  }
    
  SIM800.println("AT+SAPBR=1,1");
  waitResponse("OK", 3000);
  delay(DELAY_MS);
    
  SIM800.println("AT+SAPBR=2,1");
  waitResponse("OK");
  delay(DELAY_MS);
}

boolean gprs_disconnect() {
  SIM800.println("AT+CGATT=0");
  waitResponse("OK", 6000);
  return true;
}

boolean is_gprs_connected() {
  SIM800.println("AT+CGATT?");
  if (waitResponse("+CGATT: 1", 6000) == 1) { 
    return false; 
  }
  return true;
}

void post_to_firebase(String data) {
  String firebasePath = FIREBASE_HOST + FIREBASE_PATH + ".json?auth=" + FIREBASE_AUTH;

  SIM800.println("AT+HTTPINIT");
  
  delay(10);

  SIM800.println("AT+HTTPSSL=1");
  
  delay(10);

  SIM800.println("AT+HTTPPARA=\"CID\",1");
 
  delay(10);

  SIM800.println("AT+HTTPPARA=\"URL\",\"" + firebasePath + "\"");
  
  delay(10);

  SIM800.println("AT+HTTPPARA=\"REDIR\",1");

  delay(10);

  SIM800.println("AT+HTTPPARA=\"CONTENT\",\"application/json\"");

  delay(10);

  SIM800.println("AT+HTTPDATA=" + String(data.length()) + ",1000");
  waitResponse("DOWNLOAD");

  SIM800.println(data);
  
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

boolean waitResponse(String expected_answer) {
  return waitResponse(expected_answer, 1000);
}

boolean waitResponse(String expected_answer, unsigned int timeout) {
  uint8_t x = 0, answer = 0;
  String response = "";

  //Clean the input buffer
  while (SIM800.available() > 0)
    SIM800.read();

  unsigned long previous = millis();
  do {
    if (SIM800.available() != 0) {
      char c = SIM800.read();
      response.concat(c);
      x++;
      if (response.indexOf(expected_answer) > 0) {
        answer = 1;
      }
    }
  } while ((answer == 0) && ((millis() - previous) < timeout));

  Serial.println(response);
  return answer;
}
