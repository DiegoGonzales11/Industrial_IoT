/*
 * UART.h
 *
 *  Author: Diego
 */ 


#ifndef UART_H_
#define UART_H_

void UART_init(unsigned int baud);
void UART_end(void);

void UART_sendChar(unsigned char data);
void UART_sendInt(int data);
void UART_sendFloat(float data, uint8_t decimal);
void UART_sendString(char *str);
void UART_sendString_ln(char *str);

unsigned char UART_receiveChar(void);
unsigned char UART_receiveChar_wait(void);


#endif /* UART_H_ */