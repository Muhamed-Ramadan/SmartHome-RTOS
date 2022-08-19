/*
* SmartHome.c
*
* Created: 01/09/2021 08:49:50 
*  Author: Administrator
*/
#define F_CPU	8000000UL
#include <util/delay.h>
#include "dataTypes.h"
#include "registers.h"
#include "DIO.h"
#include "KEYPAD.h"
#include "LCD.h"
#include "ADC.h"
#include "DHT.h"
#include "bitMath.h"
#include "Proj_Cof.h"
#include "SmartHome.h"
#include "string.h"
#include "MQTT.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "uart128.h"





/*RTOS Services*/
xSemaphoreHandle	bsFireEvent				=	NULL;
xSemaphoreHandle	bsTempEvent				=	NULL;   

xSemaphoreHandle	IrrigationEvent			=	NULL;
xSemaphoreHandle	LightsEvent				=	NULL;


xSemaphoreHandle	bsPassEntered			=	NULL;
xSemaphoreHandle	bsEnterAgain			=	NULL;
xSemaphoreHandle	bsPassEvent				=	NULL;
xQueueHandle		mqPassword				=	NULL;

static u8 oldpassword[]="1234";
static u8 trials;

extern	void (*ptrINT)(void);
extern u8 temp2;
extern	u8 tempstr[10];
static u8 flag=0;



/***************** fire alarm tasks ***************/
void T_FireDetect(void *pvinitData)
{
	u8 msg[]="home on fire";
	while (1)
	{
		if (readPin(FLAME_SENSOR))
		{
			MQTT_Publish((u8*)"G/fire",msg,strlen((char*)msg));
			xSemaphoreGive(bsFireEvent);
		}
		vTaskDelay(300);
	}		
}
void T_FireAlarmAction(void *pvinitData)
{
	
	while (1)
	{
		if (xSemaphoreTake(bsFireEvent,100))
		{
			
			
			writePin(FIREPUMP,HIGH);
			writePin(BUZZER,HIGH);
		}
		else
		{
			writePin(FIREPUMP,LOW);
			writePin(BUZZER,LOW);
		}
		vTaskDelay(300);
	}
}


/***************** temp control tasks ***************/
void T_TempAction(void *pvinitData)
{
	while(1)
	{
		if(xSemaphoreTake(bsTempEvent,3000))
		{
			writePin(FAN,HIGH);
		}
		else
		{
			writePin(FAN,LOW);
		}
	}
}
void T_TempRead	 (void *pvinitData)
{
	u8 temp[15];
	u8 hum[15];
	u8 str0[]="Temp High & Fan ON";
	while(1)
	{
		disp_strXY(3,1,(u8*)"Humid=     %RH");
		disp_strXY(4,1,(u8*)"temp=      Deg.c");
		DHT_Represent(hum,temp);
		disp_strXY(3,7,hum);
		disp_strXY(4,6,temp);
		if((strcmp((char*)temp,"25")>0))
		{
			MQTT_Publish((u8*)"G/temp",str0,strlen((char*)str0));
			MQTT_Publish((u8*)"G/temp",temp,strlen((char*)temp));
			xSemaphoreGive(bsTempEvent);
			vTaskDelay(1000);
		}
		else
		{
			MQTT_Publish((u8*)"G/temp",temp,strlen((char*)temp));
			vTaskDelay(1000);
		}
		MQTT_Publish((u8*)"G/hum",hum,strlen((char*)hum));
		vTaskDelay(1000);
	}
	
		
}
/****************************Outer light and irregation control*********************************/
void T_SOIL_LIGHT_Input(void* pvInitData){
	u8 tempstr[4]={0};
	u16 LDR=0;
	u16 soil=0;
	while (1)
	{
		soil=read_analog(SOILSENSOR);
		LDR=read_analog(LDRSENSOR);
		soil=(100-(soil*100UL)/1019UL);
		LDR=((LDR*100UL)/407UL);
		
		//LCD_GOTXY(0,0);
		disp_strXY(1,1,(u8*)"soil moist=   %");
		LCD_GOTXY(1,12);
		disp_int(soil);
		disp_strXY(2,1,(u8*)"Light=   %");
		LCD_GOTXY(2,7);
		disp_int(LDR);
		
		if (soil<=30)//turn on pump
		{
			xSemaphoreGive(IrrigationEvent);
		}
		else
		{

		}
		//controlling outer lights
		if (LDR<30)//turn on lights
		{
			xSemaphoreGive(LightsEvent);
		}
		else
		{
			
		}
		
		vTaskDelay(100);///--->>CH
	}
}

void T_SOIL_LIGHT_Action(void* pvInitData)
{
	while (1)
	{
		if(xSemaphoreTake(IrrigationEvent,100))
		{
			writePin(SOIL_PUMP,HIGH);
		}
		else
		{
			writePin(SOIL_PUMP,LOW);
		}
		if(xSemaphoreTake(LightsEvent,100))
		{
			writePin(OUTERLIGHT,HIGH);
		}
		else
		{
			writePin(OUTERLIGHT,LOW);
		}
	}
}
/***********************************************************************************************************/void T_PasswordGet(void *pvinitData){	u8 password[5]	=	{0};		u8 keyPressed=0, counter=0;		while(1)	{
			if (xSemaphoreTake(bsEnterAgain,300))			{				LCD_CLEAR();
				disp_strXY(2,1,(u8*)"Enter password :");
				keyPressed=0;
				counter=0;
				while (counter<5)
				{
					keyPressed=GetKey();
					
					if (keyPressed!='#' && keyPressed!=0)
					{
						password[counter]=keyPressed;
						disp_charXY(3,counter+1,keyPressed);
						keyPressed=0;
						counter++;
					}//end if
					else if(keyPressed=='#')
					{
						/*password[counter]='\0';*/
						counter=0;
						xQueueSend(mqPassword,password,300);
						xSemaphoreGive(bsPassEntered);
					}// else if

				}//while			}			vTaskDelay(200);	}}void T_PasswordCheck(void *pvinitData){	u8 password[5]={0};	while(1)	{				if (xSemaphoreTake(bsPassEntered,1000))
		{
			xQueueReceive(mqPassword,password,100);
			if (strcmp((char*)oldpassword,(char*)password)==0)
			{
				LCD_CLEAR();
				writePin(DOOR_LED,HIGH);//open door
				disp_strXY(2,1,(u8*)"Pass accepted");
				_delay_ms(3000);
				writePin(DOOR_LED,LOW);//close door
			}
			else
			{
				LCD_CLEAR();
				disp_strXY(2,1,(u8*)"Wrong Pass");
				vTaskDelay(1000);
				LCD_CLEAR();
				disp_strXY(2,1,(u8*)"Enter password :");
				xSemaphoreGive(bsEnterAgain);
			}
			vTaskDelay(200);
		}// if semaphore take			}}void T_DoorAction(void *pvinitData){	while(1)	{				}}





/***********************************************************************************************************/

void turnOnFireAlarm(void)
{
	//Callback(FireAlarm);
}

void Project_Init(void)
{
	KEYPAD_Init();
	LCD_init();
	ADC_init();
	Uart_Init(UART_0, 9600);
	
	
	
	
	MQTT_Connect((u8*)"1111");
	disp_strXY(1,1,(u8*)"** WELCOM TO ***");
	disp_strXY(2,1,(u8*)"**** SMART *****");
	disp_strXY(3,1,(u8*)"**** HOME ******");
	disp_strXY(4,1,(u8*)"****************");
	_delay_ms(3000);
	LCD_CLEAR();
	
	
	pinDirection(DOOR_LED,OUTPUT);
	
	
	
	pinDirection(TV_PIN,OUTPUT);
	
	pinDirection(INNERLIGHT,OUTPUT);
	
	
	/*fire system config*/
	xTaskCreate(T_FireDetect,NULL,100,NULL,9,NULL);
	xTaskCreate(T_FireAlarmAction,NULL,100,NULL,10,NULL);
	bsFireEvent=xSemaphoreCreateBinary();
	pinDirection(FLAME_SENSOR,INPUT);//Flame sensor as input
	pinDirection(FIREPUMP,OUTPUT);
	pinDirection(BUZZER,OUTPUT);
	
	
	
	/*Temperature system config*/
 	xTaskCreate(T_TempRead,NULL,200,NULL,7,NULL);
 	xTaskCreate(T_TempAction,NULL,100,NULL,8,NULL);
	bsTempEvent=xSemaphoreCreateBinary();
	pinDirection(FAN,OUTPUT);
	
	
	/*Door Security system config*/
	xTaskCreate(T_PasswordGet,NULL,100,NULL,4,NULL);	xTaskCreate(T_PasswordCheck,NULL,100,NULL,5,NULL);	//xTaskCreate(T_DoorAction,NULL,100,NULL,6,NULL);
	//bsPassEvent		=	xSemaphoreCreateBinary();
	bsEnterAgain	=	xSemaphoreCreateBinary();
	bsPassEntered	=	xSemaphoreCreateBinary();
	mqPassword		=	xQueueCreate(PASS_NUM,PASS_LENGTH);
	xSemaphoreGive(bsEnterAgain);
	
	
	/* SOIL IRRIGATION and LIGHTS IN NIGHT */
	xTaskCreate(T_SOIL_LIGHT_Input,NULL,100,NULL,1,NULL);
	xTaskCreate(T_SOIL_LIGHT_Action,NULL,100,NULL,2,NULL);
	IrrigationEvent = xSemaphoreCreateBinary(); // make sure if it is initially = 0
	LightsEvent	    = xSemaphoreCreateBinary();
	pinDirection(SOIL_PUMP,OUTPUT);
	pinDirection(OUTERLIGHT,OUTPUT);
	
	
	
	
	
	vTaskStartScheduler();
}


void Get_Password(u8* password)
{
	
	while(flag!=1)
	{
	LCD_CLEAR();
	disp_strXY(2,1,(u8*)"Enter password :");
	u8 keyPressed=0, counter=0;
	while (counter<4)
	{
		keyPressed=GetKey();
		
		if (keyPressed!=0)
		{
			password[counter]=keyPressed;
			disp_charXY(3,counter+1,keyPressed);
			keyPressed=0;
			counter++;
		}
	}
	Check_Password(password);
	}
}

void Check_Password(u8* password)
{
	
	if (strcmp((char*)oldpassword,(char*)password)==0)
	{
		LCD_CLEAR();
		writePin(DOOR_LED,HIGH);//open door
		disp_strXY(2,1,(u8*)"Pass accepted");
		_delay_ms(3000);
		writePin(DOOR_LED,LOW);//close door
		trials=0;
		flag=1;
		return;
	}
	disp_intXY(2,1,trials);
	if (trials>=2)
	{
		writePin(BUZZER,HIGH); // 	turn on alarm
		_delay_ms(3000);
		writePin(BUZZER,LOW);// 	turn off alarm
		trials=0;
		return;
	}
	LCD_CLEAR();
	disp_strXY(2,1,(u8*)"Wrong Pass");
	_delay_ms(500);
	trials++;
	disp_intXY(2,6,trials);
	Get_Password(password);	
}


void temp(void)
{
	u8 temp[60];
	u8 hum[60];
	disp_strXY(3,1,(u8*)"Humid=     %RH");
	disp_strXY(4,1,(u8*)"temp=      Deg.c");
	DHT_Represent(hum,temp);
	disp_strXY(3,7,hum);
	disp_strXY(4,6,temp);
	
	MQTT_Publish((u8*)"G/DHT",temp,strlen((char*)temp));
	_delay_ms(1000);
	MQTT_Publish((u8*)"G/hum",hum,strlen((char*)hum));
	_delay_ms(1000);
}


void FireAlarm(void)
{
	u8 msg[]="home on fire";
	TGLBit(PORTD,3);
	TGLBit(PORTB,7);
	MQTT_Publish((u8*)"G/fire",msg,strlen((char*)msg));
	_delay_ms(1000);	
}

void IOTcharinput(u8 data)
{
	switch(data)
	{
		case '0':
		writePin(TV_PIN,LOW);
		break;
		case '1':
		writePin(TV_PIN,HIGH);
		break;
		case '2':
		writePin(INNERLIGHT,HIGH);
		break;
		case '3':
		writePin(INNERLIGHT,LOW);
		break;
		case '4':
		writePin(FAN,HIGH);
		break;
		case '5':
		writePin(FAN,LOW);
		break;
		case '6':
		writePin(OUTERLIGHT,HIGH);
		break;
		case '7':
		writePin(OUTERLIGHT,LOW);
		break;
	}
}




