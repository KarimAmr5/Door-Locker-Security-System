/******************************************************************************
 *
 * Module: Timer 0 PWM header file
 *
 * File Name: PWM.h
 *
 * Description: Header file for the TIMER0 PWM AVR driver
 *
 * Created on: Feb 20, 2024
 *
 * Author: Karim Amr
 *
******************************************************************************/

#ifndef PWM_H_
#define PWM_H_

/*******************************************************************************
 *                            Required Libraries                               *
 *******************************************************************************/

#include "std_types.h"

/*****************************************************************/
/*							   Definitions				      	 */
/*****************************************************************/

/*Define the port and pin number for EN pin of H-bridge */
#define DC_MOTOR_EN_PORT    PORTB_ID
#define DC_MOTOR_EN_PIN  	PIN3_ID

#define MAX_TIMER_VALUE 	255


/***************************************************************/
/*					Functions ProtoTypes					   */
/***************************************************************/

/*Initialize and start the PWM of Timer0 with required duty cycle*/
void PWM_Timer0_Start(uint8 duty_cycle);

#endif /* PWM_H_ */
