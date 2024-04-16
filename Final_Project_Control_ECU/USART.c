 /******************************************************************************
 *
 * Module : USART
 *
 * File Name: UART.h
 *
 * Description : Source file of UART driver
 *
 * Created on: Feb 28, 2024
 *
 * Author: Mohamed Tarek
 *
 * Modified by : Karim Amr
 *
******************************************************************************/

/*******************************************************************************
 *                            Required Libraries                               *
 *******************************************************************************/

#include "USART.h"
#include "avr/io.h" /* To use the UART Registers */
#include "common_macros.h" /* To use the macros like SET_BIT */



/********************************************************************************
 *                              Functions Definitions                           *
 ********************************************************************************/

/* Description
 * Initialize UART by :
 * 1. Selecting clock mode ( Asynchronous / Synchronous) (Static to Asynchronous Operation for Final Project)
 * 2. Selecting parity mode ( Disabled  , Even , Odd )
 * 3. Selecting number of stop bits ( One_Bit , Two_Bits )
 * 4. Selecting Baud rate
 * 5. Setting data frame bits ( 5 , 6 ,  7 , 8 )
 */
void UART_init(const UART_ConfigType * Config_Ptr)
{
	uint16 Baud_rate = 0 ; /*Variable that holds the result of the Baud calculations */

	UCSRA = (1<<U2X); /*For double transmission speed*/

   /*************************************************
	***************** UCSRB Settings ****************
	*  RXCIE = 0 Disable RX complete interrupt
	*  TXCIE = 0 Disable TX complete interrupt
	*  UDRIE = 0 Disable Data Register Empty interrupt
	*  RXEN  = 1 Enable Receiver
	*  TXEN  = 1 Enable Transmitter
	*  *Following are settings for 9-bit mode between two AVR's
	*  UCSZ2 = 0 Character size
	*  RXB8  = 0 Receiver data bit 8
	*  TXB8  = 0 Transmitter data bit 8
	*************************************************/

	UCSRB = (1<<TXEN) | (1<<RXEN);

	/*************************************************
	***************** UCSRC Settings ****************
	*  URSEL 	= 1 		  Enable register select (Disable after configuring UCSRC)
	*  UCPOL 	= 0 		  Rising XCK edge
	*  UMSEL 	= 0 		  Asynchronous Operation
	*  UCSZ2:0  = Config_Ptr  Select Number of data bits
	*  UPM1:0   = Config_Ptr  Select Parity check
	*  USBS  	= Config_Ptr  Select number of stop bits
	*************************************************/
	SET_BIT(UCSRC,URSEL);

	UCSRC = (UCSRC & ~(0x06)) | (((Config_Ptr->bit_data)<<UCSZ0) & 0x06) ;

	UCSRC = (UCSRC & ~(0x30)) | (((Config_Ptr->parity)<<UPM0)    & 0x30) ;

	UCSRC = (UCSRC & ~(0x08)) | (((Config_Ptr->stop_bit)<<USBS)  & 0x08) ;

	CLEAR_BIT(UCSRC,UMSEL);

	/*************************************************
	***************** UBRRL Settings ****************
	*	UBRR formula = ( ( F_CPU / (8*BAUD_rate) ) -1 )
	*************************************************/
	Baud_rate = (uint16) (  (F_CPU / (Config_Ptr->baud_rate *8UL) )  - 1);

	/* Sets the the Last 8-bits of the Baud_rate IF available
	 * by shifting right with 8 into UBRRH , then sets the First
	 * 8-bits into UBRRL */
	UBRRH =  Baud_rate >> 8  ;
	UBRRL =  Baud_rate ;
}


/* Description
 * return the value of the received byte through UART frame
 */
uint8 UART_recieveByte(void)
{
	/*Pooling Technique
	 * checks the USART receive bit flag
	 * if it's ZERO , the receive buffer is still empty OR
	 * has no unread data */
	while(BIT_IS_CLEAR(UCSRA,RXC));

	/*When flag is set to ONE , the receive buffer has data that is unread
	 * & return the Data register */
	return UDR;
}


/* Description
 * Send 8-bit data through UART frame
 * uses the UDRE flag check in order to send data without overwriting of prev. unsent data
 */
void UART_sendByte(uint8 data) /*UDRE flag pooling method*/
{
	/*The loop checks on the empty data register flag (UDRE)
	 * while the flag is ZERO , the data buffer is still contains data
	 * that is to be sent and not ready to receive new one*/
	while(BIT_IS_CLEAR(UCSRA,UDRE));

	/*Once the flag is set to ONE , the loop exits and sends the data byte
	 * to the buffer for transmitting */
	UDR = data ;


/*	The Transmit complete flag pooling method (TXC)
	Disadvantage : Uses more clock cycles

	sends the data to the USART data register
	UDR = data ;

	* Once the transmitting complete flag is complete , the loop exits
	while(BIT_IS_CLEAR(UCSRA,TXC));

	* set the bit of TXC in order to clear the flag
	SET_BIT(UCSRA,TXC);*/


}


/* Description
 * return the value of the received string through UART frame
 * into an string ( pointer to global string )
 * & replace the '#' that indicates the end of the string
 * with '\0' to set the string Null
 */
void UART_recieveString(uint8 *Str)
{
	/*Variable to loop on the string*/
	uint8 bufferBit = 0 ;

	/*Do-While loop , takes the first byte of the array and then
	 * loop till the string receives the '#' character*/
	do
	{
		Str[bufferBit] = UART_recieveByte;
		bufferBit++;

	}while(UART_recieveByte != '#');

	/*Return back to the '#' character and replaces it with null '\0' */
	bufferBit--;
	Str[bufferBit] = '\0';

}

/* Description
 * Send array of string through UART frame
 * until the '#' character is detected
 */
void UART_sendString(const char *Str)
{
	/*Variable to loop on the string*/
	uint8 bufferBit = 0 ;

	/*Loop on the string and send Byte by Byte*/
	while(Str[bufferBit] != '\0')
	{
		UART_sendByte(Str[bufferBit]);
		bufferBit++;
	}

}


/* Description :
 * Sends array of data through UART frame
 * until size of array is reached
 */
void UART_sendData(const uint8 * Data, uint8 dataSize)
{
	/*Variable to loop on the data array*/
	uint8 bufferBit ;

	/*Loop on the array and send Byte by Byte*/
	for(bufferBit = 0 ; bufferBit < dataSize ; bufferBit++ )
	{
		UART_sendByte(Data[bufferBit]);
	}
}

/* Description
 * return the value of the received array through UART frame
 * into an array ( pointer to global array )
 * until size of array is reached
 */
void UART_recieveData(uint8 * Data , uint8 dataSize)
{
	/*Variable to loop on the data array*/
	uint8 bufferBit ;

	/*Loop on the array and send Byte by Byte*/
	for(bufferBit = 0 ; bufferBit < dataSize ; bufferBit++ )
	{
		Data[bufferBit] = UART_recieveByte();
	}
}
