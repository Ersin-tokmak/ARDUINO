#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_RX_BUFFER 256

#include <ArduinoHttpClient.h>
#include <TinyGsmClient.h>
#include <SoftwareSerial.h>

const char FIREBASE_HOST[] = "https://deneme2-318ef-default-rtdb.europe-west1.firebasedatabase.app";
const String FIREBASE_AUTH = "gw505Rf2kjJKFu7FbNf7cOc0zAKsUvzJ0Kyasj6k";
const String FIREBASE_PATH = "";
const int SSL_PORT = 443;

char apn[] = "internet";
char user[] = "";
char pass[] = "";

SoftwareSerial gsmSerial(2, 3);
TinyGsm modem(gsmSerial);

TinyGsmClientSecure gsm_client_secure_modem(modem, 0);
HttpClient http_client = HttpClient(gsm_client_secure_modem, FIREBASE_HOST, SSL_PORT);

unsigned long previousMillis = 0;
long interval = 10000;
const int MAX_RETRY_COUNT = 5;

// Function prototype
void PostToFirebase(const char* method, const String& path, const String& data, HttpClient* http);

void setup() {
  Serial.begin(9600);
  Serial.println("Seri haberleşme başlatılıyor");

  gsmSerial.begin(9600);
  Serial.println("GSM seri haberleşme başlatılıyor");

  Serial.println("Modem başlatılıyor...");
  if (!modem.waitForNetwork()) {
    Serial.println("Ağa bağlanma başarısız oldu. Lütfen SIM kartınızı ve sinyal gücünü kontrol edin.");
    while (1);  // Ağ başlatma başarısız olduğunda sonsuz döngü
  }

  String modemInfo = modem.getModemInfo();
  Serial.print("Modem: ");
  Serial.println(modemInfo);

  http_client.setHttpResponseTimeout(9 * 1000);
}

void loop() {
  Serial.print(F("Bağlanılıyor "));
  Serial.print(apn);
  int retryCount = 0;
  while (!modem.gprsConnect(apn, user, pass)) {
    Serial.println(" bağlanılamadı");
    delay(1000);
    retryCount++;
    if (retryCount > MAX_RETRY_COUNT) {
      Serial.println("Maksimum tekrar sayısına ulaşıldı. Çıkılıyor.");
      while (1);  // Maksimum tekrar sayısına ulaşıldığında sonsuz döngü
    }
  }
  Serial.println(" OK");

  http_client.connect(FIREBASE_HOST, SSL_PORT);

  // Firebase'e sabit koordinatları gönder
  String pil1 = "10";  // Örnek bir enlem (latitude)
  String pil2 = "55";  // Örnek bir boylam (longitude)

  String data = "{";
  data += "\"pil1\":" + pil1 + ",";
  data += "\"pil2\":" + pil2 + "";
  data += "}";

  PostToFirebase("PATCH", FIREBASE_PATH, data, &http_client);
}

void PostToFirebase(const char* method, const String& path, const String& data, HttpClient* http) {
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
    Serial.println("HTTP POST bağlantısı kesildi");
  }
}
