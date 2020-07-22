/*
 * ADC.c
 * 
 * Author: Diego
 */ 
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "ADC.h"

void ADC_init(void)
{
	//ADC config
	ADCSRA |= (1<<ADEN);//ADC enable
	ADCSRA |= ((1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0)); //ADC clock
	
	ADMUX |= (1<<REFS0);	//reference voltage 
	ADMUX &= ~(1<<REFS1);
	
}


int ADC_read(char chanel)
{
	int Ain,AinLow;
	
	switch(chanel){
		case 0:
			ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (1<<MUX0));
			break;
		case 1:
			ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1));
			ADMUX |= (1<<MUX0);
			break;
		case 2:
			ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX0));
			ADMUX |= (1<<MUX1);
			break;
		case 3:
			ADMUX &= ~((1<<MUX3) | (1<<MUX2));
			ADMUX |= ((1<<MUX0) | (1<<MUX1));
			break;
		case 4:
			ADMUX &= ~((1<<MUX3) | (1<<MUX1) | (1<<MUX0));
			ADMUX |= (1<<MUX2);
			break;
		case 5:
			ADMUX &= ~((1<<MUX3) | (1<<MUX1) );
			ADMUX |= ((1<<MUX2) | (1<<MUX0));
			break;
		case 6:		//only for ATmega328 smd
			ADMUX &= ~((1<<MUX2) | (1<<MUX0) );
			ADMUX |= ((1<<MUX3) | (1<<MUX1));
			break;
		case 7:		//only for ATmega328 smd
			ADMUX &= ~(1<<MUX3);
			ADMUX |= ((1<<MUX2) | (1<<MUX1) | (1<<MUX0));
			break;
	}
	

	ADCSRA |= (1<<ADSC);		//start measure 
	while((ADCSRA&(1<<ADIF))==0);	//wait the flag 
	
	_delay_us(10);
	
	AinLow = (int)ADCL;		
	Ain = (int)ADCH*256;		
			
	Ain = Ain + AinLow;				
	return(Ain);			//return the value
}