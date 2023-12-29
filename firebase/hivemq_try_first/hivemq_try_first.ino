#include <SoftwareSerial.h>


SoftwareSerial sim800(2,3);  // SIM800C modülünü bağlantı pimlerine göre tanımlayın

const char* apn = "internet";  // İnternet sağlayıcınızın APN'sini girin
const char* mqttBroker = "d0fcbe3c356746459d8ab6956abe6736.s1.eu.hivemq.cloud";  // HiveMQ broker adresini girin
const int mqttPort = 8883;  // HiveMQ broker port numarasını girin
const char* mqttClientId = "aaa";  // MQTT istemci kimliğini girin
const char* mqttUsername = "ersintkm98";  // MQTT sunucusu kullanıcı adınızı girin
const char* mqttPassword = "Ertok.248";  // MQTT sunucusu şifrenizi girin
const char* mqttTopic = "aaa";  // Göndermek istediğiniz MQTT konusunu girin


String sendATCommand(String command, int timeout) {
  String response = "";
  sim800.println(command);
  long int time = millis();
  while ((time + timeout) > millis()) {
    while (sim800.available()) {
      char c = sim800.read();
      response += c;
    }
  }
  return response;
}

void setup() {
  sim800.begin(9600);
  Serial.begin(9600);

  delay(1000);

  sim800.println("AT");
  delay(2000);
  sim800.println("AT+CNMI=2,2,0,0,0");  // Gelen SMS'leri devre dışı bırakır
  delay(2000);

  // GPRS bağlantısını etkinleştirin
  String response = sendATCommand("AT+CGATT?\r\n", 1000);
  if (response.indexOf("+CGATT: 0") != -1) {
    // GPRS bağlantısı mevcut değilse, yeniden etkinleştirin
    sendATCommand("AT+CGATT=1\r\n", 1000);
    delay(2000);
  }

  // SIM800C modülünü APN'ye bağlanmaya zorlar
  sendATCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n", 2000);
  sendATCommand("AT+SAPBR=3,1,\"APN\",\"" + String(apn) + "\"\r\n", 2000);
  sendATCommand("AT+SAPBR=1,1\r\n", 3000);

  // MQTT bağlantısı için gereken AT komutlarını gönderme
  sendATCommand("AT+CMQTTSTART\r\n", 2000);
  sendATCommand("AT+CMQTTACCQ=0,\"" + String(mqttClientId) + "\"\r\n", 2000);
  sendATCommand("AT+CMQTTCONNECT=0,\"" + String(mqttBroker) + "\"," + String(mqttPort) + "\r\n", 2000);
  sendATCommand("AT+CMQTTUSERNAMEFLAG=0,1\r\n", 2000);
  sendATCommand("AT+CMQTTPASSWORDFLAG=0,1\r\n", 2000);
  sendATCommand("AT+CMQTTUSERNAME=0,\"" + String(mqttUsername) + "\"\r\n", 2000);
  sendATCommand("AT+CMQTTPASSWORD=0,\"" + String(mqttPassword) + "\"\r\n", 5000);

  // HiveMQ broker'a bağlanma durumunu kontrol etme
  response = sendATCommand("AT+CMQTTSTATUS=0\r\n", 1000);
  while (response.indexOf("+CMQTTSTATUS: 0,2") == -1) {
    Serial.println("MQTT broker'a bağlanılıyor...");
    delay(5000);
    response = sendATCommand("AT+CMQTTSTATUS=0\r\n", 1000);
  }
  Serial.println("MQTT broker'a bağlanıldı!");
}

void loop() {
  // MQTT mesajı gönderme
  String payload = "Merhaba, bu bir test mesajıdır!";
  sendATCommand("AT+CMQTTPUB=0,0,0," + String(strlen(mqttTopic)) + "\r\n", 1000);
  sendATCommand(String(mqttTopic) + "\r\n", 1000);
  sendATCommand(payload + "\r\n", 1000);
  delay(1000);
}
