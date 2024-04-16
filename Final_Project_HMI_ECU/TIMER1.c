/******************************************************************************
 *
 * Module: Timer 1 source file
 *
 * File Name: TIMER1.c
 *
 * Description: Source file for the TIMER1 AVR driver
 *
 * Created on: Mar 11, 2024
 *
 * Author: Karim Amr
 *
******************************************************************************/

/*******************************************************************************
 *                            Required Libraries                               *
 *******************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "common_macros.h"

#include "TIMER1.h"

/*******************************************************************************
 *                          Local Variable declaration                         *
 *******************************************************************************/

static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                             Functions Definitions                           *
 *******************************************************************************/

/* Description :
 * Initialize TIMER1 with required configuration from Config_Ptr*/
void Timer1_init(const Timer1_ConfigType * Config_Ptr)
{
	/*Set Initial counting value inside TCNT1*/
	TCNT1 = Config_Ptr->initial_value;

    /*************************************************
	***************** UCSRB Settings ****************
	*  COM1A1:0 = 0 Disable Compare Output Mode for Compare unit A
	*  COM1B1:0 = 0 Disable Compare Output Mode for Compare unit B
	*  FOC1A  	= 1 Force Output Compare for Compare unit A
	*  FOC1B 	= 1 Force Output Compare for Compare unit B
	*  WGM11:0  = 0 Waveform Generation Mode (ZERO for both Normal & CTC)
	*************************************************/
	TCCR1A = (1<<FOC1A) | (1<<FOC1B) ;

	/*************************************************
	***************** TCCR1B Settings ****************
	*  ICNC1   = 0 		     Disable Input Capture Noise Canceler
	*  ICES1   = 0 		     Disable Input Capture Edge Select
	*  WGM13:2 = Config_PTR  Waveform Generation Mode ( Set in next Switch statement )
	*  CS12:0  = Config_PTR  Clock Select (Pre-scalar)
	*
	*  BIT # 5 must be written to zero when writing in TCCR1B
	*************************************************/
	TCCR1B = (TCCR1B & ~(0x07)) | ( 0x07 & (Config_Ptr->prescaler) );

	/*************************************************
	***************** TIMSK Settings ****************
	*  TICIE1 = 0  			Input Capture Interrupt Enable
	*  OCIE1A = Config_PTR  Output Compare A Match Interrupt Enable
	*  OCIE1B = 0			Output Compare B Match Interrupt Enable
	*  TOIE1  = Config_PTR  Overflow Interrupt Enable
	*************************************************/
	switch(Config_Ptr->mode)
	{
	case Normal:

		SET_BIT(TIMSK,TOIE1); /*Enable Input Capture Interrupt */

		break;

	case Compare :

		SET_BIT(TIMSK,OCIE1A); /*Enable Output Compare A Match Interrupt*/

		OCR1A = Config_Ptr->compare_value;

		TCCR1B |= (1<<WGM12); /*Set wave generation mode to CTC*/

		break;
	}

	SET_BIT(SREG,7);
}

/* Description :
 * Uninitialize /TurnOff TIMER1 and reset its counter/settings registers */
void Timer1_deInit(void)
{
	/*Reset all the previously set Registers back to default state */

	CLEAR_BIT(SREG,7);

	TCNT1 = 0;
	OCR1A = 0;
	TCCR1A = 0 ;
	TCCR1B = 0 ;

	/* 0b11000011*/
	TIMSK &= 0xC3 ;

	/*Set the callback pointer back to NULL */
	g_callBackPtr = NULL_PTR;
}

/* Description :
 * Sent the address of the required function to be called at ISR toggle
 * from higher/different abstraction level */
void Timer1_setCallBack(void(*a_ptr)(void)){

	g_callBackPtr = a_ptr;
}


/*******************************************************************************
 *                                ISR Definitions 	                           *
 *******************************************************************************/

ISR(TIMER1_COMPA_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		g_callBackPtr();
		//(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}

ISR(TIMER1_OVF_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */

		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}

/*******************************************************************************/
