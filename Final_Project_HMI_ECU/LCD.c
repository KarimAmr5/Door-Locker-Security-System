 /******************************************************************************
 *
 * Module: LCD
 *
 * File Name: lCD.c
 *
 *  Description: Source file for the LCD driver
 *
 * Created on: Feb 10, 2024
 *
 * Author: Mohamed Tarek
 *
 * Modified by : Karim Amr
 *
******************************************************************************/

#include "GPIO.h"
#include "LCD.h"
#include <util/delay.h>
#include "stdlib.h"
#include "common_macros.h" /* For GET_BIT Macro */

void LCD_init(void)
{
	GPIO_setupPinDirection(LCD_RS_PORT_ID,LCD_RS_PIN_ID,PIN_OUTPUT); /*RS pin = output*/
	GPIO_setupPinDirection(LCD_E_PORT_ID,LCD_E_PIN_ID,PIN_OUTPUT); /*Enable pin = output*/

	_delay_ms(20); /*delay greater than 15ms to give it time to initialize */

#ifdef LCD_DATA_BITS_SAME_PORT
#if (LCD_DATA_BITS_MODE==8)
	/*Set Data pins as output in case of 8-bits mode && all pins are on the same port*/
	GPIO_setupPortDirection(LCD_DATA_PORT_ID,PIN_OUTPUT);
	LCD_sendCommand(LCD_TWO_LINES_EIGHT_BITS_MODE);

#elif (LCD_DATA_BITS_MODE==4)
	GPIO_setupPinDirection(LCD_DATA_PORT_ID,LCD_DB4_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT_ID,LCD_DB5_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT_ID,LCD_DB6_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT_ID,LCD_DB7_PIN_ID,PIN_OUTPUT);

	LCD_sendCommand(LCD_TWO_LINES_FOUR_BITS_MODE_INIT1);
	LCD_sendCommand(LCD_TWO_LINES_FOUR_BITS_MODE_INIT2);

	LCD_sendCommand(LCD_TWO_LINES_FOUR_BITS_MODE);

#endif
#endif

#ifndef LCD_DATA_BITS_SAME_PORT
#if (LCD_DATA_BITS_MODE==8)
	/*Set Data pins as output in case of 8-bits mode && all pins are on the same port*/
	GPIO_setupPinDirection(LCD_DATA_PORT_ID0,LCD_DB0_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT_ID0,LCD_DB1_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT_ID0,LCD_DB2_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT_ID0,LCD_DB3_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT_ID0,LCD_DB4_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT_ID0,LCD_DB5_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT_ID0,LCD_DB6_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT_ID0,LCD_DB7_PIN_ID,PIN_OUTPUT);

	LCD_sendCommand(LCD_TWO_LINES_EIGHT_BITS_MODE);
#elif (LCD_DATA_BITS_MODE==4)
	GPIO_setupPinDirection(LCD_DATA_PORT_ID0,LCD_DB4_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT_ID0,LCD_DB5_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT_ID0,LCD_DB6_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT_ID0,LCD_DB7_PIN_ID,PIN_OUTPUT);

	LCD_sendCommand(LCD_TWO_LINES_FOUR_BITS_MODE_INIT1);
	LCD_sendCommand(LCD_TWO_LINES_FOUR_BITS_MODE_INIT2);

	LCD_sendCommand(LCD_TWO_LINES_FOUR_BITS_MODE);
#endif
#endif

	LCD_sendCommand(LCD_CURSOR_OFF);
	LCD_sendCommand(LCD_CLEAR_COMMAND);
}

/*Description :
 * Function that send required command to the LCD*/
/*Check Data-Sheet for sending command sequence*/
void LCD_sendCommand(uint8 command)
{
	GPIO_writePin(LCD_RS_PORT_ID,LCD_RS_PIN_ID,LOGIC_LOW);
	_delay_ms(1);
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_HIGH);
	_delay_ms(1);


#ifdef LCD_DATA_BITS_SAME_PORT
#if (LCD_DATA_BITS_MODE==8)
	GPIO_writePort(LCD_DATA_PORT_ID,command);
	_delay_ms(1);
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW);
	_delay_ms(1);
#elif (LCD_DATA_BITS_MODE==4)
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB4_PIN_ID,GET_BIT(command,4));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB5_PIN_ID,GET_BIT(command,5));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB6_PIN_ID,GET_BIT(command,6));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB7_PIN_ID,GET_BIT(command,7));

	_delay_ms(1);
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW);
	_delay_ms(1);
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_HIGH);
	_delay_ms(1);

	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB4_PIN_ID,GET_BIT(command,0));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB5_PIN_ID,GET_BIT(command,1));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB6_PIN_ID,GET_BIT(command,2));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB7_PIN_ID,GET_BIT(command,3));

	_delay_ms(1);
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW);
	_delay_ms(1);

#endif
#endif

/*Configure the PORT & PIN ID of the LCD connection accordingly*/
#ifndef LCD_DATA_BITS_SAME_PORT
#if (LCD_DATA_BITS_MODE==8)
		GPIO_writePin(LCD_DATA_PORT_ID0,LCD_DB0_PIN_ID,GET_BIT(command,0));
		GPIO_writePin(LCD_DATA_PORT_ID0,LCD_DB1_PIN_ID,GET_BIT(command,1));
		GPIO_writePin(LCD_DATA_PORT_ID0,LCD_DB2_PIN_ID,GET_BIT(command,2));
		GPIO_writePin(LCD_DATA_PORT_ID0,LCD_DB3_PIN_ID,GET_BIT(command,3));
		GPIO_writePin(LCD_DATA_PORT_ID1,LCD_DB4_PIN_ID,GET_BIT(command,4));
		GPIO_writePin(LCD_DATA_PORT_ID1,LCD_DB5_PIN_ID,GET_BIT(command,5));
		GPIO_writePin(LCD_DATA_PORT_ID1,LCD_DB6_PIN_ID,GET_BIT(command,6));
		GPIO_writePin(LCD_DATA_PORT_ID1,LCD_DB7_PIN_ID,GET_BIT(command,7));


		_delay_ms(1);
		GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW);
		_delay_ms(1);

#elif (LCD_DATA_BITS_MODE==4)
	GPIO_writePin(LCD_DATA_PORT_ID0,LCD_DB4_PIN_ID,GET_BIT(command,4));
	GPIO_writePin(LCD_DATA_PORT_ID0,LCD_DB5_PIN_ID,GET_BIT(command,5));
	GPIO_writePin(LCD_DATA_PORT_ID1,LCD_DB6_PIN_ID,GET_BIT(command,6));
	GPIO_writePin(LCD_DATA_PORT_ID1,LCD_DB7_PIN_ID,GET_BIT(command,7));

	_delay_ms(1);
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW);
	_delay_ms(1);
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_HIGH);
	_delay_ms(1);

	GPIO_writePin(LCD_DATA_PORT_ID0,LCD_DB4_PIN_ID,GET_BIT(command,0));
	GPIO_writePin(LCD_DATA_PORT_ID0,LCD_DB5_PIN_ID,GET_BIT(command,1));
	GPIO_writePin(LCD_DATA_PORT_ID1,LCD_DB6_PIN_ID,GET_BIT(command,2));
	GPIO_writePin(LCD_DATA_PORT_ID1,LCD_DB7_PIN_ID,GET_BIT(command,3));

	_delay_ms(1);
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW);
	_delay_ms(1);

#endif
#endif


}

void LCD_displayCharacter(uint8 data)
{
	GPIO_writePin(LCD_RS_PORT_ID,LCD_RS_PIN_ID,LOGIC_HIGH);
	_delay_ms(1);
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_HIGH);
	_delay_ms(1);


#ifdef LCD_DATA_BITS_SAME_PORT
#if (LCD_DATA_BITS_MODE==8)
	GPIO_writePort(LCD_DATA_PORT_ID,data);
	_delay_ms(1);
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW);
	_delay_ms(1);
#elif (LCD_DATA_BITS_MODE==4)
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB4_PIN_ID,GET_BIT(data,4));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB5_PIN_ID,GET_BIT(data,5));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB6_PIN_ID,GET_BIT(data,6));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB7_PIN_ID,GET_BIT(data,7));

	_delay_ms(1);
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW);
	_delay_ms(1);
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_HIGH);
	_delay_ms(1);

	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB4_PIN_ID,GET_BIT(data,0));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB5_PIN_ID,GET_BIT(data,1));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB6_PIN_ID,GET_BIT(data,2));
	GPIO_writePin(LCD_DATA_PORT_ID,LCD_DB7_PIN_ID,GET_BIT(data,3));

	_delay_ms(1);
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW);
	_delay_ms(1);

#endif
#endif

/*Configure the PORT & PIN ID of the LCD connection accordingly*/
#ifndef LCD_DATA_BITS_SAME_PORT
#if (LCD_DATA_BITS_MODE==8)
		GPIO_writePin(LCD_DATA_PORT_ID0,LCD_DB0_PIN_ID,GET_BIT(data,0));
		GPIO_writePin(LCD_DATA_PORT_ID0,LCD_DB1_PIN_ID,GET_BIT(data,1));
		GPIO_writePin(LCD_DATA_PORT_ID0,LCD_DB2_PIN_ID,GET_BIT(data,2));
		GPIO_writePin(LCD_DATA_PORT_ID0,LCD_DB3_PIN_ID,GET_BIT(data,3));
		GPIO_writePin(LCD_DATA_PORT_ID1,LCD_DB4_PIN_ID,GET_BIT(data,4));
		GPIO_writePin(LCD_DATA_PORT_ID1,LCD_DB5_PIN_ID,GET_BIT(data,5));
		GPIO_writePin(LCD_DATA_PORT_ID1,LCD_DB6_PIN_ID,GET_BIT(data,6));
		GPIO_writePin(LCD_DATA_PORT_ID1,LCD_DB7_PIN_ID,GET_BIT(data,7));


		_delay_ms(1);
		GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW);
		_delay_ms(1);

#elif (LCD_DATA_BITS_MODE==4)
	GPIO_writePin(LCD_DATA_PORT_ID0,LCD_DB4_PIN_ID,GET_BIT(data,4));
	GPIO_writePin(LCD_DATA_PORT_ID0,LCD_DB5_PIN_ID,GET_BIT(data,5));
	GPIO_writePin(LCD_DATA_PORT_ID1,LCD_DB6_PIN_ID,GET_BIT(data,6));
	GPIO_writePin(LCD_DATA_PORT_ID1,LCD_DB7_PIN_ID,GET_BIT(data,7));

	_delay_ms(1);
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW);
	_delay_ms(1);
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_HIGH);
	_delay_ms(1);

	GPIO_writePin(LCD_DATA_PORT_ID0,LCD_DB4_PIN_ID,GET_BIT(data,0));
	GPIO_writePin(LCD_DATA_PORT_ID0,LCD_DB5_PIN_ID,GET_BIT(data,1));
	GPIO_writePin(LCD_DATA_PORT_ID1,LCD_DB6_PIN_ID,GET_BIT(data,2));
	GPIO_writePin(LCD_DATA_PORT_ID1,LCD_DB7_PIN_ID,GET_BIT(data,3));

	_delay_ms(1);
	GPIO_writePin(LCD_E_PORT_ID,LCD_E_PIN_ID,LOGIC_LOW);
	_delay_ms(1);

#endif
#endif



}

void LCD_displayString(const char * string) /* Pointer size is equal to access size */
{
	uint8 s_stringLooping;

	for(s_stringLooping=0 ; string[s_stringLooping]!='\0' ; s_stringLooping++)
	{
		LCD_displayCharacter(string[s_stringLooping]);

		/*Switch to move cursor on mapped LCD memory only */
		LCD_sendCommand(LCD_MOVE_CURSOR_RIGHT);
	}

}

void LCD_moveCursor(uint8 row,uint8 col)
{
	uint8 lcd_memory_address ;

	switch(row)
	{
		case 0:
			lcd_memory_address = col;
			break;
		case 1:
			lcd_memory_address = col+0x40;
			break;
		case 2:
			lcd_memory_address = col+0x10;
			break;
		case 3:
			lcd_memory_address = col+0x50;
			break;
	}

	/*Line description :
	 * We found out the max. memory location has a binary of 0101 1111
	 * which means that the last bit will always be ZERO , hence it is set to ONE
	 * when we want to the change the cursor location followed by the required
	 * location of cursor to be set  */
	LCD_sendCommand(LCD_SET_CURSOR_LOCATION |lcd_memory_address);
}

void LCD_displayStringRowColumn(uint8 row , uint8 col,const char * string)
{
	LCD_moveCursor(row,col);
	LCD_displayString(string);
}

void LCD_cleanScreen(void)
{
	LCD_sendCommand(LCD_CLEAR_COMMAND);
}

void LCD_intgerToString(int data)
{
	char buffer[16];

	/*Integer to ASCII conversion into an array ,
	 * input : input (integer) , array (string) , base of numbers (10)
	 * then is send to string display */
	itoa(data,buffer,10);
	LCD_displayString(buffer);
}
