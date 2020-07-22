#include "led.h"
void setup() {
  // put your setup code here, to run once:
  pinMode(5,OUTPUT);
}

void loop() {
  led_on(5);
  delay(1000);
  digitalWrite(5,LOW);
}
