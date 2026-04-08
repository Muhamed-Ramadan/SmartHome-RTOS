/*
 * ADC.c
 *
 * Created: 8/21/2021 11:01:34 AM
 *  Author: ehab2_phjirwi
 */ 

#include "registers.h"
#include "ADC.h"




void ADC_init(void)
{
	ADMUX|=(AVCC<<6)|(ADLAR<<5);	
	ADCSRA|=(SELEPRESC)|(ADATE<<5)|(ADIE<<3)|(ADEN<<7);
}
u16 read_analog(u16 ADC_NUM)
{ 
	ADMUX&=~(0x1F);
	ADMUX|=(ADC_NUM);
	ADCSRA|=(1<<ADSC);			 //start conversion
	while((ADCSRA>>6)&0x01); //wait until conversion complete
	return (ADC);		
}
