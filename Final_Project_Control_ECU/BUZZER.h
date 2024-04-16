/******************************************************************************
 *
 * Module: Buzzer
 *
 * File Name: Buzzer.h
 *
 * Description: Header file for the Buzzer driver
 *
 * Created on: Mar 12, 2024
 *
 * Author: Karim Amr
 *
******************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_


/*******************************************************************************
 *                                    Definitions                               *
 *******************************************************************************/

#define BUZZER_PORT 	PORTB_ID
#define BUZZER_PIN		PIN4_ID

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/* Description
 * Initialize Pin to output & Turn OFF buzzer
 */
void Buzzer_init(void);

/* Description
 * Turns ON Buzzer
 */
void Buzzer_on(void);

/* Description
 * Turns OFF Buzzer
 */
void Buzzer_off(void);

#endif /* BUZZER_H_ */
