
//servo motor
#define SERVO 3
//ultrasonic
#define ULT_ECHO  4
#define ULT_TRIG  5
//box detector
#define LASER 6
#define LDR A0
//control factory
#define RELE 7
//HMI
/*
lcd
*/
//pilot
#define RED_PILOT 8
#define GREEN_PILOT 9

//control macros
#define ON  1
#define OFF 0 

//configuration
void pin_configuration(void);

void servo_control();
void ultrasonic_control();
void laser_control();
void LCD_control();
void pilots_control();


void setup{
  
}
void loop{
  
}

void pin_configuration(void){
  pinMode(SERVO,OUTPUT);
  pinMode(ULT_TRIG,OUTPUT);
  pinMode(ULT_ECHO,INPUT);
  pinMode(SERVO,OUTPUT);
  pinMode(LASER,OUTPUT);
  pinMode(LDR,INPUT);
  pinMode(RELE,OUTPUT);
  pinMode(RED_PILOT,OUTPUT);
  pinMode(GREEN_PILOT,OUTPUT);
}


void servo_control();
void ultrasonic_control();
void laser_control();
void LCD_control();
void pilots_control();
