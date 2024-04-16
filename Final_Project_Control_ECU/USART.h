 /******************************************************************************
 *
 * Module : USART
 *
 * File Name: UART.h
 *
 * Description : Header file of UART driver
 *
 * Created on: Feb 28, 2024
 *
 * Author: Mohamed Tarek
 *
 * Modified by : Karim Amr
 *
******************************************************************************/

#ifndef USART_H_
#define USART_H_

#include "std_types.h"

/*******************************************************************************
 *                                    Definitions                               *
 *******************************************************************************/

typedef enum {
	FiveBit , SixBit , SevenBit , EightBit
}UART_BitData ;

typedef enum {
	DisabledParity , EvenParity=2 , OddParity
}UART_Parity ;

typedef enum {
	OneStopBit , TwoStopBits
}UART_StopBit ;

typedef uint32 UART_BaudRate ;

typedef struct{
 UART_BitData bit_data;
 UART_Parity parity;
 UART_StopBit stop_bit;
 UART_BaudRate baud_rate;
}UART_ConfigType;

/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/* Description
 * Initialize UART by :
 * 1. Selecting clock mode ( Asynchronous / Synchronous) (Static to Asynchronous Operation)
 * 2. Selecting parity mode ( Disabled  , Even , Odd )
 * 3. Selecting number of stop bits ( One_Bit , Two_Bits )
 * 4. Selecting Baud rate
 * 5. Setting data frame bits ( 5 , 6 ,  7 , 8 )
 */
void UART_init(const UART_ConfigType * Config_Ptr);

/* Description
 * return the value of the received byte through UART frame
 */
uint8 UART_recieveByte(void);


/* Description
 * Send 8-bit data through UART frame
 * uses the UDRE flag check in order to send data without overwriting of prev. unsent data
 */
void UART_sendByte(uint8 data);

/* Description
 * return the value of the received string through UART frame
 * into an string ( pointer to global string )
 * & replace the '#' that indicates the end of the string
 * with '\0' to set the string Null
 */
void UART_recieveString(uint8 *Str);

/* Description
 * Send array of string through UART frame
 * until the '#' character is detected
 */
void UART_sendString(const char *Str);



/* Description
 * Send array of data through UART frame
 * until the size of array is reached
 */
void UART_sendData(const uint8 *Data, uint8 dataSize);


/* Description
 * return the value of the received array through UART frame
 * into an array ( pointer to global array )
 * until size of array is reached
 */
void UART_recieveData(uint8 * Data , uint8 dataSize);



#endif /* USART_H_ */
