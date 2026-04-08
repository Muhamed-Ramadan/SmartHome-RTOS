/*
* uart.c
*
* Created: 6/3/2020 2:31:41 PM
*  Author: MahmoudH
*/



#ifndef F_CPU
#define F_CPU	8000000UL
#endif

#include	"avr/io.h"
#include	"dataTypes.h"
#include	"bitMath.h"
#include	"util/delay.h"
#include "avr/interrupt.h"
#include "uart128.h"


void	Uart_Init(UART_Type uartNum, u32 u32Baud){
	u16 ubrr = 0;
	ubrr = ((F_CPU)/(16*u32Baud)) - 1 ;
	
	switch (uartNum)
	{
		case UART_0:
		/* Set baud rate */
		UBRR0H = (u8)(ubrr>>8);
		UBRR0L = (u8)ubrr;
		/* Enable receiver and transmitter */
		UCSR0B = (1<<RXEN)|(1<<TXEN);
		/* Set frame format: 8data, 1stop bit */
		UCSR0C = (3<<UCSZ0);
		break;
		case UART_1:
		/* Set baud rate */
		UBRR1H = (u8)(ubrr>>8);
		UBRR1L = (u8)ubrr;
		/* Enable receiver and transmitter */
		UCSR1B = (1<<RXEN)|(1<<TXEN);
		/* Set frame format: 8data, 1stop bit */
		UCSR1C = (3<<UCSZ0);
		break;
	}
}
void	Uart_SendByte(UART_Type uartNum, u8 u8Data){
	switch (uartNum)
	{
		case UART_0:
		/* Wait for empty transmit buffer */
		while ( !( UCSR0A & (1<<UDRE)) );
		/* Put data into buffer, sends the data */
		UDR0 = u8Data;
		break;
		case UART_1:
		/* Wait for empty transmit buffer */
		while ( !( UCSR1A & (1<<UDRE)) );
		/* Put data into buffer, sends the data */
		UDR1 = u8Data;
		break;
	}
}
u8	Uart_ReceiveByte(UART_Type uartNum){
	u8 u8Data = 0;
	switch (uartNum)
	{
		case UART_0:
		/* Wait for empty transmit buffer */
		while ( !( UCSR0A & (1<<RXC)) );
		/* Put data into buffer, sends the data */
		u8Data = UDR0;
		break;
		case UART_1:
		/* Wait for empty transmit buffer */
		while ( !( UCSR1A & (1<<RXC)) );
		/* Put data into buffer, sends the data */
		u8Data = UDR1;
		break;
	}
	return u8Data;
}

void	Uart_SendStr(UART_Type uartNum, u8* pu8Str){
	u32 ind = 0;
	while(pu8Str[ind] != 0){
		Uart_SendByte(uartNum,pu8Str[ind]);
		ind++;
	}
}
BOOL	Uart_ReceiveByte_Unblock(UART_Type uartNum, u8* pu8Data){
	
	BOOL result = FALSE;
	switch (uartNum)
	{
		case UART_0:
			if(UCSR0A & (1<<RXC)){
				result = TRUE;
				(*pu8Data) = UDR0;
			}
		break;
		case UART_1:
			if(UCSR1A & (1<<RXC)){
				result = TRUE;
				(*pu8Data) = UDR1;
			}
		break;
	}
	return result;
}