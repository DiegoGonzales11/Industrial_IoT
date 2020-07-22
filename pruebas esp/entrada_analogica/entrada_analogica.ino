void setup() {
  Serial.begin(9600);
  pinMode(A0,INPUT);
}

void loop() {
  int pot;
  pot = analogRead(A0);
  Serial.println(pot);
  delay(1000);
}
