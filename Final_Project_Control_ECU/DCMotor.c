/******************************************************************************
 *
 * Module: DCMotor driver source file
 *
 * File Name: DCMotor.c
 *
 * Description: Source file for the DCMotor driver
 *
 * Created on: Feb 20, 2024
 *
 * Author: Karim Amr
 *
******************************************************************************/


/*******************************************************************************
 *                            Required Libraries                               *
 *******************************************************************************/


#include "DCMotor.h"
#include "gpio.h"
#include "PWM.h"
#include "std_types.h"

/*******************************************************************************
*							  Functions Definitions 	    				   *
********************************************************************************/

/* Description :
 * Initialize the DC MOTOR by
 * setting required pins to output
 * and set its initial state to : STOP*/
void DcMotor_init(void)
{
	/*set both pins to output */
	GPIO_setupPinDirection(DC_MOTOR_PORT1,DC_MOTOR_PIN1,PIN_OUTPUT);
	GPIO_setupPinDirection(DC_MOTOR_PORT2,DC_MOTOR_PIN2,PIN_OUTPUT);

	/*Set the motor to STOP position*/
	GPIO_writePin(DC_MOTOR_PORT1,DC_MOTOR_PIN1,LOGIC_LOW);
	GPIO_writePin(DC_MOTOR_PORT2,DC_MOTOR_PIN2,LOGIC_LOW);

	/*Set initial speed to ZERO using PWM driver */
	PWM_Timer0_Start(MOTOR_STOP);
}

/* Description :
 * set both motor speed and direction by the given function arguments
 * State : STOP OR CW OR ACW*/
void DcMotor_Rotate(DcMotor_State state)
{
	switch(state)
	{
	case STOP:
		/*Set Direction*/
		GPIO_writePin(DC_MOTOR_PORT1,DC_MOTOR_PIN1,LOGIC_LOW);
		GPIO_writePin(DC_MOTOR_PORT2,DC_MOTOR_PIN2,LOGIC_LOW);

		/*Set speed */
		PWM_Timer0_Start(MOTOR_STOP); /*OR ZERo ?*/
		break;
	case ACW :
		/*Set Direction*/
		GPIO_writePin(DC_MOTOR_PORT1,DC_MOTOR_PIN1,LOGIC_LOW);
		GPIO_writePin(DC_MOTOR_PORT2,DC_MOTOR_PIN2,LOGIC_HIGH);

		/*Set speed */
		PWM_Timer0_Start(MOTOR_RUN);
		break;
	case CW:
		/*Set Direction*/
		GPIO_writePin(DC_MOTOR_PORT1,DC_MOTOR_PIN1,LOGIC_HIGH);
		GPIO_writePin(DC_MOTOR_PORT2,DC_MOTOR_PIN2,LOGIC_LOW);

		/*Set speed */
		PWM_Timer0_Start(MOTOR_RUN);
		break;
	}
}

