


//// kod çalışıyor///////




#include <SoftwareSerial.h>

SoftwareSerial gprsSerial(2, 3);

void setup()
{
  gprsSerial.begin(9600); // the GPRS baud rate
  Serial.begin(9600);
  delay(1000);
}

void loop()
{
  float t = 13; // Sabit sıcaklık değeri
  float h = 1; // Sabit nem değeri
  delay(100);

  /*Serial.print("Temperature = ");
  Serial.print(t);
  Serial.println(" °C");
  Serial.print("Humidity = ");
  Serial.print(h);
  Serial.println(" %");
*/
  if (gprsSerial.available())
    Serial.write(gprsSerial.read());

  gprsSerial.println("AT");
  delay(500);

  gprsSerial.println("AT+CPIN?");
  delay(500);

  gprsSerial.println("AT+CREG?");
  delay(500);

  gprsSerial.println("AT+CGATT?");
  delay(500);

  gprsSerial.println("AT+CIPSHUT");
  delay(500);

  gprsSerial.println("AT+CIPSTATUS");
  delay(500);

  gprsSerial.println("AT+CIPMUX=0");
  delay(500);

  ShowSerialData();

  gprsSerial.println("AT+CSTT=\"internet\""); //start task and setting the APN,
  delay(500);

  ShowSerialData();

  gprsSerial.println("AT+CIICR"); //bring up wireless connection
  delay(1000);

  ShowSerialData();

  gprsSerial.println("AT+CIFSR"); //get local IP adress
  delay(500);

  ShowSerialData();

  gprsSerial.println("AT+CIPSPRT=0");
  delay(500);

  ShowSerialData();

  gprsSerial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\""); //start up the connection
  delay(6000);

  ShowSerialData();

  gprsSerial.println("AT+CIPSEND"); //begin send data to remote server
  delay(1000);
  ShowSerialData();

  String str = "GET https://api.thingspeak.com/update?api_key=QCDDM3FGTN6DUMLB&field1=" + String(t) + "&field2=" + String(h)+"&field3="+6;
  Serial.println(str);
  gprsSerial.println(str); //begin send data to remote server

  delay(500);
  ShowSerialData();

  gprsSerial.println((char)26); //sending
  delay(500);                 //waitting for reply, important! the time is base on the condition of internet
  gprsSerial.println();

  ShowSerialData();

  gprsSerial.println("AT+CIPSHUT"); //close the connection
  delay(100);
  ShowSerialData();
}

void ShowSerialData()
{
  while (gprsSerial.available() != 0)
    Serial.write(gprsSerial.read());
  delay(500);
}
