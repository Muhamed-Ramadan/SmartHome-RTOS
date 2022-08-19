/*
 * LCD.h
 *
 * Created: 8/19/2021 5:30:31 PM
 *  Author: Mahmoud-PCU
 */ 


#ifndef LCD_H_
#define LCD_H_


			/*	LCD PINS LOCATION	*/
#define data_pins (0xF0)
#define ctr_pin   (0x0C)
			/*	LCD PORTS	*/
#define data_port PORTA
#define ctr_port  PORTA
#define data_ddr  DDRA
#define ctr_ddr   DDRA
			/* LCD CONTROL PINS */
#define LCD_RS	  PAU2   //0->to select instruction mode  1->to select data mode
#define LCD_E	  PAU3  //
			/* COMMAND LIST */
#define CMD_CLR			0X01
#define CMD_RET			0X02
#define CMD_CUR_INC		0X06	/*CURSER DIRECTION LEFT TO RIGHT */ 
#define CMD_CUR_DEC		0X04	/*CURSER DIRECTION RIGHT TO LEFT */ 
#define CMD_CUR_SIT		0X0C	/* DISPLAY (ON) CURSER (ON  &  BLINKING)*/
#define CMD_SHIFT		0X10	/*SHIFTING IS DISABLED*/
#define CMD_FUN_SET		0X28	/*SET AS (4_BITS,2_LINES,5X8 CHARACTER) MODE*/
#define CGRAM_ADD		0X40	/*BASE ADDRESS OF CGRAM*/
#define DDRAM_ADD		0X80	/*BASE ADDRESS OF DDRAM*/



void LCD_init(void);
void SendToLCD(u8 data);
void LCD_send_cmd(u8 command);
void LCD_send_data(u8 data);
void LCD_latch(void);
void LCD_CLEAR (void);
void disp_char(u8 letter);
void disp_str(u8* str);
void LCD_GOTXY(u8 row,u8 col);
void disp_charXY(u8 row,u8 col,u8 letter);
void disp_strXY(u8 row,u8 col,u8* str);
void disp_int(u32 intvalue);
void disp_intXY(u8 row,u8 col,u32 intvalue);
void LCD_ret_Home(void);
#endif /* LCD_H_ */