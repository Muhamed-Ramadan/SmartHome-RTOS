/*
 * MQTT.c
 *
 * Created: 8/29/2021 1:10:59 PM
 *  Author: MahmoudH
 */ 
#include "dataTypes.h"
#include "uart128.h"
#include "MQTT.h"
#include <string.h>

u16 PID = 1;
u8 proName[] = "MQTT";
void sendUTFStr(u8* str){
	Uart_SendByte(UART_0,0x00); // MSB = 0 because packet len < 256
	Uart_SendByte(UART_0,strlen((char*)str));
	Uart_SendStr(UART_0, str);
}
void MQTT_Connect(u8* id){
	u8 RL = (2 + strlen((char*)proName)) + 1 + 1 + 2 + ( 2 + strlen((char*)id));
	Uart_SendByte(UART_0,0x10);
	Uart_SendByte(UART_0,RL);
	sendUTFStr(proName);
	Uart_SendByte(UART_0,0x04);
	Uart_SendByte(UART_0,0x02);
	Uart_SendByte(UART_0,0xFF);
	Uart_SendByte(UART_0,0xFF);
	sendUTFStr(id);
}
void MQTT_Publish(u8* topic, u8* msgPtr, u8 msgLen){
	u8 RL = (2 + strlen((char*)topic)) + msgLen;
	u8 ind = 0;
	Uart_SendByte(UART_0,0x30);
	Uart_SendByte(UART_0,RL);
	sendUTFStr(topic);
	for (ind = 0; ind < msgLen; ind++)
	{
		Uart_SendByte(UART_0,msgPtr[ind]);
	}
}
void MQTT_Subscribe(u8* topic){
	u8 RL = 2 + (2 + strlen((char*)topic)) + 1;
	Uart_SendByte(UART_0,0x82);
	Uart_SendByte(UART_0,RL);
	
	Uart_SendByte(UART_0,(u8)(PID>>8)); // MSB
	Uart_SendByte(UART_0,(u8)(PID));
	PID++;
	
	sendUTFStr(topic);	
	Uart_SendByte(UART_0,0x00);
}