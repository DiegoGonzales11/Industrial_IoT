int pulsIn(void);
void setup() {
  Serial.begin(9600);
  pinMode(10,OUTPUT);
  pinMode(9,INPUT);

}

void loop() {
  digitalWrite(10,HIGH);
  delayMicroseconds(10);
  digitalWrite(10,LOW);

  int time,distancia;
  time = pulsIn();
  distancia = time/58.2;

  Serial.println(distancia);
  delay(500);
}
int pulsIn(void){
  int time=0;
  while(digitalRead(9)==0){
  }

  while(digitalRead(9)==1){
    delayMicroseconds(1);
    time++;
  }

  return time;
}
