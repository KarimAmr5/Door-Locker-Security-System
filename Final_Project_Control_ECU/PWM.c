/******************************************************************************
 *
 * Module: Timer 0 PWM source file
 *
 * File Name: PWM.c
 *
 * Description: Source file for the TIMER0 PWM AVR driver
 *
 * Created on: Feb 20, 2024
 *
 * Author: Karim Amr
 *
******************************************************************************/

/*******************************************************************************
 *                            Required Libraries                               *
 *******************************************************************************/

#include "PWM.h"
#include "std_types.h"
#include "gpio.h"
#include "avr/io.h"

/***************************************************************/
/*					Functions Definitions					   */
/***************************************************************/

/*Initialize and start the PWM of Timer0 with required duty cycle*/
void PWM_Timer0_Start(uint8 duty_cycle)
{
	/*Sets the Enable pin of OC0 as output for PWM signal */
	GPIO_setupPinDirection(DC_MOTOR_EN_PORT,DC_MOTOR_EN_PIN,PIN_OUTPUT);

	if(0 == duty_cycle)
	{
		/*Turns off Timer0-PWM to save power / CPU load*/
		TCCR0 = 0 ;
		OCR0  = 0 ;
		TCNT0 = 0 ;
	}
	else
	{
		/*Sets Timer0 mode on Fast PWM
		 * Set PWM on non-inverting mode
		 * Set FCPU pre-scaler on FCPU/8
		 * to set duty cycle freq. on 500 Hz */
		TCCR0 = (1<<WGM00) |  (1<<WGM01) | (1<<COM01) | (1<<CS01) ;

		/*Counter Starts from Zero */
		TCNT0 = 0;

		/*Sets the duty cycle value according to function argument */
		OCR0 = ((duty_cycle * MAX_TIMER_VALUE)/(100));
	}
}
