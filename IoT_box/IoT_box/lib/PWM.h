/*
 * IoT_box.c
 *
 * 
 * Author: Usuario
 */ 



#ifndef PWM_H_
#define PWM_H_

	#include <avr/io.h>

	void PWM_init(int freq);
	void PWM_on();
	void PWM_off();

	void PWM_setDutyA(int duty);

#endif /* PWM_H_ */