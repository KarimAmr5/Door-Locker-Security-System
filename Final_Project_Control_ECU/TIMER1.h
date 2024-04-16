/******************************************************************************
 *
 * Module: Timer 1 header file
 *
 * File Name: TIMER1.h
 *
 * Description: Header file for the TIMER1 AVR driver
 *
 * Created on: Mar 11, 2024
 *
 * Author: Karim Amr
 *
******************************************************************************/

#ifndef TIMER1_H_
#define TIMER1_H_

/*******************************************************************************
 *                            Required Libraries                               *
 *******************************************************************************/

#include "common_macros.h"
#include "std_types.h"

/*******************************************************************************
 *                              Type Definitions                               *
 *******************************************************************************/
typedef enum {
	Normal,Compare
}Timer1_Mode;

typedef enum {
	Stop,FCPU_1,FCPU_8,FCPU_64,FCPU_256,FCPU_1024
}Timer1_Prescaler;


typedef struct {
 uint16 initial_value;
 uint16 compare_value; /* it will be used in compare mode only. */
 Timer1_Prescaler prescaler;
 Timer1_Mode mode;
 } Timer1_ConfigType;

 /*******************************************************************************
  *                             Functions Prototypes                            *
  *******************************************************************************/

/* Description :
 * Initialize TIMER1 with required configuration from Config_Ptr*/
void Timer1_init(const Timer1_ConfigType * Config_Ptr);

/* Description :
 * Uninitialize /TurnOff TIMER1 and reset its counter/settings registers */
void Timer1_deInit(void);

/* Description :
 * Sent the address of the required function to be called at ISR toggle
 * from higher/different abstraction level */
void Timer1_setCallBack(void(*a_ptr)(void));


#endif /* TIMER1_H_ */
