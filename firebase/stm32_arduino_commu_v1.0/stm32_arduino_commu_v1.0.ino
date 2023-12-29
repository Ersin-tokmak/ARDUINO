#include <SoftwareSerial.h>

SoftwareSerial stm32(2, 3); // RX pin: 2, TX pin: 3
char paket[500]; // Paket boyutunu, sensörün gönderdiği veri boyutuna göre ayarlayın
uint8_t pil1, pil2, pil3, pil4, pil5;
int i=0;
void setup() {
  Serial.begin(9600);
  Serial.println("STM32 BAŞLIYOR");
  stm32.begin(9600);
}

void loop() {
  if (stm32.available() > 0) {
    for ( i = 0; i < 5; i++) {
      paket[i] = stm32.read();
    }
    

    pil1 = paket[0];
    pil2 = paket[1];
    pil3 = paket[2];
    pil4 = paket[3];
    pil5 = paket[4];
    

    Serial.print("Pil 1: ");
    Serial.println(pil1);
    Serial.print("Pil 2: ");
    Serial.println(pil2);
    Serial.print("Pil 3: ");
    Serial.println(pil3);
    Serial.print("Pil 4: ");
    Serial.println(pil4);
    Serial.print("Pil 5: ");
    Serial.println(pil5);
  }


  delay(500);
}

/*
 
 void change_data(){
  pil1=
}
*/
