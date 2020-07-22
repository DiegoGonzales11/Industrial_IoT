#include "Ubidots.h"


#define D0   16 //GPIO16 - WAKE UP
#define D1   5  //GPIO5
#define D2   4  //GPIO4
#define D3   0  //GPIO0
#define D4   2  //GPIO2 - TXD1

#define D5   14 //GPIO14 - HSCLK
#define D6   12 //GPIO12 - HMISO
#define D7   13 //GPIO13 - HMOSI - RXD2
#define D8   15 //GPIO15 - HCS   - TXD2
#define RX   3  //GPIO3 - RXD0 
#define TX   1  //GPIO1 - TXD0


#define DEVICE  "5ccf7fd920e1"  
#define VARIABLE  "var_led"   
#define ID_TEMP "5f127ac81d8472644642c4d4" 
#define TOKEN  "BBFF-GJHTr6N0ao2jF2JaDduVNUIgHzRzRw"   
#define WIFISSID "XIMENA"  
#define PASSWORD "23984789"  
Ubidots client(TOKEN);
const byte  Pin_led = D8; 
  

void setup() {
    Serial.begin(115200);
    pinMode(Pin_led, OUTPUT);
    digitalWrite(Pin_led, LOW);
    client.wifiConnect(WIFISSID, PASSWORD);
}

void loop() 
{
 

   float Valor_Led = client.get(DEVICE, VARIABLE);
    if (Valor_Led==1)
    {
    digitalWrite(Pin_led, HIGH);
    }
    else
    {
      digitalWrite(Pin_led, LOW);
    }
    client.send();
    delay(500);
}
