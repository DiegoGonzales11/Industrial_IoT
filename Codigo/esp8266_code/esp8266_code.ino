#include <LiquidCrystal_I2C.h>
#include "Ubidots.h"

// set the LCD number of columns and rows
#define lcdColumns = 16;
#define lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

//servo motor
#define SERVO D0
//HMI
#define SCL D1
#define SDA D2
//box detector
#define LASER D3
#define LDR A0
//control factory
#define RELE D4
//pilot
#define RED_PILOT D5
#define GREEN_PILOT D6
//motor
#define MOTOR D7
//ultrasonic
#define ULT_ECHO  D8
#define ULT_TRIG  D2

//wifi
#define DEVICE  "5ccf7fd920e1"  
#define SWITCH1  "sw_on"
#define SWITCH2  "sw_off"
#define TOKEN  "BBFF-GJHTr6N0ao2jF2JaDduVNUIgHzRzRw"   
#define WIFISSID "XIMENA"  
#define PASSWORD "23984789"

char const *totalBox = "totalBox";
char const *bigBox = "bigBox";
char const *smallBox = "smallBox";


Ubidots client(TOKEN);

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

//configuration
void pin_configuration(void);

void servo_control(int state);
int ultrasonic_control(void);
void laser_control(int state);
void LCD_init(void);
void LCD_stop(void);
void LCD_go(void);
void LCD_show(int total,int big, int small);
void pilots_control(int state);
void rele_control(int state);
void motor_control(int state);

int verf_state(void);

//wifi
void send(int value, char const *ptr);
int recieve(void);

//variables globales
int factory_state = ON;
int total_box = 0, big_box = 0, small_box = 0;
int flag_servo = 0; //1=big_box y 0 = small_box
int flag_on = 0;
int flag_off = 0;

void setup(){
  
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

    laser_control(ON);
    delayMicroseconds(50);
    int LDR_value;
    LDR_value = analogRead(LDR);
    if (LDR_value < 50){
      big_box++;
      flag_servo = 1;
    }

    if(ultrasonic_control() == 1){
      total_box++;
    }

    small_box = total_box - big_box;

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

    send(total_box,totalBox);
    send(big_box,bigBox);
    send(small_box,smallBox);
    
    if(verf_state == 0){
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

    if(verf_state == 1){
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
  pinMode(LASER,OUTPUT);
  pinMode(LDR,INPUT);
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
int ultrasonic_control(void){
  digitalWrite(ULT_TRIG,HIGH);
  delayMicroseconds(10);
  digitalWrite(ULT_TRIG,LOW);

  int time,distancia;
  time = pulsIn(ULT_ECHO,HIGH);
  distancia = time*0,01715;
  if(distancia > 6)
    return 0;   //nada
  else
    return 1;   //box
}
void laser_control(int state){
  switch (state){
  case ON:
    digitalWrite(LASER,HIGH);
    break;
  
  case ON:
    digitalWrite(LASER,LOW);
    break;
  }
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

int verf_state(void){
  int state;
  //get 0-off 1-on
  return state;
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
  delay(500);
}

int recieve(void){
  int sw_on, sw_off, check;
  sw_on = client.get(DEVICE,SWITCH1);
  sw_off = client.get(DEVICE,SWITCH2);
  client.send();
  delay(500);
  
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

