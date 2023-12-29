
/////////////////////////////   kod çalışıyor7////////////////////////////


#define TINY_GSM_MODEM_SIM800

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

#include <SoftwareSerial.h>
SoftwareSerial sim800(2, 3);

const char FIREBASE_HOST[] = "smart-f8838-default-rtdb.firebaseio.com";
const String FIREBASE_AUTH = "sF6Jl6mrETO9k5fOLXCFo9VT6r6TwvCBNFYCV7Xm";
const String FIREBASE_PATH = "Elektromobil/Battery";
const int SSL_PORT = 443;

char apn[] = "internet";
char user[] = "";
char pass[] = "";

float pil1 = 0,pil2 = 50,pil3 = 10,pil4 = 20,pil5 = 0,pil6 = 50,pil7 = 10,pil8 = 20,pil9 = 0,pil10 = 50,pil11 = 10,pil12 = 20,pil13 = 0,pil14 = 50,pil15 = 10,pil16 = 20,pil17 = 0,pil18 = 50,pil19 = 10;

TinyGsm modem(sim800);
TinyGsmClientSecure gsm_client_secure_modem(modem, 0);
HttpClient http_client = HttpClient(gsm_client_secure_modem, FIREBASE_HOST, SSL_PORT);

unsigned long previousMillis = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println("Seri bağlantı başladı");

  sim800.begin(9600);
  Serial.println("SIM800L bağlantısı açıldı");

  Serial.println("MODEM ACILIYOR...");
  modem.restart();
  String modemInfo = modem.getModemInfo();
  Serial.print("Modem: ");
  Serial.println(modemInfo);

  http_client.setHttpResponseTimeout(15 * 1000);
}

void loop()
{
  Serial.print(F("baglanıyor "));
  Serial.print(apn);
  if (!modem.gprsConnect(apn, user, pass))
  {
    Serial.println(" HATA");
    delay(1000);
    return;
  }
  Serial.println(" OK");

  http_client.connect(FIREBASE_HOST, SSL_PORT);

  while (true)
  {
    if (!http_client.connected())
    {
      Serial.println();
      http_client.stop(); 
      Serial.println("HTTP BAGLI DEGİL");
      break;
    }
    else
    {
      
      PostFixedValuesToFirebase(&http_client);
    }
  }
}

void PostToFirebase(const char *method, const String &path, const String &data, HttpClient *http)
{
  String response;
  int statusCode = 0;
  http->connectionKeepAlive(); 

  String url;
  if (path[0] != '/')
  {
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
  Serial.print("DURUM KODU: ");
  Serial.println(statusCode);
  response = http->responseBody();
  Serial.print("CEVAP: ");
  Serial.println(response);

  if (!http->connected())
  {
    Serial.println();
    http->stop(); // HTTP BAĞLANTISINI KESME FONKSİYONU
    Serial.println("HTTP POST baglantı kesildi");
  }
}

void PostFixedValuesToFirebase(HttpClient *http)
{
  /* burda yaptığım firebaase e gönderdiğimiz değer aynı olmaması için 0.1 arttırıyoruz bunun sebebi firebase e aynı değer gönderildiğinde veri geldi diye algılamıyor sadece farklı olanları 
        alıyor alıcı tarafında bunu bilerek azaltıp alacağız.
  */

  pil1+=0.1,pil2+=0.1,pil3+=0.1,pil4+=0.1,pil5+=0.1,pil6+=0.1,pil7+=0.1,pil8+=0.1,pil9+=0.1,pil10+=0.1,pil11+=0.1,pil12+=0.1,pil13+=0.1,pil14+=0.1,pil15+=0.1,pil16+=0.1,pil17+=0.1,pil18+=0.1,pil19+=0.1;
  String data = "{";
  data += "\"pil1\":" + String(pil1) + ",";
  data += "\"pil2\":" + String(pil2) + ",";
  data += "\"pil3\":" + String(pil3) + ",";
  data += "\"pil4\":" + String(pil4) + ",";
  data += "\"pil5\":" + String(pil5) + ",";
  data += "\"pil6\":" + String(pil6) + ",";
  data += "\"pil7\":" + String(pil7) + ",";
  data += "\"pil8\":" + String(pil8) + ",";
  data += "\"pil9\":" + String(pil9) + ",";
  data += "\"pil10\":" + String(pil10) + ",";
  data += "\"pil11\":" + String(pil11) + ",";
  data += "\"pil12\":" + String(pil12) + ",";
  data += "\"pil13\":" + String(pil13) + ",";
  data += "\"pil14\":" + String(pil14) + ",";
  data += "\"pil15\":" + String(pil15) + ",";
  data += "\"pil16\":" + String(pil16) + ",";
  data += "\"pil17\":" + String(pil17) + ",";
  data += "\"pil18\":" + String(pil18);
  /*data += "\"pil19\":" + String(pil19);
  data += "\"pil20\":" + String(pil20);*/
  data += "}";
/*
 
  
 */
  PostToFirebase("PATCH", FIREBASE_PATH, data, http);

  
  delay(1000);
}
