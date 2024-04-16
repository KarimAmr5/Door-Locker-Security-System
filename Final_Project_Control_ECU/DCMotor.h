/******************************************************************************
 *
 * Module: DCMotor driver header file
 *
 * File Name: DCMotor.h
 *
 * Description: Header file for the DCMotor driver
 *
 * Created on: Feb 20, 2024
 *
 * Author: Karim Amr
 *
******************************************************************************/

#ifndef DCMOTOR_H_
#define DCMOTOR_H_

/*******************************************************************************
 *                            Required Libraries                               *
 *******************************************************************************/


#include "std_types.h"

/*****************************************************************/
/*							   Definitions				      	 */
/*****************************************************************/

/*Select the Motor Driver input pins & ports*/
#define DC_MOTOR_PORT1  PORTB_ID
#define DC_MOTOR_PIN1	PIN2_ID

#define DC_MOTOR_PORT2  PORTB_ID
#define DC_MOTOR_PIN2	PIN1_ID

#define MOTOR_STOP		0   /*Duty Cycle = 0   */
#define MOTOR_RUN		100 /*Duty Cycle = 1000*/

typedef enum {
	STOP,CW,ACW
}DcMotor_State;

/***************************************************************/
/*					Functions Prototypes					   */
/***************************************************************/

/*Initialize the DC MOTOR*/
void DcMotor_init(void);

/*Controls the motors speed and direction according to the given arguments */
void DcMotor_Rotate(DcMotor_State state);

/***************************************************************/



#endif /* DCMOTOR_H_ */
