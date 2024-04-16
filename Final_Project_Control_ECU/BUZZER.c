/******************************************************************************
 *
 * Module: Buzzer
 *
 * File Name: Buzzer.c
 *
 * Description: Source file for the Buzzer driver
 *
 * Created on: Mar 12, 2024
 *
 * Author: Karim Amr
 *
******************************************************************************/

/*******************************************************************************
 *                            Required Libraries                               *
 *******************************************************************************/

#include "BUZZER.h"
#include "gpio.h"
#include "std_types.h"

/*******************************************************************************
 *                              Functions Definitions                           *
 *******************************************************************************/

/* Description
 * Initialize Pin to output & Turn OFF buzzer
 */
void Buzzer_init(void)
{
	GPIO_setupPinDirection(BUZZER_PORT,BUZZER_PIN,PIN_OUTPUT); /*Set Pin to output*/

	GPIO_writePin(BUZZER_PORT,BUZZER_PIN,LOGIC_LOW); /*Turn Off Buzzer*/

}

/* Description
 * Turns ON Buzzer
 */
void Buzzer_on(void)
{
	GPIO_writePin(BUZZER_PORT,BUZZER_PIN,LOGIC_HIGH); /*Turn On Buzzer*/
}

/* Description
 * Turns OFF Buzzer
 */
void Buzzer_off(void)
{
	GPIO_writePin(BUZZER_PORT,BUZZER_PIN,LOGIC_LOW);/*Turn Off Buzzer*/
}
