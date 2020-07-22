#include <LiquidCrystal_I2C.h>
#include "Ubidots.h"

// set the LCD number of columns and rows
#define lcdColumns 16
#define lcdRows 2
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

//servo motor
#define SERVO D0
//HMI
#define SCL D1
#define SDA D2
//control factory
#define RELE D3
//pilot
#define RED_PILOT D4
#define GREEN_PILOT D5
//motor
#define MOTOR D6
//ultrasonic
#define ULT_ECHO  D7
#define ULT_TRIG  D8

//wifi
#define DEVICE  "5ccf7fd920e1"  
#define SWITCH1  "sw_on"
#define SWITCH2  "sw_off"
#define TOKEN  "BBFF-GJHTr6N0ao2jF2JaDduVNUIgHzRzRw"   
#define WIFISSID "CLARO-B612-1B66"  
#define PASSWORD "1D282tF6TT"

Ubidots client(TOKEN);
//pin macros
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

//control macros
#define ON  1
#define OFF 0 

//code for comunication
#define STOP  0
#define START 1
#define NOTHING 2

//servo
#define BACK  2
#define GO  1


//box macros
#define BIG 2
#define SMALL 1
#define NO_BOX  0

char *msg1 = "Welcome to";
char *msg2 = "the factory";
char *msg3 = "Please wait";
char *msg4 = "loading ...";

char *msg5 = "Total box: ";
char *msg6 = "Big:";
char *msg7 = "Sml:";

char *msg8 = "Factory stopped";
char *msg9 = "plase wait";

char *msg10 = "Factory starting";

char *ubidot_big = "bigBox";
char *ubidot_small = "smallBox";
char *ubidot_total = "totalBox";

//configuration
void pin_configuration(void);

void servo_control(int state);
int ultrasonic_control(void);
int check_ultrasonic(void);
int check_box(void);
void LCD_init(void);
void LCD_stop(void);
void LCD_go(void);
void LCD_show(int total,int big, int small);
void pilots_control(int state);
void rele_control(int state);
void motor_control(int state);
int pulsIn(void);

//wifi
void send(int value, char const *ptr);
int receive(void);

//variables globales
int factory_state = ON;
int total_box = 0, big_box = 0, small_box = 0;
int flag_servo = 0; //1=big_box y 0 = small_box
int flag_on = 0;
int flag_off = 0;

void setup(){
  pin_configuration();
  LCD_init();
}

void loop(){
  if(factory_state = ON){
    if(flag_on == 1){
      LCD_go();
    }
    flag_on = 0;

    if(total_box == 0){
      LCD_show(0,0,0);
    }
    rele_control(ON);
    pilots_control(ON);
    motor_control(ON);

    int aux_box;

    aux_box = check_box();
    switch (aux_box)
    {
    case BIG:
      big_box++;
      break;
    case SMALL:
      small_box++;
      break;
    }
    
    total_box = big_box + small_box;

    LCD_show(total_box,big_box,small_box);

    delay(5000);    //time from sensor to servo motor

    if (flag_servo == 1){
      motor_control(OFF);
      servo_control(GO);
      delay(500);
      servo_control(BACK);
      delay(500);
      servo_control(OFF);
      motor_control(ON);
      flag_servo = 0;
    }

    send(total_box,ubidot_total);
    send(big_box,ubidot_big);
    send(small_box,ubidot_small);
    
    if(receive() == 0){
      factory_state = OFF;
      flag_off = ON;
    }
  }
  else{
    if(flag_off == 1){
      LCD_stop();
    }
    flag_off = 0;
    rele_control(OFF);
    pilots_control(OFF);
    motor_control(OFF);

    if(receive() == 1){
      factory_state = ON;
      flag_on = ON;
    }
  }
}

void pin_configuration(void){
  //Serial config
  Serial.begin(96000);
  //Wifi config
  client.wifiConnect(WIFISSID, PASSWORD);
  pinMode(ULT_TRIG,OUTPUT);
  pinMode(ULT_ECHO,INPUT);
  pinMode(RELE,OUTPUT);
  pinMode(RED_PILOT,OUTPUT);
  pinMode(GREEN_PILOT,OUTPUT);
  pinMode(MOTOR,OUTPUT);
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
}

void servo_control(int state){
  switch (state){
  case GO:          //90 grades
    analogWrite(SERVO,76);
    break;
  case BACK:
    analogWrite(SERVO,51);
    break;
  case OFF:
    analogWrite(SERVO,0);
    break;
  }
}

int pulsIn(void){
  int time=0;
  while(digitalRead(ULT_ECHO)==0){
  }

  while(digitalRead(ULT_ECHO)==1){
    delayMicroseconds(1);
    time++;
  }

  return time;
}

int ultrasonic_control(void){
  digitalWrite(ULT_TRIG,HIGH);
  delayMicroseconds(10);
  digitalWrite(ULT_TRIG,LOW);

  int time,distancia;
  time = pulsIn();
  distancia = time*0,01715;
  return distancia;
}

int check_box(void){
  int distance;
  distance = check_ultrasonic();

  if(distance < 2)
    return BIG;
  else if((distance >2) && (distance < 6))
    return SMALL;
  else
    return NO_BOX;
}

int check_ultrasonic(void){
  int sample[5];
  int avrg;

  for(int i=0; i<5; i++){
    sample[i] = ultrasonic_control();
  }

  for(int i=0; i<5; i++){
    avrg = avrg + sample[i];
  }

  return avrg/5;
}

void pilots_control(int state){
  switch (state)
  {
  case ON:
    digitalWrite(GREEN_PILOT,HIGH);
    digitalWrite(RED_PILOT,LOW);
    break;
  
  case OFF:
    digitalWrite(GREEN_PILOT,LOW);
    digitalWrite(RED_PILOT,HIGH);
    break;
  }
}

void rele_control(int state){
  if (state == ON)
    digitalWrite(RELE,LOW);
  else
    digitalWrite(RELE,HIGH);
}

void motor_control(int state){
  if(state == ON)
    digitalWrite(MOTOR,HIGH);
  else
    digitalWrite(MOTOR,LOW);
}

void LCD_init(void){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(msg1);
  lcd.setCursor(0,1);
  lcd.print(msg2);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(msg3);
  lcd.setCursor(0,1);
  lcd.print(msg4);
  delay(1000);
}
void LCD_stop(void){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(msg8);
  lcd.setCursor(0,1);
  lcd.print(msg9);
}
void LCD_go(void){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(msg10);
  lcd.setCursor(0,1);
  lcd.print(msg9);
}
void LCD_show(int total,int big, int small){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(msg5);
  lcd.setCursor(11,0);
  lcd.print(total);

  lcd.setCursor(0,1);
  lcd.print(msg6);
  lcd.setCursor(5,1);
  lcd.print(big);

  lcd.setCursor(7,1);
  lcd.print(msg7);
  lcd.setCursor(11,1);
  lcd.print(small);
}

void send(int value, char const *ptr){
  client.add(ptr,value);
  client.send();
  delay(100);
}

int receive(void){
  int sw_on, sw_off, check;
  sw_on = client.get(DEVICE,SWITCH1);
  sw_off = client.get(DEVICE,SWITCH2);

  delay(300);

  if((sw_off == OFF) && (sw_on == OFF)){
    return NOTHING;
  }
  else if ((sw_off == ON) && (sw_on == OFF)){
    check = OFF;
  }
  else if((sw_off == OFF) && (sw_on == ON)){
    check = ON;
  }
  else{
    return NOTHING;
  }

  if((factory_state == ON) && (check == ON)){
    return NOTHING;
  }
  if((factory_state == OFF) && (check == ON)){
    return ON;
  }
  if((factory_state == ON) && (check == OFF)){
    return OFF;
  }
  if((factory_state == OFF) && (check == OFF)){
    return NOTHING;
  }
}
