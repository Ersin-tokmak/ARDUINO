#include <SoftwareSerial.h>

// SIM800C TX pin: Arduino digital pin 2
// SIM800C RX pin: Arduino digital pin 3
SoftwareSerial sim800c( 2 , 3 );

void setup() {
  // Seri iletişim hızı ayarını yapın
  Serial.begin(9600);
  sim800c.begin(9600);

  delay(1000);

  // SIM800C'yi başlatmak için AT komutunu gönderin
  sendATCommand("AT\r\n", 1000);

  // SIM800C'ye pin kodunu gönderin
  sendATCommand("AT+CPIN=3447\r\n", 1000);

  // GPRS bağlantısını etkinleştirin
  sendATCommand("AT+CGATT=1\r\n", 1000);
  if("AT+CGATT"==error)
  {
    try sendATCommand("AT+CGATT=1\r\n", 1000);
  }

  // APN ayarlarını yapın (Operatöre bağlı olarak değişebilir)
  sendATCommand("AT+CSTT=\"internet\",\"\",\"\"\r\n", 1000);

  // GPRS bağlantısını açın
  sendATCommand("AT+CIICR\r\n", 1000);
  

  // IP adresini alın
  sendATCommand("AT+CIFSR\r\n", 1000);

  // ThingSpeak sunucusuna bağlanın
  sendATCommand("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n", 3000);

  // Veri göndermek için bağlantıyı hazırlayın
  sendATCommand("AT+CIPSEND\r\n", 1000);
}

void loop() {
  // put your main code here, to run repeatedly:

}
void sendATCommand(String command, int timeout) {
  sim800c.print(command);

  long int time = millis();
  while((time + timeout) > millis()) {
    while(sim800c.available()) {
      // SIM800C'den gelen yanıtı seri monitöre yazdırın
      char c = sim800c.read();
      Serial.print(c);
    }
  }
}
