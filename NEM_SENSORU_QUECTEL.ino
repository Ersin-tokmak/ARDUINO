#include <SoftwareSerial.h>

#define ledPin 6
#define sensorPin A2
#define buzzerPin 7 // Örnek bir pin numarası, kullanacağınız pine göre değiştirin

SoftwareSerial sim800(10, 11); // RX, TX

void setup() {
  Serial.begin(9600);
  sim800.begin(9600);

  delay(2000);
  sendCommand("AT+CGREG?");
  delay(2000);
  sendCommand("AT+CGDCONT=1,\"IP\",\"internet\"");

  delay(2000);
  sendCommand("AT+CGACT?");
  delay(2000);
  sendCommand("AT+CGACT=0,1");
  delay(2000);
  sendCommand("AT+CGACT=0,1");
  delay(2000);
  sendCommand("AT+CGACT=1,1");
  delay(6000);
  sendCommand("AT+QMTOPEN=0,\"shallowtongue436.cloud.shiftr.io\",1883");
  delay(5000);
  sendCommand("AT+QMTOPEN=0,\"shallowtongue436.cloud.shiftr.io\",1883");
  delay(5000);
  sendCommand("AT+QMTCONN=0,\"TETRA\",\"shallowtongue436\",\"55OUfeUGgPAGu775\"");
  delay(5000);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
}

void loop() {
  int sensorValue = readSensor();
  Serial.print("Analog output: ");
  Serial.println(sensorValue);

  if (sensorValue < 30) {
    digitalWrite(buzzerPin, HIGH); // Buzzerı çalıştır
    delay(1000); // Buzzerın çalışma süresi, istediğiniz gibi ayarlayabilirsiniz
    digitalWrite(buzzerPin, LOW); // Buzzerı durdur
  }

  char sensorValueStr[5]; // Assuming sensorValue will be between 0 and 1023
  itoa(sensorValue, sensorValueStr, 10); // Convert sensorValue to a string

  // Calculate the length of the sensorValue string and add 8 for the rest of the message
  int messageLength = strlen(sensorValueStr) + 4;

  char message[50];
  snprintf(message, sizeof(message), "AT+QMTPUB=0,0,0,0,\"/NEMORANI\",%d,\"NEM:%s\"", messageLength, sensorValueStr);
  sendCommand(message);
  delay(2000);
}

int readSensor() {
  int sensorValue = analogRead(sensorPin);  // Read the analog value from sensor
  int outputValue = map(sensorValue, 0, 1023, 255, 0); // map the 10-bit data to 8-bit data
  analogWrite(ledPin, outputValue); // generate PWM signal
  return outputValue;  // Return analog moisture value
}

void sendCommand(String command) {
  sim800.println(command);
  delay(100);
  while (sim800.available()) {
    Serial.write(sim800.read());
  }
}
