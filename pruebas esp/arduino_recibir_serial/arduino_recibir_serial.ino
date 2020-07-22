char state;
char read_serial(void);
void setup() {
  Serial.begin(9600);
  pinMode(7,OUTPUT);
}

void loop() {
  
  state = read_serial();
  switch(state){
    case '0':
      digitalWrite(7,LOW);
      break;
    case '1':
      digitalWrite(7,HIGH);
      break;    
  }  
}

char read_serial(void){
  char dato;
  while(Serial.available() == 0){}
  dato = Serial.read();
  if(dato == '\n'){}
  else{
    return dato;
  }
}
