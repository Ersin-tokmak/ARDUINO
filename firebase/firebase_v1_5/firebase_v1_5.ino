

#include <Adafruit_FONA.h>
#include <SoftwareSerial.h>
#include <FirebaseArduino.h>

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

char fonaReplyBuffer[255];

int a = 2; // Sabit bir değer

#define FIREBASE_HOST "your-firebase-host.firebaseio.com"
#define FIREBASE_AUTH "your-firebase-auth-token"
#define FIREBASE_PATH "/value"

void setup() {
  Serial.begin(9600);
  fonaSS.begin(4800);

  if (!fona.begin(fonaSS)) {
    Serial.println(F("Couldn't find FONA"));
    while (1);
  }

  fonaSS.println("AT+CGATT=1");
  delay(100);
  if (!fona.sendCheckReply(F("AT+SAPBR=3,1,\"Contype\",\"GPRS\""), "OK")) {
    Serial.println(F("Failed to set APN"));
    while (1);
  }

  if (!fona.sendCheckReply(F("AT+SAPBR=3,1,\"APN\",\"your_APN_here\""), "OK")) {
    Serial.println(F("Failed to set APN"));
    while (1);
  }

  if (!fona.sendCheckReply(F("AT+SAPBR=1,1"), "OK")) {
    Serial.println(F("Failed to open GPRS connection"));
    while (1);
  }

  if (!fona.sendCheckReply(F("AT+SAPBR=2,1"), "OK")) {
    Serial.println(F("Failed to get an IP address"));
    while (1);
  }

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  // Sabit değeri Firebase'e gönder
  String path = FIREBASE_PATH;
  Firebase.setInt(path, a);

  delay(10000); // 10 saniye bekle
}
