/******************************************************************************
 *
 * Project: Door Locker Security System (Standard Embedded Diploma Final Project )
 *
 * File Name: Final_Project_Control_ECU.c
 *
 * Created on: Mar 14, 2024
 *
 * Author: Karim Amr
 *
 * Group = 84
 *
 * Description: System that controls Locking/Unlocking door motor using
 * 				password to be set by user
 *
 * 				This is the Control module that controls the system hardware
 * 				such as EEPROM , Motor , Buzzer .
 *
 * 				This code was able to achieve a maximum on only 52% CPU load in Proteus !
 *
******************************************************************************/


/*******************************************************************************
 *                            Required Libraries                               *
 *******************************************************************************/

#include "BUZZER.h"
#include "external_eeprom.h"
#include "DCMotor.h"
#include "USART.h"
#include "twi.h"
#include "TIMER1.h"
#include <util/delay.h> /*To use simple delay functions*/

/********************************************************************************
 *                                 Definitions     	 	                        *
 ********************************************************************************/

/*Length of the Password*/
#define PASSWORD_LENGTH 5

/*Motor operation time in Seconds*/
#define MOTOR_START_TIME 			0
#define MOTOR_UNLOCKING_TIME 		15
#define MOTOR_STOP_TIME  			3
#define MOTOR_STOPPING_TIME 		(MOTOR_UNLOCKING_TIME+MOTOR_STOP_TIME)
#define MOTOR_LOCK_TIME   			15
#define MOTOR_LOCKING_TIME			(MOTOR_UNLOCKING_TIME+MOTOR_STOP_TIME+MOTOR_LOCK_TIME)


#define PASSWORD_ADDRESS 0x0100 /*EEPROM password memory address*/

#define LOCK_DURATION 60000 /*60 Seconds*/

/*From Enum Application State in HMI ECU*/
#define UNMATCHED_PASSWORD  1
#define MAIN_MENU			0

/*-----------------------------------------------------------------------------*/

typedef enum
{
	EmptyPW , IncorrectPW , CorrectPW
}Password_Results;

typedef enum
{
	Loop , SetPW , EnterPW , OpenningDoor , LockedMode , EmptyLoop
}UART_commands;

/********************************************************************************
 *                              Global Variables	                            *
 ********************************************************************************/

UART_commands UART_nextState ;

/********************************************************************************
 *                              Function Prototypes	                            *
 ********************************************************************************/

/*Description:
 * Get the two password entries from HMI ECU for matching
 * and to set it in EEPROM if they match */
void setPassword();

/*Description:
 * Compare each digit of the two password entries arrays
 * if they match , will return feedback to save it inside EEPROM
 * OR if the mismatch , will return feedback to try again */
uint8 passwordMatching(const uint8 * a_firstEntry , const uint8 * a_secondEntry);

/*Description:
 * Receive password from HMI ECU to compare it with set password in EEPROM
 * in order to proceed forward with the desired operation */
void passwordEntry(void);

/*Description:
 * Compares the received password with the one set inside
 * the EEPROM and return with the relevant matching result*/
uint8 passwordEntryMatching(const uint8 * pwBuffer);

/*Description:
 * I it starts  Timer 1 and send feedback to HMI ECU ,
 * then it starts unlocking , stopping & locking the door by moving the motor
 * in the desired direction and duration */
void doorAction(void);

/*Description:
 * Activate Locked mode on the whole system & activates the buzzer for the
 * required amount of time and returns back to normal operation
 *  ( sent back to Loop) */
void lockedMode(void);

/********************************************************************************
 *                              Application Code	                            *
 ********************************************************************************/



int main()
{
 /************************ Configuration Structure *********************/

   /***************** UART Settings ****************
	*  Data Bit Mode 		= 8 Bits
	*  Parity Check 		= Even Parity
	*  Number of Stop Bits  = One
	*  Baud Rate			= 9600
	*************************************************/
	UART_ConfigType s_UARTconfig = {EightBit,EvenParity,OneStopBit,9600};

   /***************** Timer 1 Settings ****************
	*  Initial Value 	= 0
	*  Compare Value 	= 7813 ( every one second )
	*  Pre-Scalar 		= F_CPU/1024
	*  Timer1 Mode		= CTC (Compare Mode)
	*************************************************/
	Timer1_ConfigType s_Timer1Config = {0,7813,FCPU_1024,Compare};

   /***************** TWI(I2) Settings ****************
	*  Address  = 10
	*  Bit Rate = 400 Kb/s
	*************************************************/
	TWI_ConfigType s_TWIconfig = {0b00001010,Fast_Mode};


	/*********************************************************************/

	/************************ Drivers Initializations *********************/

	UART_init(&s_UARTconfig);

	_delay_ms(10);  /*Allow time for transmission & initialization*/

	TWI_init(&s_TWIconfig);

	_delay_ms(10);  /*Allow time for transmission & initialization*/

	DcMotor_init();

	Buzzer_init();

	/*Set the UART to state to ready until command (Byte) is received*/
	UART_nextState = UART_recieveByte();

	/***************************** Main Loop ****************************/

	while(1)
	{
		/*Checks the required application function*/
		switch(UART_nextState)
		{

		case Loop: /*Ready mode until command is received*/
			UART_nextState = UART_recieveByte();
			break;

		case SetPW: /*Match & set password inside EEPROM*/
			setPassword();
			break;

		case EnterPW: /*Compare the entered password with the one saved inside EEPROM*/
			passwordEntry();
			break;

		case OpenningDoor: /*Start door opening operation */

			/*Set the function to be called by TIMER 1 ISR*/
			Timer1_setCallBack(&doorAction);

			/*Initiate TIMER with the required configuration*/
			Timer1_init(&s_Timer1Config);

			/*Send feedback command to HMI ECU for time synchronization*/
			UART_sendByte(UART_nextState);

			/*Send the application to empty loop until the operation is complete*/
			UART_nextState = EmptyLoop ;

			/*To prevent the One second delay until the first Interrupt occurs*/

			/*Requires that the counter if conditions to be incremented by an extra ONE
			* for correct timing adjustment*/
			 /*doorAction();*/

			break;

		case LockedMode :/*Enters the system into locked mode for predefined amount of time */

			lockedMode();
			break;

		case EmptyLoop :
			/*The target from the empty loop is to make the application
			 * do the lowest possible tasks to reduce CPU load
			 * until the Application status is updated
			 * & to prevent confliction between tasks*/
			break;
		}
	}
}

/********************************************************************************
 *                              Functions Definitions                           *
 ********************************************************************************/

void setPassword()
{
	/*2 Local array of password size to receive the password digit in it*/
	uint8 firstPasswordEntry[PASSWORD_LENGTH] = {0} ;
	uint8 secondPasswordEntry[PASSWORD_LENGTH] = {0} ;

	uint8 checkResults = 0;

	uint8 passwordDigit ;

	/*Receive first password Entry*/
	UART_recieveData(firstPasswordEntry,PASSWORD_LENGTH);

	_delay_ms(100); /*Allow time for transmission*/

	/*Receive second password Entry*/
	UART_recieveData(secondPasswordEntry,PASSWORD_LENGTH);

	_delay_ms(100); /*Allow time for transmission*/

	/*Return the results of the password matching inside checkResults */
	checkResults = passwordMatching(firstPasswordEntry,secondPasswordEntry);


	/*If password is unmatched , send the required command to HMI ECU to try again*/
	if(1 == checkResults)
	{
		UART_sendByte(UNMATCHED_PASSWORD);
	}
	/*If password is matched , save the password inside EEPROM memory at the pre-defined location*/
	else if (0 == checkResults)
	{
		for(passwordDigit = 0 ; passwordDigit < PASSWORD_LENGTH ; passwordDigit++)
		{
			EEPROM_writeByte((PASSWORD_ADDRESS)+passwordDigit,firstPasswordEntry[passwordDigit]);
			_delay_ms(20); /*Allow transmition time*/
		}
		/*send the required command to HMI ECU to proceed to Main Menu*/
		UART_sendByte(MAIN_MENU);
	}
	/*Set application status back to ready mode*/
	UART_nextState = Loop;
}

uint8 passwordMatching(const uint8 * a_firstEntry , const uint8 * a_secondEntry)
{
	uint8 mismatchFlag = 0; /*Default value for no mismatch */

	uint8 passwordDigit ;

	/*Loop on the two password entries and check if there is any mismatch*/
	for(passwordDigit = 0 ; passwordDigit < PASSWORD_LENGTH ; passwordDigit++)
	{
		if(a_firstEntry[passwordDigit] != a_secondEntry[passwordDigit])
		{
			/*If mismatch occur , return with the results & break the loop*/
			mismatchFlag = 1;
			break;
		}
	}
	/*If no mismatch occurs , just return the default value*/
	return mismatchFlag ;
}

void passwordEntry(void)
{
	uint8 checkResults = 0;

	/*Local array of password size to receive the password digits in it*/
	uint8 passwordBuffer[PASSWORD_LENGTH] = {0};

	/*Receive the password Entry*/
	UART_recieveData(passwordBuffer,PASSWORD_LENGTH);

	_delay_ms(100); /*Allow time for transmission*/

	/*Send the for password comparing with the one saved in EEPROM
	 * & return the results*/
	checkResults = passwordEntryMatching(passwordBuffer);

	/*Send the relative command whether it's correct / incorrect*/
	checkResults==1?UART_sendByte(IncorrectPW):UART_sendByte(CorrectPW);

	/*Set application status back to ready mode*/
	UART_nextState = Loop;
}

uint8 passwordEntryMatching(const uint8 * pwBuffer)
{
	uint8 mismatchFlag = 0; /*Default value for no mismatch */

	uint8 passwordDigit ;

	/*Local array of password size call in the saved password from EEPROM*/
	uint8 savedPassword[PASSWORD_LENGTH] = {0};

	for(passwordDigit = 0 ; passwordDigit < PASSWORD_LENGTH ; passwordDigit++)
	{
		/*Read the saved password digits from EEPROM pre-define location
		 * set it inside the local array*/
		EEPROM_readByte((PASSWORD_ADDRESS)+passwordDigit,&savedPassword[passwordDigit]);
		_delay_ms(20); /*Allow transmition time*/

		/*Loop on the password entry and check if there is any mismatch
		 * with the saved password in EEPROM*/
		if(pwBuffer[passwordDigit] != savedPassword[passwordDigit])
		{
			/*If mismatch occur , return with the results & break the loop*/
			mismatchFlag = 1;
			break;
		}
	}
	/*If no mismatch occurs , just return the default value*/
	return mismatchFlag ;
}

void doorAction(void)
{
	/*Set the counter to static to increase lifetime through out
	 * the run time & set to volatile to disable compiler
	 * optimization on the variable*/
	static volatile uint8 doorTimerCounter = MOTOR_START_TIME;

	/*Timer counter value is within unlocking mode*/
	if(doorTimerCounter < MOTOR_UNLOCKING_TIME)
	{
		/*Instead of setting DC_Motor state at each loop
		 * in which it sets the PWM settings and state
		 * every single time , just set at the first of
		 * each state to save CPU Load & context switching*/
		if(MOTOR_START_TIME == doorTimerCounter)

		/*Rotate Motor Clockwise at full speed*/
		DcMotor_Rotate(CW);
		/*Increment timer by one , which means one second has passed */
		doorTimerCounter++;
	}

	/*Timer counter value is within stop mode*/
	else if (doorTimerCounter >=MOTOR_UNLOCKING_TIME && doorTimerCounter  <MOTOR_STOPPING_TIME)
	{
		if(MOTOR_UNLOCKING_TIME == doorTimerCounter)
		/*Stop the motor rotation*/
		DcMotor_Rotate(STOP);
		/*Increment timer by one , which means one second has passed */
		doorTimerCounter++;

	}

	/*Timer counter value is within locking mode*/
	else if(doorTimerCounter >= MOTOR_STOPPING_TIME && doorTimerCounter  < MOTOR_LOCKING_TIME)
	{
		if(MOTOR_STOPPING_TIME == doorTimerCounter)
		DcMotor_Rotate(ACW);
		/*Increment timer by one , which means one second has passed */
		doorTimerCounter++;
	}

	/*IF Timer counter reached the last required time , Stop DC motor ,
	* Stop / DeInit. TIMER 1 , reset counter & set back application to ready mode*/
	else if(doorTimerCounter == MOTOR_LOCKING_TIME)
	{
		DcMotor_Rotate(STOP);
		Timer1_deInit();
		doorTimerCounter=0;
		UART_nextState = Loop ;
	}
}

void lockedMode(void)
{
	uint8 LockTimer ;


	/*Loop on the following function for predefined duration
	 * to Lock the system functions & operations */
	for(LockTimer=0 ; LockTimer<(LOCK_DURATION/500) ; LockTimer++) /*60000/500 = 120*/
	 {
		/*Creates a Beeping Sound :D*/
		///////*Alternative method = using Timer 2 PWM wave generation method*//////
		Buzzer_on();

		_delay_ms(250);

		Buzzer_off();

		_delay_ms(250);
	 }

	/*Set application back to ready mode*/
	UART_nextState=Loop;

	/*Sends feedback to HMI ECU that the locked mode has been exited*/
	UART_sendByte(UART_nextState);
}

/**********************************************************************/
