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
#define DEVICE  "5ccf7fd91701" //LA DIRECCION DE TU CUENTA EN LA PLATAFORMA IBIDOTS
#define SWITCH1  "sw_on"//
#define SWITCH2  "sw_off"
#define TOKEN  "BBFF-uhuuvMLx0B8BkrdbxHp9efBkKKb1bp"   
#define WIFISSID "XIMENA"  
#define PASSWORD "23984789" 

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

#define SW_ON  "sw_on"
#define SW_OFF "sw_off"

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

//int i=0;

void setup(){
  pin_configuration();
  LCD_init();
  Serial.println("\n\ngo\n\n");
}

void loop(){
  Serial.print("factp state: ");
  Serial.println(factory_state);
  if(factory_state == ON){
    Serial.println("\n\nok\n\n");
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
      flag_servo = 1;
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
      delay(2000);
      servo_control(OFF);
      delay(1000);
      servo_control(BACK);
      delay(2000);
      servo_control(OFF);
      motor_control(ON);
      flag_servo = 0;
    }

    ///////////////////////////
    /*
    big_box = i;
    small_box = i+1;
    total_box = big_box + small_box;
    i++;*/
    ///////////////////////////

    //LCD_show(total_box,big_box,small_box);
    
    send(total_box,ubidot_total);
    send(big_box,ubidot_big);
    send(small_box,ubidot_small);

    Serial.println(total_box);
    Serial.println(big_box);
    Serial.println(small_box);


    
    int test_ubi = receive();
    Serial.print("factp state: ");
    Serial.println(factory_state);
    Serial.print("test: ");
    Serial.println(test_ubi);
    if(test_ubi == 0){
      factory_state = OFF;
      flag_off = ON;
    }
    Serial.print("factp state: ");
    Serial.println(factory_state);
  }
  else{
    Serial.println("error");
    if(flag_off == 1){
      LCD_stop();
    }
    flag_off = 0;
    rele_control(OFF);
    pilots_control(OFF);
    motor_control(OFF);

    delay(1000);
    
    if(receive() == 1){
      factory_state = ON;
      flag_on = ON;
    }
  }
}

/////////////////////////////////////////////

void pin_configuration(void){
  //Serial config
  Serial.begin(9600);
  //Wifi config
  client.wifiConnect(WIFISSID, PASSWORD);
  //Pin config
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
  case GO:          //rotate 90 degrees 
    analogWrite(SERVO,100);
    break;
  case BACK:       //rotate -90 degrees
    analogWrite(SERVO,76);
    break;
  case OFF:       //Turn off servo
    analogWrite(SERVO,0);
    break;
  }
}
/////////////// Distance acquisition by treating the "time" parameter/////////////////////////////
 
int ultrasonic_control(void){
  digitalWrite(ULT_TRIG,HIGH);/////////////////////////////////
  delayMicroseconds(10);     //Pulse of exitation 
  digitalWrite(ULT_TRIG,LOW);/////////////////////////////////

  int time,distancia;
  time = pulseIn(ULT_ECHO,HIGH);
  distancia = time/58.2;//pulse width treatment to obtain the distance in centimeters

  Serial.println("\nlectura");
  Serial.println(distancia);
  
  return distancia;
}
//////////////////////////////////////////////////////////////////////////////////////////////////

///////////////we check what box size//////////////////////////////////////////////////////////////
int check_box(void){ 
  int distance;
  distance = check_ultrasonic();
  
  if(distance < 15)
    return BIG;
  else if((distance >15) && (distance < 25))
    return SMALL;
  else
    return NO_BOX;
}
/////////////////////////////////////////////////////////////////////////////////////////////////

////////////////WE AVERAGE FIVE SAMPLES OF DISTANCES OBTAINED///////////////////////////////////
int check_ultrasonic(void){
  int sample[5];
  int avrg=0;

  for(int i=0; i<5; i++){
    sample[i] = ultrasonic_control();
  }

  for(int i=0; i<5; i++){
    avrg = avrg + sample[i];
  }
  Serial.println("\nAvg");
  Serial.println(avrg);
  return avrg/5;
}
////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////WE CONTROL THE STATE OF THE RED AND BLUE LEDS////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////WE CONTROL THE STATE OF THE RELAY//////////////////////////////////////////////
void rele_control(int state){
  if (state == ON)
    digitalWrite(RELE,HIGH);
  else
    digitalWrite(RELE,LOW);
}



void motor_control(int state){
  if(state == ON)
    digitalWrite(MOTOR,HIGH);
  else
    digitalWrite(MOTOR,LOW);
}
////////////////////////////////////////////////////////////////////////////////////////////////

///////////////WE ASSIGN MESSAGES//////////////////////////////////////////////////
void LCD_init(void){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(msg1);//"Welcome to"
  lcd.setCursor(0,1);
  lcd.print(msg2);//"the factory"
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(msg3);//"Please wait"
  lcd.setCursor(0,1);
  lcd.print(msg4);//"loading ..."
  delay(1000);
}
////////////////////////////////////////////////////////////////////////////////////////////////

////////////////WE ASSIGN THE MESSAGE//////////////////////////////////////////////////////////
void LCD_stop(void){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(msg8);//"Factory stopped"
  lcd.setCursor(0,1); 
  lcd.print(msg9);//"plase wait"
}
////////////////////////////////////////////////////////////////////////////////////////////////
 
////////////////WE ASSIGN THE MESSAGE//////////////////////////////////////////////////////////
 void LCD_go(void){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(msg10);//"Factory starting"
  lcd.setCursor(0,1);
  lcd.print(msg9);//"plase wait"
}
////////////////////////////////////////////////////////////////////////////////////////////////
 
////////////////WE ASSIGN THE MESSAGE//////////////////////////////////////////////////////////
void LCD_show(int total,int big, int small){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(msg5);//"Total box: "
  lcd.setCursor(11,0);
  lcd.print(total);//TOTAL BOXES

  lcd.setCursor(0,1);
  lcd.print(msg6);//"Big:"
  lcd.setCursor(5,1);
  lcd.print(big);//AMOUNT OF BIG BOXES

  lcd.setCursor(7,1);
  lcd.print(msg7);//"Sml:"
  lcd.setCursor(11,1);
  lcd.print(small);//AMOUNT OF SMALL BOXES
}
////////////////////////////////////////////////////////////////////////////////////////////////
 
////////////////SENDING DATA TO THE UBIDOTS PLATFORM ///////////////////////////////////////////
void send(int value, char const *ptr){
  client.add(ptr,value);
  client.send();
  delay(100);
}
////////////////////////////////////////////////////////////////////////////////////////////////
 
////////////////SENDING DATA TO THE UBIDOTS PLATFORM ///////////////////////////////////////////
int receive(void){
  Serial.println("receive");
  int sw_on, sw_off, check;
  sw_on = client.get(DEVICE,SW_ON);
  sw_off = client.get(DEVICE,SW_OFF);
  Serial.println("Gotcha");
  Serial.println(sw_on);
  Serial.println(sw_off);
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
////////////////////////////////////////////////////////////////////////////////////////////////
