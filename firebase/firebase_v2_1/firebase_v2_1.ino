#include <SoftwareSerial.h>

#define rxPin 2
#define txPin 3
SoftwareSerial SIM800(rxPin, txPin);

const String APN = "internet"; // Mobil operatörünüzün APN bilgisi
const String USER = " "; // Eğer varsa APN kullanıcı adınız, genellikle boş bırakılır
const String PASS = " "; // Eğer varsa APN şifreniz, genellikle boş bırakılır

const String FIREBASE_HOST = "https://deneme-744b8-default-rtdb.europe-west1.firebasedatabase.app"; // Firebase proje URL'niz
const String FIREBASE_SECRET = "upzBi8Yn1CgKyOOVvbdIJbS7QHliGw3H8s7rJ5Ji"; // Firebase gizli anahtarınız

#define USE_SSL true
#define DELAY_MS 500

void init_gsm();
void gprs_connect();
boolean gprs_disconnect();
boolean is_gprs_connected();
void post_to_firebase(String data);
boolean waitResponse(String expected_answer = "OK", unsigned int timeout = 2000);

void setup() {
  Serial.begin(9600);
  SIM800.begin(9600);
  Serial.println("SIM800 başlatılıyor...");
  init_gsm();
}

void loop() {
  String data = get_static_data(); // Sabit sıcaklık ve nem değerlerini al
  Serial.println(data);

  if (!is_gprs_connected()) {
    gprs_connect();
  }

  post_to_firebase(data);

  delay(1000);
}

// Sabit sıcaklık ve nem değerlerini döndüren fonksiyon
String get_static_data() {
  String t = "30"; // Sabit sıcaklık değeri
  String h = "50.0"; // Sabit nem değeri

  String Data = "{";
  Data += "\"temprature\":\"" + t + "\",";
  Data += "\"humidity\":\"" + h + "\"";
  Data += "}";

  return Data;
}
void post_to_firebase(String data)
{
  
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //Start HTTP connection
  SIM800.println("AT+HTTPINIT");
  waitResponse();
  delay(DELAY_MS);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //Enabling SSL 1.0
  if(USE_SSL == true){
    SIM800.println("AT+HTTPSSL=1");
    waitResponse();
    delay(DELAY_MS);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //Setting up parameters for HTTP session
  SIM800.println("AT+HTTPPARA=\"CID\",1");
  waitResponse();
  delay(DELAY_MS);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //Set the HTTP URL - Firebase URL and FIREBASE SECRET
  SIM800.println("AT+HTTPPARA=\"URL\","+FIREBASE_HOST+".json?auth="+FIREBASE_SECRET);
  waitResponse();
  delay(DELAY_MS);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //Setting up re direct
  SIM800.println("AT+HTTPPARA=\"REDIR\",1");
  waitResponse();
  delay(DELAY_MS);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //Setting up content type
  SIM800.println("AT+HTTPPARA=\"CONTENT\",\"application/json\"");
  waitResponse();
  delay(DELAY_MS);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //Setting up Data Size
  //+HTTPACTION: 1,601,0 - error occurs if data length is not correct
  SIM800.println("AT+HTTPDATA=" + String(data.length()) + ",10000");
  waitResponse("DOWNLOAD");
  //delay(DELAY_MS);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //Sending Data
  SIM800.println(data);
  waitResponse();
  delay(DELAY_MS);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //Sending HTTP POST request
  SIM800.println("AT+HTTPACTION=1");
  
  for (uint32_t start = millis(); millis() - start < 20000;){
    while(!SIM800.available());
    String response = SIM800.readString();
    if(response.indexOf("+HTTPACTION:") > 0)
    {
      Serial.println(response);
      break;
    }
  }
    
  delay(DELAY_MS);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //+HTTPACTION: 1,603,0 (POST to Firebase failed)
  //+HTTPACTION: 0,200,0 (POST to Firebase successfull)
  //Read the response
  SIM800.println("AT+HTTPREAD");
  waitResponse("OK");
  delay(DELAY_MS);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //Stop HTTP connection
  SIM800.println("AT+HTTPTERM");
  waitResponse("OK",1000);
  delay(DELAY_MS);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
}





//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Initialize GSM Module
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void init_gsm()
{
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //Testing AT Command
  SIM800.println("AT");
  waitResponse();
  delay(DELAY_MS);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //Checks if the SIM is ready
  SIM800.println("AT+CPIN?");
  waitResponse("+CPIN: READY");
  delay(DELAY_MS);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //Turning ON full functionality
  SIM800.println("AT+CFUN=1");
  waitResponse();
  delay(DELAY_MS);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //Turn ON verbose error codes
  SIM800.println("AT+CMEE=2");
  waitResponse();
  delay(DELAY_MS);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //Enable battery checks
  SIM800.println("AT+CBATCHK=1");
  waitResponse();
  delay(DELAY_MS);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //Register Network (+CREG: 0,1 or +CREG: 0,5 for valid network)
  //+CREG: 0,1 or +CREG: 0,5 for valid network connection
  SIM800.println("AT+CREG?");
  waitResponse("+CREG: 0,");
  delay(DELAY_MS);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //setting SMS text mode
  SIM800.print("AT+CMGF=1\r");
  waitResponse("OK");
  delay(DELAY_MS);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
}





//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//Connect to the internet
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void gprs_connect()
{
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //DISABLE GPRS
  SIM800.println("AT+SAPBR=0,1");
  waitResponse("OK",60000);
  delay(DELAY_MS);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //Connecting to GPRS: GPRS - bearer profile 1
  SIM800.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  waitResponse();
  delay(DELAY_MS);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //sets the APN settings for your sim card network provider.
  SIM800.println("AT+SAPBR=3,1,\"APN\","+APN);
  waitResponse();
  delay(DELAY_MS);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //sets the user name settings for your sim card network provider.
  if(USER != ""){
    SIM800.println("AT+SAPBR=3,1,\"USER\","+USER);
    waitResponse();
    delay(DELAY_MS);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //sets the password settings for your sim card network provider.
  if(PASS != ""){
    SIM800.println("AT+SAPBR=3,1,\"PASS\","+PASS);
    waitResponse();
    delay(DELAY_MS);
  }
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //after executing the following command. the LED light of 
  //sim800l blinks very fast (twice a second) 
  //enable the GPRS: enable bearer 1
  SIM800.println("AT+SAPBR=1,1");
  waitResponse("OK", 30000);
  delay(DELAY_MS);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //Get IP Address - Query the GPRS bearer context status
  SIM800.println("AT+SAPBR=2,1");
  waitResponse("OK");
  delay(DELAY_MS);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
}


/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
* Function: gprs_disconnect()
* AT+CGATT = 1 modem is attached to GPRS to a network. 
* AT+CGATT = 0 modem is not attached to GPRS to a network
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
boolean gprs_disconnect()
{
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //Disconnect GPRS
  SIM800.println("AT+CGATT=0");
  waitResponse("OK",60000);
  //delay(DELAY_MS);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
  //DISABLE GPRS
  //SIM800.println("AT+SAPBR=0,1");
  //waitResponse("OK",60000);
  //delay(DELAY_MS);
  //MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

  return true;
}





/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
* Function: gprs_disconnect()
* checks if the gprs connected.
* AT+CGATT = 1 modem is attached to GPRS to a network. 
* AT+CGATT = 0 modem is not attached to GPRS to a network
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
boolean is_gprs_connected()
{
  SIM800.println("AT+CGATT?");
  if(waitResponse("+CGATT: 1",6000) == 1) { return false; }

  return true;
}





//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//Handling AT COMMANDS
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//boolean waitResponse(String expected_answer="OK", unsigned int timeout=2000) //uncomment if syntax error (arduino)
boolean waitResponse(String expected_answer, unsigned int timeout) //uncomment if syntax error (esp8266)
{
  uint8_t x=0, answer=0;
  String response;
  unsigned long previous;
    
  //Clean the input buffer
  while( SIM800.available() > 0) SIM800.read();
  
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  previous = millis();
  do{
    //if data in UART INPUT BUFFER, reads it
    if(SIM800.available() != 0){
        char c = SIM800.read();
        response.concat(c);
        x++;
        //checks if the (response == expected_answer)
        if(response.indexOf(expected_answer) > 0){
            answer = 1;
        }
    }
  }while((answer == 0) && ((millis() - previous) < timeout));
  //NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
  
  Serial.println(response);
  return answer;
}
