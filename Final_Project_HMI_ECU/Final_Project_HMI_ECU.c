/******************************************************************************
 *
 * Project: Door Locker Security System (Standard Embedded Diploma Final Project )
 *
 * File Name: Final_Project_HMI_ECU.c
 *
 * Created on: Mar 11, 2024
 *
 * Author: Karim Amr
 *
 * Group = 84
 *
 * Description: System that controls Locking/Unlocking door motor using
 * 				password to be set by user
 *
 * 				This is the HMI interface that allows user to control the system
 *
 * 				This code was able to achieve a maximum on only 52% CPU load in Proteus !
 *
******************************************************************************/


/*******************************************************************************
 *                            Required Libraries                               *
 *******************************************************************************/

#include "LCD.h"
#include "keypad.h"
#include "USART.h"
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

/*-----------------------------------------------------------------------------*/

typedef enum
{
	MainMenu , UnmatchedPW , OpenDoor , ChangePW , EmptyLoop
}Application_State;

typedef enum
{
	EmptyPW , IncorrectPW , CorrectPW
}Password_Results;

typedef enum
{
	Loop , SetPW , EnterPW , OpenningDoor , LockedMode
}UART_commands;

/********************************************************************************
 *                              Global Variables	                            *
 ********************************************************************************/

Application_State APP_nextState ;

UART_commands UART_nextState = Loop ;

Password_Results PW_Result = EmptyPW;

/********************************************************************************
 *                              Function Prototypes	                            *
 ********************************************************************************/

/*Description:
 * Get the password from user twice to set it inside the EEPROM */
uint8 setPassword(void);

/*Description:
 * Sends the password entries to Control ECU to check it it's matched
 * and get feedback of matching result*/
uint8 isPasswordMatched(const uint8 * a_firstEntry , const uint8 * a_secondEntry);

/*Description:
 * Receive password from user to compare it with set password in EEPROM
 * in order to proceed forward with the desired operation */
uint8 passwordEntry(void);

/*Description:
 * Gets the password received from user to send it to EEPROM
 * for comparison and get feedback from Control ECU
 * whether if the password in correct/incorrect */
void passwordState(const Password_Results * a_Result);

/*Description:
 * Displays the Main Menu options on LCD display
 * and gets input from user for the desired operation
 * whether to open the door or change the password*/
void mainMenu(void);

/*Description:
 * Displays the status of the door opening operation
 * on the LCD display and shows
 * whether it's unlocking , locking or stopped*/
void displayDoorStatus(void);

/*Description:
 * Activate Locked mode on the whole system and sends command to Control ECU
 * to also initiate locked mode */
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

	/*********************************************************************/

	/************************ Drivers Initializations *********************/

	UART_init(&s_UARTconfig); /*UART Init. with req. configuration*/

	_delay_ms(10); /*Allow time for transmission & initialization*/

	LCD_init(); /*LCD Init.*/

	_delay_ms(10); /*Allow time for transmission & initialization*/

	/*********************************************************************/

	LCD_displayStringRowColumn(0,2,"Welcome To ");
	LCD_displayStringRowColumn(1,1,"Final Project !");

	_delay_ms(1500); /*Display message for 1.5 seconds*/

	LCD_cleanScreen();

	LCD_moveCursor(0,0);

	/* If password are matched and saved in EEPROM ,
	 * nextStatus will be set to Main_menu
	 * OR
	 * will return UmatchedPW*/
	APP_nextState = setPassword();


	/***************************** Main Loop ****************************/

	while(1)
	{
		/*Checks the required application function*/
		switch(APP_nextState)
		{
		case MainMenu : /*Password set Success !*/

			/*Display Main Menu*/
			mainMenu();
			break;

		case UnmatchedPW : /*Failed password match  */

			LCD_cleanScreen();

			LCD_displayString("    Error !");

			LCD_displayStringRowColumn(1,0,"Unmatched pass");

			_delay_ms(1000); /*Display message for 1 second*/

			LCD_cleanScreen();

			APP_nextState = setPassword();

			break;

		case OpenDoor : /*Door unlocking is triggered , password entry is requested*/

			PW_Result = passwordEntry();

			if (CorrectPW == PW_Result) /*If password matches with EEPROM , proceed*/
			{

				passwordState(&PW_Result); /*To reset wrong password counter*/

				UART_nextState = OpenningDoor;

				/*Send command through UART to Control ECU to start door actions*/
				UART_sendByte(UART_nextState);

				_delay_ms(20); /*Allow time for transmission*/

				/*Synchronization*/
				/*If a feedback is received from Control ECU ,
				 * initiate TIMER1 counter and start displaying status*/
				if(OpenningDoor == UART_recieveByte())
				{
					LCD_cleanScreen();

					/*Set the function to be called by TIMER 1 ISR*/
					Timer1_setCallBack(&displayDoorStatus);

					/*Initiate TIMER with the required configuration*/
					Timer1_init(&s_Timer1Config);

					/*Send the application to empty loop until the operation is complete*/
					APP_nextState = EmptyLoop;

					/*To prevent the One second delay until the first Interrupt occurs*/

					/*Requires that the counter if conditions to be incremented by an extra ONE
					 * for correct timing adjustment*/
					/*displayDoorStatus();*/
				}


			}
			/*If password doesn't match with EEPROM ,
			 * allow user to try again if possible (check passwordState)*/
			else if(IncorrectPW == PW_Result)
			{
				passwordState(&PW_Result);
			}

			break;

		case ChangePW : /*Password changing is triggered , password entry is requested*/

			PW_Result = passwordEntry();

			if (CorrectPW == PW_Result) /*If password matches with EEPROM , proceed*/
			{
				passwordState(&PW_Result); /*To reset wrong password counter*/

				LCD_cleanScreen();
				LCD_displayStringRowColumn(0,2,"Choose New");
				LCD_displayStringRowColumn(1,3,"Password");
				_delay_ms(1000); /*Display message for 1 second*/
				LCD_cleanScreen();

				APP_nextState = setPassword(); /*Set the new password*/
			}
			/*If password doesn't match with EEPROM ,
			 * allow user to try again if possible (check passwordState)*/
			else if(IncorrectPW == PW_Result)
			{
				passwordState(&PW_Result);
			}
			break;

		case EmptyLoop:
			/*The target from the empty loop is to make the application
			 * do the lowest possible tasks to reduce CPU load
			 * until the Application status is updated
			 * & to prevent confliction between tasks*/
			break;
		}
	}
	/*********************************************************************/
}


/********************************************************************************
 *                              Functions Definitions                           *
 ********************************************************************************/

void mainMenu(void)
{
	uint8 menuSelect = 0; /*Local variable for menu selection*/


	LCD_cleanScreen();

	LCD_displayStringRowColumn(0,0,"+ : Open Door");
	LCD_displayStringRowColumn(1,0,"- : Change Pass");

	/*Polling on menu options INSIDE the function*/
	while(menuSelect != '+' && menuSelect != '-' )
	{
		menuSelect = KEYPAD_getPressedKey(); /*Get input from user through Keypad*/

		/*De-bounce delay */
		_delay_ms(100);
	}

	if('+' == menuSelect) /*Case '+' is chosen , trigger door opening state*/
	{
		menuSelect = 0; /*Reset selection*/
		APP_nextState = OpenDoor ;
	}

	else if ('-' == menuSelect) /*Case '-' is chosen , trigger password changing state*/
	{
		menuSelect = 0; /*Reset selection*/
		APP_nextState = ChangePW ;
	}

}

uint8 setPassword(void)
{
	/*2 Local array of password size to get the password digit in it*/
	uint8 firstPasswordEntry[PASSWORD_LENGTH] = {0} ;
	uint8 secondPasswordEntry[PASSWORD_LENGTH] = {0} ;

	uint8 digitCheck ;

	/*data type is signed to fit the condition check if decrement was applied ZERO*/
	sint8 passwordDigitCounter ;

	Application_State matchingResult ;

	LCD_displayString("Enter password :");

	LCD_moveCursor(1,0);

	/*First Password Entry*/
	/*Loop on the array to collect the required the password digits*/
	for(passwordDigitCounter = 0 ; passwordDigitCounter < PASSWORD_LENGTH ; passwordDigitCounter++)
	{
		/*Place the received input into another variable first for two reasons :
		 * 1 - To check if the received input is a numeric digit
		 * 2 - To allow a de-bounce delay  */
		digitCheck = KEYPAD_getPressedKey() ;

		/*De-Bounce delay*/
		_delay_ms(300);

		/*check if input is numeric digit*/
		if(digitCheck>=0 && digitCheck<=9)
		{
			/*Place the final value inside the relative array position*/
			firstPasswordEntry[passwordDigitCounter] = digitCheck;

			/*The following steps will print the the number entered and will
			 * be replaced shortly on LCD display with '*' character*/
			LCD_intgerToString(firstPasswordEntry[passwordDigitCounter]);

			_delay_ms(200);

			LCD_moveCursor(1,passwordDigitCounter);

			LCD_displayCharacter('*');
		}
		else
		{
			/*If the variable wasn't numeric , repeat the iteration once again*/
			passwordDigitCounter--;
		}
	}

	/*Once the entry is completed , confirm by
	 * pressing on '=' to proceed to next step*/
	while(KEYPAD_getPressedKey() != '=');

	LCD_cleanScreen();

	LCD_displayString("Re-Enter the ");

	LCD_displayStringRowColumn(1,0,"same pass: ");

	LCD_moveCursor(1,11);

	/*Second Password Entry*/
	/*Loop on the array to collect the required the password digits*/
	for(passwordDigitCounter = 0 ; passwordDigitCounter < PASSWORD_LENGTH ; passwordDigitCounter++)
	{
		/*Place the received input into another variable first for two reasons :
		 * 1 - To check if the received input is a numeric digit
		 * 2 - To allow a de-bounce delay  */
		digitCheck = KEYPAD_getPressedKey() ;

		/*De-Bounce delay*/
		_delay_ms(300);

		/*check if input is numeric digit*/
		if(digitCheck>=0 && digitCheck<=9)
		{
			/*Place the final value inside the relative array position*/
			secondPasswordEntry[passwordDigitCounter] = digitCheck;

			/*The following steps will print the the number entered and will
			 * be replaced shortly on LCD display with '*' character*/
			LCD_intgerToString(secondPasswordEntry[passwordDigitCounter]);

			_delay_ms(200);

			LCD_moveCursor(1,11+passwordDigitCounter);

			LCD_displayCharacter('*');
		}
		else
		{
			/*If the variable wasn't numeric , repeat the iteration once again*/
			passwordDigitCounter--;
		}
	}

	/*Once the entry is completed , confirm by
	 * pressing on '=' to proceed to next step*/
	while(KEYPAD_getPressedKey() != '=');

	/*Send the two arrays through to EEPROM via Control ECU */
	matchingResult = isPasswordMatched(firstPasswordEntry,secondPasswordEntry);

	/*Return the matching result confirming whether
	* the password was set or unmatched*/
	return matchingResult ;

}

uint8 isPasswordMatched(const uint8 * a_firstEntry , const uint8 * a_secondEntry)
{
	/*next Control ECU state is to set the PW if matched or return unmatched*/
	UART_nextState = SetPW;

	/*Send command to Control ECU via UART to go to set password state*/
	UART_sendByte(UART_nextState);

	/*Send first password Entry*/
	UART_sendData(a_firstEntry,PASSWORD_LENGTH);

	_delay_ms(100); /*Allow time for transmission*/

	/*Send second password Entry*/
	UART_sendData(a_secondEntry,PASSWORD_LENGTH);

	_delay_ms(100); /*Allow time for transmission*/

	/*Receives either password is matched & set OR need to be entered again*/
	return UART_recieveByte();

}

uint8 passwordEntry(void)
{
	/*Local array of password size to get the password digits in it*/
	uint8 passwordEntryArray[PASSWORD_LENGTH] = {0} ;

	/*data type is signed to fit the condition check if decrement was applied ZERO*/
	sint8 passwordDigitCounter ;
	uint8 digitCheck ;

	/*next Control ECU state is to get a password to check if correct or incorrect*/
	UART_nextState = EnterPW;

	LCD_cleanScreen();

	/*Send command to Control ECU via UART to go to enter password state*/
	UART_sendByte(UART_nextState);

	_delay_ms(10); /*Allow time for transmission*/

	LCD_displayString("Enter password :");

	LCD_moveCursor(1,0);

	/*Loop on the array to collect the required the password digits*/
	for(passwordDigitCounter = 0 ; passwordDigitCounter < PASSWORD_LENGTH ; passwordDigitCounter++)
	{
		/*Place the received input into another variable first for two reasons :
		 * 1 - To check if the received input is a numeric digit
		 * 2 - To allow a de-bounce delay  */
		digitCheck = KEYPAD_getPressedKey() ;

		/*De-Bounce delay*/
		_delay_ms(300);

		/*check if input is numeric digit*/
		if(digitCheck>=0 && digitCheck<=9)
		{
			/*Place the final value inside the relative array position*/
			passwordEntryArray[passwordDigitCounter] = digitCheck ;

			/*The following steps will print the the number entered and will
			 * be replaced shortly on LCD display with '*' character*/
			LCD_intgerToString(passwordEntryArray[passwordDigitCounter]);

			_delay_ms(200);

			LCD_moveCursor(1,passwordDigitCounter);

			LCD_displayCharacter('*');
		}
		else
		{
			/*If the variable wasn't numeric , repeat the iteration once again*/
			passwordDigitCounter--;
		}
	}

	/*Once the entry is completed , confirm by
	 * pressing on '=' to proceed to next step*/
	while(KEYPAD_getPressedKey() != '=');

	/*Send the password Entry*/
	UART_sendData(passwordEntryArray,PASSWORD_LENGTH);

	_delay_ms(100); /*Allow time for transmission*/

	/*Receive pw_match results to check whether correct / incorrect password*/
	return UART_recieveByte();

}

void passwordState(const Password_Results * a_Result)
{
	/*Static variable to collect the number of times the
	 * password entered was incorrect*/
	static uint8 wrongPasswordCounter = 0;

	/*If the maximum allowable entries of password are reached
	 * it will redirect to Locked mode !*/
	if(2 == wrongPasswordCounter && IncorrectPW == *a_Result )
	{
		wrongPasswordCounter=0;
		lockedMode();
	}

	/*If the password was incorrect , increment wrong password
	 * entries counter by one and display message on LCD */
	else if(IncorrectPW == *a_Result)
	{
		wrongPasswordCounter++;
		LCD_cleanScreen();

		LCD_displayString("Incorrect PW !");

		_delay_ms(500); /*display message for 0.5 second*/
	}

	/*If entered password was correct , reset wrong password entries counter*/
	else if (CorrectPW == *a_Result)
	{
		wrongPasswordCounter=0;
	}

	/*Reset password status back to empty*/
	PW_Result = EmptyPW;
}

void displayDoorStatus(void)
{
	/*Set the counter to static to increase lifetime through out
	 * the run time & set to volatile to disable compiler
	 * optimization on the variable*/
	static volatile uint8 doorTimerCounter = MOTOR_START_TIME;

	/*Display the main message that won't be altered at first iteration*/
	if(MOTOR_START_TIME==doorTimerCounter)
	LCD_displayStringRowColumn(0,0,"Door is ");

	/*Timer counter value is within unlocking mode*/
	if(doorTimerCounter < MOTOR_UNLOCKING_TIME)
	{
		if(MOTOR_START_TIME==doorTimerCounter)
		LCD_displayStringRowColumn(1,0,"Unlocking");


		/*A buffer animation on screen :D*/
		/*A number divisible by 3 , display three dots*/
		if(BIT_IS_SET(doorTimerCounter,0) && BIT_IS_SET(doorTimerCounter,1))
		{
			LCD_displayStringRowColumn(1,9,"...");
		}
		/*An even number , display two dots*/
		else if(BIT_IS_SET(doorTimerCounter,1) )
		{
			LCD_displayStringRowColumn(1,9,".. ");
		}
		/*An odd number , display one dot*/
		else if(BIT_IS_SET(doorTimerCounter,0))
		{
			LCD_displayStringRowColumn(1,9,".  ");
		}

		/*Increment timer by one , which means one second has passed */
		doorTimerCounter++;

	}

	/*Timer counter value is within stop mode*/
	else if (doorTimerCounter >=MOTOR_UNLOCKING_TIME && doorTimerCounter<MOTOR_STOPPING_TIME)
	{
		if(MOTOR_UNLOCKING_TIME==doorTimerCounter)
		LCD_displayStringRowColumn(1,0,"Unlocked !  ");

		/*Increment timer by one , which means one second has passed */
		doorTimerCounter++;
	}

	/*Timer counter value is within locking mode*/
	else if(doorTimerCounter >=MOTOR_STOPPING_TIME && doorTimerCounter  < (MOTOR_LOCKING_TIME))
	{
		if((MOTOR_STOPPING_TIME)==doorTimerCounter)
		LCD_displayStringRowColumn(1,0,"Locking");

		/*A buffer animation on screen :D*/
		/*A number divisible by 3 , display three dots*/
		if(BIT_IS_SET(doorTimerCounter,0) && BIT_IS_SET(doorTimerCounter,1))
		{
			LCD_displayStringRowColumn(1,7,"...");
		}
		/*An even number , display two dots*/
		else if(BIT_IS_SET(doorTimerCounter,1) )
		{
			LCD_displayStringRowColumn(1,7,".. ");
		}
		/*An odd number , display one dot*/
		else if(BIT_IS_SET(doorTimerCounter,0))
		{
			LCD_displayStringRowColumn(1,7,".  ");
		}

		/*Increment timer by one , which means one second has passed */
		doorTimerCounter++;
	}

	/*IF Timer counter reached the last required time ,
	 * Stop / DeInit. TIMER 1 , reset counter & return back to Main Menu*/
	else if(doorTimerCounter == MOTOR_LOCKING_TIME	)
	{
		Timer1_deInit();
		doorTimerCounter=0;
		APP_nextState = MainMenu ;
	}
}

void lockedMode(void)
{

	/*Set the next Control ECU state to be in locked mode*/
	UART_nextState = LockedMode;

	/*Send the UART command*/
	UART_sendByte(UART_nextState);

	_delay_ms(10); /*Allow time for transmission */

	LCD_cleanScreen();

	LCD_displayString("     Error !");

	LCD_displayStringRowColumn(1,0," You're Locked !");

	/*Loop on the previous screen till Control ECU sends a
	 * command feeding back it has exited locked mode*/
	while(Loop != UART_recieveByte());

	/*Return back to Main Menu*/
	APP_nextState = MainMenu ;
}

/**********************************************************************/
