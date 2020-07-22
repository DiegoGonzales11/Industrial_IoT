/*
 * UART.c
 *
 *  Author: Diego
 */ 

#define F_CPU 16000000UL   //change this if you use a different crystal

#include <avr/io.h>
#include <stdlib.h>
#include "usart.h"


/*
This function make the basic configuration to have a simple 
communication of 8 bits and without parity
*/
void UART_init( unsigned int baud)
{
	unsigned int ubrr;
	ubrr = (F_CPU/16/baud)-1;
	UCSR0B=0b00011000;	//transmisión y recepción habilitados a 8 bits
	UCSR0C=0b00000110;	//asíncrono, sin bit de paridad, 1 bit de parada a 8 bits
	UBRR0=ubrr;
}

/*
This function disable the UART communication
*/
void UART_end(void)
{
	UCSR0B = 0X00;
}


/*
This function send any kind of character in the ASCII code 
*/
void UART_sendChar( unsigned char data )
{
	while(!(UCSR0A & (1<<UDRE0)));		// mientras el registro UDR0 esté lleno espera
	UDR0 = data;						//cuando el el registro UDR0 está vacio se envia el caracter
}

/*
You can send any integer number between -32 768 and 32767
if you want to send a higher number you can use another 
function.

The number can be form anything format, but their have to 
be in this range.
*/
void UART_sendInt(int data)		
{
	char data_str[5];
	itoa(data,data_str,10);
	UART_sendString_ln(data_str);
}

/*
You can send any decimal number, in the range of the float type, 
you can send any kind of format but always in this range.
*/
void UART_sendFloat(float data, uint8_t decimal)
{
	int data_int, data_dec;
	data_int = (int) data;
	char data_str[5];
	float dot_decimal = (data - data_int)*10*decimal;
	
	if ( decimal != 0)
	{
		data_dec = (int) dot_decimal;
		itoa(data_int,data_str,10);
		UART_sendString(data_str);
		UART_sendChar(46);
		itoa(data_dec,data_str,10);
		UART_sendString(data_str);
		UART_sendChar(10);
	}
	else
	{
		itoa(data_int,data_str,10);
		UART_sendString(data_str);
		UART_sendChar(10);
	}
}

/*
This function check if you have a char in the buffer, and you have a character, 
the function give you the value and if the buffer is clear, you will receive 
zero.
*/
unsigned char UART_receiveChar(void)
{
	if(UCSR0A & (1<<RXC0))		//si el bit7 del registro UCSR0A se ha puesto a 1
		return UDR0;				//devuelve el dato almacenado en el registro UDR0
	
	else
		return 0 ;
}

/*
The function stopped the program waiting to receive a character.
*/
unsigned char UART_receiveChar_wait(void){
	while ((UCSR0A & (1<<RXC0)) == 0);	//esperar hasta que llegue el dato
	return(UDR0);						//devolver el dato
}

/*
The function can send a text, and you have to give it the pointer of the array,
or write the array directly in the function.
*/
void UART_sendString(char *str)
{
	while(*str != 0x00)			//mientras el último valor de la cadena sea diferente al caracter nulo
	{
		UART_sendChar(*str);		//transmite los caracteres de cadena
		str++;
	}
}

/*
The function can send a text, and you have to give it the pointer of the array,
or write the array directly in the function.

After to send the array, you will pass to the next line.
*/
void UART_sendString_ln(char *str)
{
		while(*str != 0x00)			//mientras el último valor de la cadena sea diferente al caracter nulo
		{
			UART_sendChar(*str);		//transmite los caracteres de cadena
			str++;
		}
		
		UART_sendChar(10);			//new line
}

