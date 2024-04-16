 /******************************************************************************
 *
 * Module : LCD
 *
 * File Name: lcd.h
 *
 * Description : Header file of LCD driver
 *
 * Created on: Feb 10, 2024
 *
 * Author: Mohamed Tarek
 *
 * Modified by : Karim Amr
 *
******************************************************************************/

#ifndef LCD_H_
#define LCD_H_

#include "std_types.h"
/*******************************************************************************
 *                                    Definitions                               *
 *******************************************************************************/

#define LCD_DATA_BITS_MODE 4 /*Enter the Data bits mode*/
#define LCD_DATA_BITS_SAME_PORT /*Comment out if FALSE*/

#if((LCD_DATA_BITS_MODE != 4) && (LCD_DATA_BITS_MODE != 8))

#error "Number of Data bits should be equal to 4 or 8"

#endif

/* LCD HW Ports and Pins IDs */
#define LCD_RS_PORT_ID                 PORTA_ID
#define LCD_RS_PIN_ID                  PIN1_ID

#define LCD_E_PORT_ID                  PORTA_ID
#define LCD_E_PIN_ID                   PIN2_ID

#ifdef LCD_DATA_BITS_SAME_PORT
#define LCD_DATA_PORT_ID               PORTA_ID

#if (LCD_DATA_BITS_MODE == 4)

#define LCD_DB4_PIN_ID                 PIN3_ID
#define LCD_DB5_PIN_ID                 PIN4_ID
#define LCD_DB6_PIN_ID                 PIN5_ID
#define LCD_DB7_PIN_ID                 PIN6_ID
#endif
#endif

#ifndef LCD_DATA_BITS_SAME_PORT

/*Select the required data PORTS | Add extra if needed */
#define LCD_DATA_PORT_ID0               PORTC_ID
#define LCD_DATA_PORT_ID1               PORTC_ID

#if (LCD_DATA_BITS_MODE == 8)
#define LCD_DB0_PIN_ID                 PIN3_ID
#define LCD_DB1_PIN_ID                 PIN4_ID
#define LCD_DB2_PIN_ID                 PIN5_ID
#define LCD_DB3_PIN_ID                 PIN6_ID
#define LCD_DB4_PIN_ID                 PIN3_ID
#define LCD_DB5_PIN_ID                 PIN4_ID
#define LCD_DB6_PIN_ID                 PIN5_ID
#define LCD_DB7_PIN_ID                 PIN6_ID

#elif (LCD_DATA_BITS_MODE == 4)

#define LCD_DB4_PIN_ID                 PIN3_ID
#define LCD_DB5_PIN_ID                 PIN4_ID
#define LCD_DB6_PIN_ID                 PIN5_ID
#define LCD_DB7_PIN_ID                 PIN6_ID
#endif
#endif


/* LCD Commands */
#define LCD_CLEAR_COMMAND          		     0x01
#define LCD_GO_TO_HOME              	     0x02
#define LCD_MOVE_CURSOR_RIGHT       	     0x06
#define LCD_MOVE_CURSOR_LEFT	       	     0x04
#define LCD_TWO_LINES_EIGHT_BITS_MODE  	 	 0x38
#define LCD_TWO_LINES_FOUR_BITS_MODE  		 0x28
#define LCD_TWO_LINES_FOUR_BITS_MODE_INIT1   0x33
#define LCD_TWO_LINES_FOUR_BITS_MODE_INIT2   0x32
#define LCD_CURSOR_OFF                 		 0x0C
#define LCD_CURSOR_ON                	     0x0E
#define LCD_SET_CURSOR_LOCATION      	     0x80

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/* Initialize the LCD by :
 * 1. Setup the LCD pins directions by use the GPIO driver.
 * 2. Setup the LCD Data Mode 4-bits or 8-bits.
 */
void LCD_init(void);

/*Send a specific command ( check LCD data-sheet ) to the LCD*/
void LCD_sendCommand(uint8 command) ;

/*Send a specific character to be displayed on the LCD*/
void LCD_displayCharacter(uint8 command) ;

/*Send a String to be displayed on the LCD*/
void LCD_displayString(const char * string) ;

/*Move cursor to required row and column*/
void LCD_moveCursor(uint8 row , uint8 col);

/*Display string at a specific row and column */
void LCD_displayStringRowColumn(uint8 row , uint8 col,const char * string);

/*To clear LCD screen*/
void LCD_cleanScreen(void);

/*To display integer  numbers on LCD*/
void LCD_intgerToString(int data);

#endif /* LCD_H_ */
