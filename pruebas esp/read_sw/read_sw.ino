#define SW  D8
#define LED D0

int read_sw(void);

void setup{
    Serial.begin(9600);
    pinMode(SW,INPUT);
    //pinMode(LED,OUTPUT);
}
void loop{
    int a;
    a = read_sw();
    Serial.println(a);
}

int read_sw(void){
    int time=0;
    while(digitalRead(SW) == 0){}

    delayMricosecond(3);
    if(digitalRead(SW) == 1){
        while(digitalRead == 1){
            time ++;
            delayMicrosecond(1);
        }
        return time;
    }
    else
       return 0;
}
