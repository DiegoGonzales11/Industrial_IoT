void setup() {

}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i=0; i<1023;i++){
    analogWrite(D0,i);
    delay(10);
  }
  for(int i=1023; i>0;i--){
    analogWrite(D0,i);
    delay(10);
  }
}
