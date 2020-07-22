/*
 * IoT_box.c
 *
 * Created: 26/06/2020 10:34:23 p.m.
 * Author: Usuario
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lib/ADC.h"
#include "lib/UART.h"
#include "lib/PWM.h"
#include "lib/LCD_16x2.h"

#define set_bit(reg,bit)	(reg |= (1<<bit))
#define clear_bit(reg,bit)	(reg &= ~(1<<bit))
#define toggle_bit(reg,bit)	(reg ^= (1<<bit))
#define get_bit(reg,bit)	(reg &(1<<bit))

//macros - servo
#define RIGHT	0		//0 grades
#define ORIGIN	1		//90 grades
#define LEFT	2		//180 grades

//macros - laser
#define ON	1
#define OFF 0

#define MIN_LIGHT 100
#define MIN_DIST  20



ISR(INT0_vect){		//stop
	IoT_control(OFF);
}
ISR(INT1_vect){		//start
	IoT_control(ON);
}

void general_config(void);
uint8_t LDR_read(void);
void laser_control(uint8_t state);
uint8_t ultra(void);
void servo_motor(void); 
void Uart_tx(uint8_t small_box, uint8_t big_box);
uint8_t Uart_rx(void);
void motor_belt(uint8_t state);
void rele(uint8_t state);
void IoT_control(uint8_t rx);



uint8_t factory_state=ON;
uint8_t message1[] = "Bienvenidos";
uint8_t message2[] = "Total: ";
uint8_t message3[] = "Gr: ";
uint8_t message4[] = "Pq: ";
uint8_t message5[] = "...";


int main(void)
{
	
	general_config();
	
	uint8_t ldr_value,number_small=0,number_big=0,total=0;
	
	uint8_t distance=0,server_comand;
	
	motor_belt(ON);
	laser_control(ON);
	
	LCDGotoXY(2,0);
	LCDstring(message1,11);
	LCDGotoXY(4,1);
	_delay_ms(1000);
	LCDclr();
	
	LCDGotoXY(0,0);
	LCDstring(message2,7);
	LCDGotoXY(0,1);
	LCDstring(message3,4);
	LCDGotoXY(9,1);
	LCDstring(message4,4);
	
    while(1)
    {
		if (factory_state = ON){
			distance = ultra();
			if(distance < MIN_DIST)
			total++;
			
			
			ldr_value = LDR_read();
			
			if (ldr_value < MIN_LIGHT){
				number_big++;
				_delay_ms(3000);
				servo_motor();
			}
			
			number_small = total-number_big;
			LCDGotoXY(8,0);
			LCDstring(total);
			LCDGotoXY(5,1);
			LCDstring(number_big);
			LCDGotoXY(14,1);
			LCDstring(number_small); 
			
			Uart_tx(number_small,number_big);
		}
		    
	
		set_bit(PORTC,1);
		_delay_us(100);
		clear_bit(PORTC,1);
		
		server_comand = Uart_rx();
		
		IoT_control(server_comand);
		
    }
}

void general_config(void){
	
	//LCD configuration
	LCDinit();
	
	//UART configuration
	UART_init(9600);
	
	//ADC configuration
	void ADC_init(void);
	
	
	
	//Port configuration
	
	set_bit(DDRD,1);	//Tx
	clear_bit(DDRD,0);	//Rx
	
	clear_bit(DDRD,2);	//int0-stop
	clear_bit(DDRD,3);	//int1-start
	
	set_bit(PORTD,2);	//pull-up
	set_bit(PORTD,3);
	
	//servo PWM
	
		
	set_bit(DDRD,6);	//Laser
	clear_bit(DDRC,0);	//LDR
	
	clear_bit(PORTD,6);
	
	set_bit(DDRD,7);	//trigger ultra
	clear_bit(DDRD,4);	//echo ultra
	
	clear_bit(PORTD,7);
	
	
	set_bit(DDRB,0);	//rele
	
	set_bit(PORTB,0);
	
	set_bit(DDRB,1);	//motor
	
	clear_bit(PORTB,1);
	
	
	
	set_bit(DDRC,1);	//interrupcion
	clear_bit(PORTD,1);
	
	
	//interrup config
	EICRA |= (1<<1);	//definimos flanco de bajada - INT0
	EICRA &= ~(1<<0);
	
	EICRA |= (1<<3);	//definimos flanco de bajada - INT1
	EICRA &= ~(1<<2);
	
	EIMSK |= (1<<0);	//habilita INT0
	EIMSK |= (1<<1);	//habilita INT1
	
	sei();				//habilitador global
}

void rele(uint8_t state){
	if(state == 0)
		set_bit(PORTB,0);
	
	else
		clear_bit(PORTB,0);
}

void motor_belt(uint8_t state){
	if(state == 0)
		clear_bit(PORTB,1);
		
	else
		set_bit(PORTB,1);
}

void IoT_control(uint8_t rx){
	if(rx = 0){				//stop
		if(factory_state = OFF)
			asm("nop")
		else{
			factory_state = OFF;
			rele(OFF);
		}
	}
	
	else if(rx = 1){		//start
		if(factory_state = ON)
			asm("nop";)
		else{
			factory_state = ON;
			rele(ON);
		}
	else                    //no message
 		asm("nop");		
	}
}

void laser_control(uint8_t state){
	switch(state){
		case 0:
			clear_bit(PORTD,6);
			break;
		case 1:
			set_bit(PORTD,6);
			break;
	}
}

uint8_t LDR_read(void){
	uint8_t value;
	value = ADC_read(0);
	
	return value;
}

uint8_t ultra(void){
	set_bit(PORTD,7);
	_delay_us(10);
	clear_bit(PORTD,7);
	
	int time, distancia;
	
	while((PIND & (1<<4)) == 0){
		asm("nop");
	}
	
	while((PIND & (1<<4)) == 1){
		_delay_us(1);
		time++;
	}
	
	distancia=(time*4)/58;
	
	return distancia;
}

void Uart_tx(uint8_t small_box, uint8_t big_box){
	UART_sendInt(small_box);
	UART_sendInt(big_box);
}

uint8_t Uart_rx(void){
	unsigned char state;
		
	state =	UART_receiveChar_wait();
	
	switch(state){
		case 's':		//stop
			return 0;
			break;
		case 'a':		//start
			return 1;
			break;
	}
}

void servo_motor(void){
	
	cli();
	//PWM configuration
	PWM_init(500);
	PWM_on();
	
	PWM_setDutyA(50);		//0 grades
	_delay_ms(500);
	PWM_setDutyA(75);		//90 grades
	_delay_ms(500);
	
}