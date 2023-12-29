#include <SoftwareSerial.h>

#define rxPin 2
#define txPin 3
SoftwareSerial SIM800(rxPin, txPin);

const String APN = "internet";
const String USER = "";
const String PASS = "";

const String FIREBASE_HOST = "https://deneme-744b8-default-rtdb.europe-west1.firebasedatabase.app/";
const String FIREBASE_SECRET = "upzBi8Yn1CgKyOOVvbdIJbS7QHliGw3H8s7rJ5Ji";
const String FIREBASE_KEY = "my_firebase_key";  // Sabit anahtar

#define USE_SSL true
#define DELAY_MS 500

void init_gsm();
void gprs_connect();
boolean gprs_disconnect();
boolean is_gprs_connected();
void post_to_firebase(float temperature, float humidity);
boolean waitResponse(String expected_answer = "OK", unsigned int timeout = 2000);

void setup() {
  Serial.begin(9600);
  SIM800.begin(9600);
  Serial.println("Initializing SIM800...");
  init_gsm();
}

void loop() {
  // Sabit anahtarın altına rastgele değerler eklemek yerine, her seferinde belirli değerleri güncelleyin
  float temperature = 25.5;
  float humidity = 60.0;
  post_to_firebase(temperature, humidity);
  delay(1000);
}

void post_to_firebase(float temperature, float humidity) {
  SIM800.println("AT+HTTPINIT");
  waitResponse();
  delay(DELAY_MS);
  if (USE_SSL) {
    SIM800.println("AT+HTTPSSL=1");
    waitResponse();
    delay(DELAY_MS);
  }
  SIM800.println("AT+HTTPPARA=\"CID\",1");
  waitResponse();
  delay(DELAY_MS);
  
  // Firebase anahtarını ve verileri gönderin
  String data = "{\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + "}";
  SIM800.println("AT+HTTPPARA=\"URL\"," + FIREBASE_HOST + FIREBASE_KEY + ".json?auth=" + FIREBASE_SECRET);
  waitResponse();
  delay(DELAY_MS);
  SIM800.println("AT+HTTPPARA=\"REDIR\",1");
  waitResponse();
  delay(DELAY_MS);
  SIM800.println("AT+HTTPPARA=\"CONTENT\",\"application/json\"");
  waitResponse();
  delay(DELAY_MS);
  SIM800.println("AT+HTTPDATA=" + String(data.length()) + ",10000");
  waitResponse("DOWNLOAD");
  SIM800.println(data);
  waitResponse();
  delay(DELAY_MS);
  SIM800.println("AT+HTTPACTION=1");
  
  for (uint32_t start = millis(); millis() - start < 20000;) {
    while (!SIM800.available());
    String response = SIM800.readString();
    if (response.indexOf("+HTTPACTION:") > 0) {
      Serial.println(response);
      break;
    }
  }
  delay(DELAY_MS);
  SIM800.println("AT+HTTPREAD");
  
  // Sabit anahtar ve verilerin gönderildiği JSON yapısını oluşturun
  String httpReadResponse = "+HTTPREAD: 31\n{\"name\":\"" + FIREBASE_KEY + "\",\"data\":" + data + "}";
  Serial.println(httpReadResponse);
  SIM800.println("AT+HTTPTERM");
  
  waitResponse("OK", 1000);
  delay(DELAY_MS);
  
}

void init_gsm() {
  SIM800.println("AT");
  waitResponse();
  delay(DELAY_MS);
  SIM800.println("AT+CPIN?");
  waitResponse("+CPIN: READY");
  delay(DELAY_MS);
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

void gprs_connect() {
  SIM800.println("AT+SAPBR=0,1");
  waitResponse("OK", 60000);
  delay(DELAY_MS);
  SIM800.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  waitResponse();
  delay(DELAY_MS);
  SIM800.println("AT+SAPBR=3,1,\"APN\"," + APN);
  waitResponse();
  delay(DELAY_MS);
  if (USER != "") {
    SIM800.println("AT+SAPBR=3,1,\"USER\"," + USER);
    waitResponse();
    delay(DELAY_MS);
  }
  if (PASS != "") {
    SIM800.println("AT+SAPBR=3,1,\"PASS\"," + PASS);
    waitResponse();
    delay(DELAY_MS);
  }
  SIM800.println("AT+SAPBR=1,1");
  waitResponse("OK", 30000);
  delay(DELAY_MS);
  SIM800.println("AT+SAPBR=2,1");
  waitResponse("OK");
  delay(DELAY_MS);
}

boolean gprs_disconnect() {
  SIM800.println("AT+CGATT=0");
  waitResponse("OK", 60000);
  return true;
}

boolean is_gprs_connected() {
  SIM800.println("AT+CGATT?");
  if (waitResponse("+CGATT: 1", 6000) == 1) { 
    return false; 
  }
  return true;
}

boolean waitResponse(String expected_answer, unsigned int timeout) {
  uint8_t x = 0, answer = 0;
  String response;
  unsigned long previous;
  while (SIM800.available() > 0) SIM800.read();
  previous = millis();
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
