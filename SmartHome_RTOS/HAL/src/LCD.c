/*
 * LCD.c
 *
 * Created: 8/19/2021 5:29:51 PM
 *  Author: Mahmoud-PC
 */ 
#define F_CPU	8000000UL
#include <util/delay.h>
#include "dataTypes.h"
#include "registers.h"
#include "DIO.h"
#include "LCD.h"
#include <stdio.h>



/*clear must be the last command*/
void LCD_init(void)
{
	pinsDirection(&data_ddr, data_pins, OUTPUT);
	pinsDirection(&ctr_ddr, ctr_pin, OUTPUT);
	_delay_ms(40);
	LCD_ret_Home();
	LCD_send_cmd(CMD_CUR_INC);
	LCD_send_cmd(CMD_CUR_SIT);
	LCD_send_cmd(CMD_SHIFT);
	LCD_send_cmd(CMD_FUN_SET);
	LCD_CLEAR();
}

void LCD_CLEAR(void)
{
	LCD_send_cmd(CMD_CLR);
	_delay_ms(2);
}
void LCD_ret_Home(void)
{
	LCD_send_cmd(CMD_RET);
	_delay_ms(2);
}
void SendToLCD(u8 data)
{
	writePins(&PORTA,data_pins,LOW);
	writePins(&PORTA,(data&data_pins),HIGH);
	LCD_latch();
	writePins(&PORTA,data_pins,LOW);
	writePins(&PORTA,((data<<4)&data_pins),HIGH);
	LCD_latch();
	_delay_us(50);
}

void LCD_send_cmd(u8 command)
{
	writePin(LCD_RS,LOW);
	SendToLCD(command);
}

void LCD_send_data(u8 data)
{
	writePin(LCD_RS,HIGH);
	SendToLCD(data);
}

void LCD_latch(void)
{
	writePin(LCD_E,HIGH);
	_delay_us(50);
	writePin(LCD_E,LOW);
	_delay_us(50);
}
void disp_char(u8 letter)
{
	LCD_send_data(letter);
}

void disp_charXY(u8 row,u8 col,u8 letter)
{
	LCD_GOTXY(row,col);
	disp_char(letter);	
}
void disp_str(u8* str)
{
	u8 indx=0;
	while(str[indx]!='\0')
	{
		disp_char(indx[str]);
		indx++;
	}
}
void LCD_GOTXY(u8 row,u8 col)
{
	if (row==1)
	{
		LCD_send_cmd(DDRAM_ADD+0x00+col-1);
	}
	else if(row==2)
	{
		LCD_send_cmd(0xC0+col-1);	
	}
	else if(row==3)
	{
		LCD_send_cmd(0x90+col-1);
	}
	else if(row==4)
	{
		LCD_send_cmd(0xD0+col-1);
	}
	else
	{
		
	}
	
}

void disp_strXY(u8 row,u8 col,u8* str)
{
	LCD_GOTXY(row,col);
	disp_str(str);
}


void disp_int(u32 intvalue)
{
	u8 string[11]={0};
		sprintf(string,"%u",intvalue);
		disp_str(string);
}
void disp_intXY(u8 row,u8 col,u32 intvalue)
{
	LCD_GOTXY(row,col);
	disp_int(intvalue);
}



