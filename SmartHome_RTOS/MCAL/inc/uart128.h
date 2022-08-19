/*
 * uart.h
 *
 * Created: 6/3/2020 2:31:30 PM
 *  Author: MahmoudH
 */ 


#ifndef UART_H_
#define UART_H_



typedef enum{
	UART_0,UART_1
}UART_Type;

void	Uart_Init(UART_Type uartNum, u32 u32Baud);
void	Uart_SendByte(UART_Type uartNum, u8 u8Data);
u8		Uart_ReceiveByte(UART_Type uartNum);

void	Uart_SendStr(UART_Type uartNum, u8* pu8Str);
//Bool	Uart_ReceiveByte_Unblock(UART_Type uartNum, INT8U* pu8Data);

#endif /* UART_H_ */