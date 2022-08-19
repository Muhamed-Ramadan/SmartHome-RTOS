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

/* Global Shared variables  */
SysVar SensorReading={"0","25","0",0,30,0,20,0};
// initial values as: Tc=0 deg ; Tt=25 deg ; Hum=0 %Hg ; MOIST=0 ; LDR=0 ; LCDState = MAINSTATE  //


/*RTOS Services*/

EventGroupHandle_t egEvents =NULL;
EventGroupHandle_t egEvents1 =NULL;


xSemaphoreHandle	bsPassEntered			=	NULL;
xSemaphoreHandle	bsPassCheck				=	NULL;
xQueueHandle		mqPassword				=	NULL;

static u8 oldpassword[]="1234";




/**************** LCD Display Task ****************/


void	T_DisplayData		(void *pvinitData)
{
	EventBits_t state=0;
	u8 moistPublish[4];
	u8 lightPublish[4];


	while(1)
	{
		state=xEventGroupWaitBits(egEvents,(E_FireAlarmDisp|E_GetPassDisp|E_TempConfigDisp|E_MainDisp|E_CTempUpdate|E_CMoistUpdate|E_CHUMUpdate|E_CLightUpdate|E_UIDisp|E_SITDisp),1,0,150);
		if((state&E_MainDisp)==E_MainDisp)
		{
			if (SensorReading.LCDState==MAIN_STATE)
			{
				disp_strXY(1,1,MAIN_L1);
				disp_intXY(1,12,SensorReading.C_MOIST);
				disp_strXY(2,1,MAIN_L2);
				disp_intXY(2,7,SensorReading.C_LDR);
				disp_strXY(3,1,MAIN_L3);
				disp_strXY(3,7,SensorReading.C_HUM);
				disp_strXY(4,1,MAIN_L4);
				disp_strXY(4,6,SensorReading.C_TEMP);
			}
		}
		else if((state&E_UIDisp)==E_UIDisp)
		{
			disp_strXY(1,1,UI_L1);
			disp_strXY(2,1,UI_L2);
			disp_strXY(3,1,UI_L3);
			disp_strXY(4,1,UI_L4);
		}
		else if((state&E_SITDisp)==E_SITDisp)
		{
			disp_strXY(1,1,SETTING_L1);
			disp_strXY(2,1,SETTING_L2);
			disp_strXY(3,1,SETTING_L3);
			disp_strXY(4,1,SETTING_L4);
		}
		else if((state&E_TempConfigDisp)==E_TempConfigDisp)
		{
			disp_strXY(1,1,GetTemp_L1);
			disp_strXY(2,1,GetTemp_L2);
			disp_strXY(2,9,(u8*)"old=");
			disp_strXY(2,13,SensorReading.T_TEMP);
			disp_strXY(3,1,GetTemp_L3);
			disp_strXY(4,1,GetTemp_L4);
		}
		else if((state&E_GetPassDisp)==E_GetPassDisp)
		{
			disp_strXY(1,1,Getpass_L1);
			disp_strXY(2,1,Getpass_L2);
			disp_strXY(3,1,Getpass_L3);
			disp_strXY(4,1,Getpass_L4);
		}
		else if((state&E_FireAlarmDisp)==E_FireAlarmDisp)
		{
			disp_strXY(1,1,ALARM_L1);
			disp_strXY(2,1,ALARM_L2);
			disp_strXY(3,1,ALARM_L3);
			disp_strXY(4,1,ALARM_L4);
			vTaskDelay(700);
			LCD_CLEAR();
			vTaskDelay(300);
		}
		else
		{}
		
		
		
		
		if((state&E_CMoistUpdate)==E_CMoistUpdate)
		{
			disp_strXY(1,12,(u8*)"   ");
			disp_intXY(1,12,SensorReading.C_MOIST);
			itoa(SensorReading.C_MOIST,moistPublish,10);
			MQTT_Publish((u8*)"G/soil",moistPublish,strlen((char*)moistPublish));

		}
		if((state&E_CLightUpdate)==E_CLightUpdate)
		{
			disp_strXY(2,7,(u8*)"   ");
			disp_intXY(2,7,SensorReading.C_LDR);
			itoa(SensorReading.C_LDR,lightPublish,10);
			MQTT_Publish((u8*)"G/light",lightPublish,strlen((char*)lightPublish));
		}
		if((state&E_CHUMUpdate)==E_CHUMUpdate)
		{
			disp_strXY(3,7,(u8*)"   ");
			disp_strXY(3,7,SensorReading.C_HUM);
			MQTT_Publish((u8*)"G/hum",SensorReading.C_HUM,strlen((char*)SensorReading.C_HUM));

		}
		if((state&E_CTempUpdate)==E_CTempUpdate)
		{
			disp_strXY(4,6,(u8*)"      ");
			disp_strXY(4,6,SensorReading.C_TEMP);
			MQTT_Publish((u8*)"G/temp",SensorReading.C_TEMP,strlen((char*)SensorReading.C_TEMP));
		}
		state=xEventGroupWaitBits(egEvents1,(E_MoistConigDisp|E_lightConigDisp),1,0,150);
		if((state&E_MoistConigDisp)==E_MoistConigDisp)
		{
			disp_strXY(1,1,GetMoist_L1);
			disp_strXY(2,1,GetMoist_L2);
			disp_strXY(2,9,(u8*)"old=");
			disp_intXY(2,13,SensorReading.T_MOIST);
			disp_strXY(3,1,GetMoist_L3);
			disp_strXY(4,1,GetMoist_L4);
		}
		else if((state&E_lightConigDisp)==E_lightConigDisp)
		{
			disp_strXY(1,1,GetLight_L1);
			disp_strXY(2,1,GetLight_L2);
			disp_strXY(2,9,(u8*)"old=");
			disp_intXY(2,13,SensorReading.T_LDR);
			disp_strXY(3,1,GetLight_L3);
			disp_strXY(4,1,GetLight_L4);
		}
		vTaskDelay(100);
		
	}
}

/******************************************************************************* fire alarm tasks **************************************************************************************************/
void T_FireDetect(void *pvinitData)
{
	while (1)
	{
		if (readPin(FLAME_SENSOR))
		{
			xEventGroupSetBits(egEvents,(E_FireDetect|E_FireAlarmDisp));
		}
		vTaskDelay(500);
	}
}
void T_FireAlarmAction(void *pvinitData)
{
	u8 flag=0;
	u8 msg[]="home on fire";
	u8 msg2[]="No fire";
	EventBits_t state=0;
	while (1)
	{
		
		state=xEventGroupWaitBits(egEvents,E_FireDetect,1,0,500);
		if ((state&E_FireDetect)==E_FireDetect)
		{
			writePin(FIREPUMP,HIGH);
			writePin(BUZZER,HIGH);
			vTaskDelay(600);
			writePin(BUZZER,LOW);
			vTaskDelay(150);
			MQTT_Publish((u8*)"G/fire",msg,strlen((char*)msg));
			flag=1;
		}
		else
		{
			if(flag==1)
			{
				flag=0;
				MQTT_Publish((u8*)"G/fire",msg2,strlen((char*)msg2));
				
				SensorReading.LCDState=MAIN_STATE;
				xEventGroupSetBits(egEvents,E_MainDisp);
			}
			writePin(FIREPUMP,LOW);
			writePin(BUZZER,LOW);
		}
		vTaskDelay(400);
	}
}


/************************************************************************************* temp control tasks ***************************************************************************************************/
void T_TempAction(void *pvinitData)
{
	u8 str0[]="Fan ON";
	u8 str1[]="Fan Off";
	
	EventBits_t state=0;
	vTaskDelay(1000);
	while(1)
	{
		state=xEventGroupWaitBits(egEvents,E_TempCheck,1,0,portMAX_DELAY);
		if((state&E_TempCheck)==E_TempCheck)
		{
			if(atoi(SensorReading.C_TEMP)>atoi(SensorReading.T_TEMP))
			{
				writePin(FAN,HIGH);
				MQTT_Publish((u8*)"G/temp",str0,strlen((char*)str0));
			}
			else
			{
				writePin(FAN,LOW);
				MQTT_Publish((u8*)"G/temp",str1,strlen((char*)str1));
			}
		}
		vTaskDelay(1000);

	}
}
void T_TempRead	 (void *pvinitData)
{
	u8 temp[15];
	u8 hum[15];
	while(1)
	{
		DHT_Represent(hum,temp);
		if((strcmp((char*)temp,SensorReading.C_TEMP))!=0)
		{
			strcpy(SensorReading.C_TEMP,(char*)temp);
			if(SensorReading.LCDState==MAIN_STATE)
			{
				xEventGroupSetBits(egEvents,E_CTempUpdate);
			}
			xEventGroupSetBits(egEvents,E_TempCheck);
		}
		if ((strcmp((char*)hum,SensorReading.C_HUM))!=0)
		{
			strcpy(SensorReading.C_HUM,(char*)hum);
			if(SensorReading.LCDState==MAIN_STATE)
			{
				xEventGroupSetBits(egEvents,E_CHUMUpdate);
			}
		}
		else
		{
		}
		vTaskDelay(10000);
	}
	
	
}
/*************************************************************************************Outer light **************************************************************************************************************/
void T_LIGHT_Input(void* pvInitData){
	u16 LDR=0;
	while (1)
	{
		LDR=read_analog(LDRSENSOR);
		LDR=((LDR*100UL)/407UL);
		
		//controlling outer lights
		if(LDR != SensorReading.C_LDR )
		{
			SensorReading.C_LDR = LDR;
			if (SensorReading.LCDState == MAIN_STATE)
			{
				xEventGroupSetBits(egEvents,E_CLightUpdate);
			}
			xEventGroupSetBits(egEvents,E_lightCheck);
		}
		vTaskDelay(1000); //sec for Testing to be updated
	}
}
void T_LIGHT_Action(void* pvInitData)
{
	
	u8 msg1[]="Outer light ON";
	u8 msg2[]="Outer light OFF";
	EventBits_t state=0;
	while (1)
	{
		state=xEventGroupWaitBits(egEvents,E_lightCheck,1,0,portMAX_DELAY);
		if((state&E_lightCheck)==E_lightCheck)
		{
			if(SensorReading.C_LDR<SensorReading.T_LDR)
			{
				MQTT_Publish((u8*)"G/light",msg1,strlen((char*)msg1));
				writePin(OUTERLIGHT,HIGH);
			}
			else
			{
				MQTT_Publish((u8*)"G/light",msg2,strlen((char*)msg2));
				writePin(OUTERLIGHT,LOW);
			}
			vTaskDelay(5000);
		}
	}
}
/*******************************************************************************************************Irregation control**************************************************************************************/
void T_SOIL_Input(void* pvInitData)
{
	u16 Moist=0;
	while (1)
	{
		Moist=read_analog(SOILSENSOR);
		Moist=(100-(Moist*100UL)/1019UL);
		if(Moist != SensorReading.C_MOIST )
		{
			SensorReading.C_MOIST = Moist;
			if (SensorReading.LCDState == MAIN_STATE)
			{
				xEventGroupSetBits(egEvents,E_CMoistUpdate);
			}
			xEventGroupSetBits(egEvents,E_MoistCheck);
		}
		vTaskDelay(10000);
	}
}
void T_SOIL_Action(void* pvInitData)
{
	
	u8 msg1[]="Water Pump ON";
	u8 msg2[]="Water Pump OFF";
	EventBits_t state=0;
	while (1)
	{
		state=xEventGroupWaitBits(egEvents,E_MoistCheck,1,0,portMAX_DELAY);
		if((state&E_MoistCheck)==E_MoistCheck)
		{
			if(SensorReading.C_MOIST<SensorReading.T_MOIST)
			{
				MQTT_Publish((u8*)"G/soil",msg1,strlen((char*)msg1));
				writePin(SOIL_PUMP,HIGH);
			}
			else
			{
				MQTT_Publish((u8*)"G/soil",msg2,strlen((char*)msg2));
				writePin(SOIL_PUMP,LOW);
			}
			vTaskDelay(500);
		}
	}
}
/*************************************************************************************Door system control***************************************************************************************************/void T_PasswordGet(void *pvinitData){	u8 password[5]	=	{0};	u8 keyPressed=0, counter=0;	//EventBits_t state=0;	while(1)	{
		//state=xEventGroupWaitBits(egEvents,E_UIDOOR,0,0,1000);
		//if ((state&E_UIDOOR)==E_UIDOOR)
		if(xSemaphoreTake(bsPassEntered,1000))
		{
			keyPressed=0;
			counter=0;
			while (counter<5 && keyPressed !='#'&& keyPressed !='*' &&SensorReading.LCDState==GET_PASS_STATE)
			{
				
				keyPressed=GetKey();
				
				if (keyPressed!='#' && keyPressed!=0 && keyPressed!='*')
				{
					password[counter]=keyPressed;
					disp_charXY(2,counter+1,keyPressed);
					keyPressed=0;
					counter++;
				}//end if
				else if(keyPressed=='*' || (counter==(strlen(oldpassword))))
				{
					counter=0;
					xQueueSend(mqPassword,password,300);
					password[0]=0;
					password[1]=0;
					password[2]=0;
					password[3]=0;
					xSemaphoreGive(bsPassCheck);
				}
				else if(keyPressed=='#')
				{
					SensorReading.LCDState=MAIN_STATE;
					xEventGroupSetBits(egEvents,E_MainDisp);
				}
				vTaskDelay(200);
			}
					}//if semaphor
				vTaskDelay(200);	}}void T_PasswordCheck(void *pvinitData){	u8 password[5]={0};	//EventBits_t state=0;	while(1)	{		//state=xEventGroupWaitBits(egEvents,E_PassCheck,1,0,portMAX_DELAY);		if (xSemaphoreTake(bsPassCheck,portMAX_DELAY))
		{
			xQueueReceive(mqPassword,password,100);
			if (strcmp((char*)oldpassword,(char*)password)==0)
			{
				LCD_CLEAR();
				writePin(DOOR_LED,HIGH);//open door
				disp_strXY(2,1,(u8*)"Pass accepted");
				vTaskDelay(3000);
				writePin(DOOR_LED,LOW);//close door
			}
			else
			{
				LCD_CLEAR();
				disp_strXY(2,1,(u8*)"Wrong Pass");
				vTaskDelay(1000);
			}
			SensorReading.LCDState=MAIN_STATE;
			xEventGroupSetBits(egEvents,E_MainDisp);
			vTaskDelay(300);
		}// if semaphore take			}}
/***********************************************************************************************************/

void Project_Init(void)
{
	KEYPAD_Init();
	LCD_init();
	ADC_init();
	Uart_Init(UART_0, 9600);
	
	pinDirection(DOOR_LED,OUTPUT);
	
	pinDirection(TV_PIN,OUTPUT);
	pinDirection(INNERLIGHT,OUTPUT);
	
	
	egEvents = xEventGroupCreate();					//Event Group Create
	egEvents1 = xEventGroupCreate();
	xEventGroupSetBits(egEvents,E_MainDisp);
	
	
	/* LCD Display config*/
	xTaskCreate(T_DisplayData,NULL,200,NULL,1,NULL);
	
	
	/*fire system config*/
	xTaskCreate(T_FireDetect,NULL,100,NULL,9,NULL);
	xTaskCreate(T_FireAlarmAction,NULL,85,NULL,10,NULL);
	pinDirection(FLAME_SENSOR,INPUT);//Flame sensor as input
	pinDirection(FIREPUMP,OUTPUT);
	pinDirection(BUZZER,OUTPUT);
	
	/*irrigation system config*/
	xTaskCreate(T_SOIL_Input,NULL,100,NULL,2,NULL);
	xTaskCreate(T_SOIL_Action,NULL,85,NULL,3,NULL);
	pinDirection(SOIL_PUMP,OUTPUT);
	
	/*Outer light system config*/
	xTaskCreate(T_LIGHT_Input,NULL,100,NULL,2,NULL);
	xTaskCreate(T_LIGHT_Action,NULL,85,NULL,3,NULL);
	pinDirection(OUTERLIGHT,OUTPUT);
	
	/*Temperature system config*/
	xTaskCreate(T_TempRead,NULL,200,NULL,7,NULL);
	xTaskCreate(T_TempAction,NULL,85,NULL,8,NULL);
	pinDirection(FAN,OUTPUT);
	/////////////////////////////////////
	xTaskCreate(T_UserInput,NULL,250,NULL,4,NULL);
	
	
	/*Door Security system config*/
	xTaskCreate(T_PasswordGet,NULL,100,NULL,5,NULL);	xTaskCreate(T_PasswordCheck,NULL,100,NULL,6,NULL);
	bsPassCheck	=	xSemaphoreCreateBinary();
	bsPassEntered	=	xSemaphoreCreateBinary();
	mqPassword		=	xQueueCreate(PASS_NUM,PASS_LENGTH);
	
	/*TV,INNER LIGHTS USER CONTROL*/
	xTaskCreate(IOTcharinput,NULL,120,NULL,2,NULL);
	
	/*System Modifications Tasks*/	xTaskCreate(T_SystemModify,NULL,200,NULL,5,NULL);
	
	MQTT_Connect((u8*)"1111");
	disp_strXY(1,1,(u8*)"** WELCOME TO **");
	disp_strXY(2,1,(u8*)"**** SMART *****");
	disp_strXY(3,1,(u8*)"**** HOME ******");
	disp_strXY(4,1,(u8*)"****************");
	_delay_ms(3000);
	LCD_CLEAR();
	MQTT_Subscribe("G/devices");
	
	
	
	
	vTaskStartScheduler();
}

/********************************************************************************/

void T_UserInput(void *pvinitData)
{
	u8 keyPressed=0;
	while (1)
	{
		if (SensorReading.LCDState==MAIN_STATE)
		{
			keyPressed=GetKey();
			if (keyPressed!=0)
			{
				keyPressed=0;
				SensorReading.LCDState=UI_STATE;
				xEventGroupSetBits(egEvents,E_UIDisp);
			}
		}
		else if (SensorReading.LCDState==UI_STATE)
		{
			vTaskDelay(100);
			keyPressed=GetKey();
			switch(keyPressed)
			{
				case'1':
				SensorReading.LCDState=GET_PASS_STATE;
				xEventGroupSetBits(egEvents,E_GetPassDisp);
				vTaskDelay(200);
				//xEventGroupSetBits(egEvents,E_UIDOOR);
				xSemaphoreGive(bsPassEntered);
				break;
				case'2':
				SensorReading.LCDState=CONFIG_STATE;
				xEventGroupSetBits(egEvents,E_SITDisp);
				break;
				case'#':
				SensorReading.LCDState=MAIN_STATE;
				xEventGroupSetBits(egEvents,E_MainDisp);
				break;
				
			}
			
		}
		else if (SensorReading.LCDState==CONFIG_STATE)
		{
			vTaskDelay(100);
			keyPressed=GetKey();
			switch(keyPressed)
			{
				case'1':
				SensorReading.LCDState=TEMP_CONFIG_STATE;
				xEventGroupSetBits(egEvents,E_TempConfigDisp);
				xEventGroupSetBits(egEvents1,E_GetTemp);
				vTaskDelay(200);
				break;
				case'2':
				SensorReading.LCDState=LIGHT_CUTOFF_STATE;
				xEventGroupSetBits(egEvents1,E_lightConigDisp);
				xEventGroupSetBits(egEvents1,E_GetLight);
				break;
				case'3':
				SensorReading.LCDState = MOIST_CONFIG_STATE;
				xEventGroupSetBits(egEvents1,E_MoistConigDisp);
				xEventGroupSetBits(egEvents1,E_GetMoist);
				break;
				case'#':
				SensorReading.LCDState=MAIN_STATE;
				xEventGroupSetBits(egEvents,E_MainDisp);
				break;
				
			}
			
		}
		vTaskDelay(50);
	}
}



/**********************************System Modifications************************************/

void T_SystemModify			(void *pvinitData)
{
	u8 temp[7]={0};
	u8 index=0,keyPressed=0;

	EventBits_t state=0;
	while(1)
	{
		state = xEventGroupWaitBits(egEvents1,E_GetMoist|E_GetLight|E_GetTemp,1,0,250);
		if((state&E_GetMoist)==E_GetMoist)
		{
			index=0;
			keyPressed=0;
			while(index<4 && keyPressed !='#'&& keyPressed !='*' &&SensorReading.LCDState==MOIST_CONFIG_STATE)
			{
				keyPressed=GetKey();
				if(keyPressed != 0 && keyPressed !='#'&& keyPressed !='*')
				{
					temp[index] = keyPressed;
					disp_charXY(2,index+1,keyPressed);
					index++;
					keyPressed=0;
				}
				else if(keyPressed =='*')
				{
					SensorReading.T_MOIST = atoi(temp);
					for(index=0;index<3;index++)
					{
						temp[index] = 0;
					}
					index=0;

					SensorReading.LCDState = MAIN_STATE;
					xEventGroupSetBits(egEvents,E_MainDisp);
					xEventGroupSetBits(egEvents,E_MoistCheck);
				}
				else if(keyPressed == '#')
				{
					for(index=0;index<3;index++)
					{
						temp[index] = 0;
					}
					index=0;

					SensorReading.LCDState = MAIN_STATE;
					xEventGroupSetBits(egEvents,E_MainDisp);
				}
				vTaskDelay(100);
			}
		}//end of MOIST
		/***********************************************************************************************************************************************************************************************************************************************************************/
		else if((state&E_GetLight)==E_GetLight)
		{
			index=0;
			keyPressed=0;
			while(index<4 && keyPressed !='#'&& keyPressed !='*' &&SensorReading.LCDState==LIGHT_CUTOFF_STATE)
			{
				keyPressed=GetKey();
				if(keyPressed != 0 && keyPressed !='#'&& keyPressed !='*')
				{
					temp[index] = keyPressed;
					disp_charXY(2,index+1,keyPressed);
					index++;
					keyPressed=0;
				}
				else if(keyPressed =='*')
				{
					SensorReading.T_LDR = atoi(temp);
					for(index=0;index<3;index++)
					{
						temp[index] = 0;
					}
					index=0;

					SensorReading.LCDState = MAIN_STATE;
					xEventGroupSetBits(egEvents,E_MainDisp);
					xEventGroupSetBits(egEvents,E_lightCheck);
				}
				else if(keyPressed == '#')
				{
					for(index=0;index<3;index++)
					{
						temp[index] = 0;
					}
					index=0;

					SensorReading.LCDState = MAIN_STATE;
					xEventGroupSetBits(egEvents,E_MainDisp);
				}
				vTaskDelay(100);
			}
		}
		/***********************************************************************************************************************************************************************************************************************************************************************/
		else if((state&E_GetTemp)==E_GetTemp)
		{
			index=0;
			keyPressed=0;
			while(index<4 && keyPressed !='#'&& keyPressed !='*' &&SensorReading.LCDState==TEMP_CONFIG_STATE)
			{
				keyPressed=GetKey();
				if(keyPressed != 0 && keyPressed !='#'&& keyPressed !='*')
				{
					temp[index] = keyPressed;
					disp_charXY(2,index+1,keyPressed);
					index++;
					keyPressed=0;
				}
				else if(keyPressed =='*')
				{
					strcpy(SensorReading.T_TEMP,temp);
					for(index=0;index<3;index++)
					{
						temp[index] = 0;
					}
					index=0;

					SensorReading.LCDState = MAIN_STATE;
					xEventGroupSetBits(egEvents,E_MainDisp);
					xEventGroupSetBits(egEvents,E_TempCheck);
				}
				else if(keyPressed == '#')
				{
					for(index=0;index<3;index++)
					{
						temp[index] = 0;
					}
					index=0;

					SensorReading.LCDState = MAIN_STATE;
					xEventGroupSetBits(egEvents,E_MainDisp);
				}
				vTaskDelay(100);
			}
		}
		vTaskDelay(200);
	}//while
}

/**********************************************INNER,TV USER CONTROL***************************************************/
extern u8 data;
void IOTcharinput(void *pvinitData)
{
	while(1)
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
		vTaskDelay(300);
	}
}

/****************************************************************************************************************/



