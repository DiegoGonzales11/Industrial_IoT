void setup() {
  Serial.begin(9600);
  pinMode(A0,INPUT);
}

void loop() {
  int pot;
  pot = analogRead(A0);
  if(pot > 800)
    Serial.println('1');
  else
    Serial.println('0');    
}
