void setup() {
  Serial.begin(9600);
  pinMode(S1,INPUT);
}

void loop() {
  int boton;
  boton = digitalRead(S1);
  Serial.println(boton);
  Serial.println("hi");
  delay(1000);
}
