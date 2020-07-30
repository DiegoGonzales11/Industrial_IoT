#include "Ubidots.h"

char *pot = "pot";

#define DEVICE  "5ccf7fd91701"  
#define LED "led"
#define TOKEN  "BBFF-uhuuvMLx0B8BkrdbxHp9efBkKKb1bp"   
#define WIFISSID "XIMENA"  
#define PASSWORD "23984789"  
Ubidots client(TOKEN);

void setup() {
    Serial.begin(9600);
    client.wifiConnect(WIFISSID, PASSWORD);
    pinMode(D0,OUTPUT);
    digitalWrite(D0,LOW);
}

void loop() 
{
  int a;
  a = analogRead(A0);
  client.add(pot,a);
  client.send();
  delay(100);
  int b;
  b = client.get(DEVICE,LED);
  if(b == 1)
    digitalWrite(D0,HIGH);
  else
    digitalWrite(D0,LOW);
}
