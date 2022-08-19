/*
 * HDT.c
 *
 * Created: 8/24/2021 10:27:13 AM
 *  Author: Mahmoud-PC
 */ 
#define	F_CPU	8000000UL
#include "dataTypes.h"
#include "bitMath.h"
#include "DIO.h"
#include "DHT.h"
#include <string.h>
#include <stdio.h>
#include <util/delay.h>


void DHT_start(void)
{
	pinDirection(DHT_PIN,OUTPUT);		
	writePin(DHT_PIN,LOW);				//apply 0 to start the sensor
	_delay_ms(20);						
	writePin(DHT_PIN,HIGH);				//apply 1 to start the sensor
}

u8 DHT_read(void)
{
	u8 bitLogic=0 ,i=0;
	for (i=0;i<8;i++)
	{		while (readPin(DHT_PIN)==0);		_delay_us(30);		if (readPin(DHT_PIN)==0)		{			bitLogic=bitLogic<<1;		}		else		{			bitLogic=((bitLogic<<1)|0x01);		}				while(readPin(DHT_PIN));			}
	return bitLogic;
}

void DHT_Response(void)
{
	pinDirection(DHT_PIN,INPUT);
	while(readPin(DHT_PIN));
	while(!readPin(DHT_PIN));
	while(readPin(DHT_PIN));
}

BOOL DHT_Represent(u8* hum,u8* temp)
{
	u16 temp_1=0,hum_1=0;
	u8 humidity_I = 0 , humidity_D = 0, temp_I = 0, temp_D = 0 , sumCheck = 0;
	DHT_start();
	DHT_Response();
	humidity_I=DHT_read();
	humidity_D=DHT_read();
	temp_I=DHT_read();
	temp_D=DHT_read();
	
		hum_1=((humidity_I<<8)|humidity_D);
		temp_1=((temp_I<<8)|temp_D);
		
		intTostring(hum_1,temp_1,temp,hum);
		return TRUE;
	
}


void intTostring(u16 hum_1,u16 temp_1,u8* temp,u8* hum )
{
	u8 dot[]=".";
	u8 temp_2[10]={0},hum_2[10]={0};
			if (GETBit(temp_1,15)==1)
			{
				CLRBit(temp_1,15);
				temp[0]='-';
				temp++;
			}
			sprintf(temp,"%u",(temp_1)/10);
			sprintf(hum,"%u",(hum_1)/10);
			
			sprintf(temp_2,"%u",((temp_1)%10));
			sprintf(hum_2,"%u",((hum_1)%10));
			
			strcat(hum,dot);
			strcat(temp,dot);
			strcat(hum,hum_2);
			strcat(temp,temp_2);
}