/*
 * ADC.h
 *
 * Created: 8/21/2021 11:01:05 AM
 *  Author: ehab2_phjirwi
 */ 


#ifndef ADC_H_
#define ADC_H_


enum {ADC0,ADC1,ADC2,ADC3,ADC4,ADC5,ADC6,ADC7}ADC_SELECT;
enum {AREF,AVCC,Internal_2=3}voltageRef;
enum{DIV_2=1,DIV_4,DIV_8,DIV_16,DIV_32,DIV_64,DIV_128}prescaler;
	
		
#define ADLAR		0		//0 -> for right adjustment				1 -> for left adjustment	
#define ADEN		1		//1 -> to enable ADC				    0 -> to disable ADC
#define ADATE		0		//1 -> to enable Auto trigger			0 -> to disable Auto trigger
#define ADIE		0		//1 -> to enable interupt		    	0 -> to disable interupt
#define ADSC		6
#define SELEPRESC  DIV_128  // the prescaler for ADC (determine the division factor between the XTAL frequency and the input clock to the ADC.)

void ADC_init(void);
u16 read_analog(u16 ADC_NUM);

#endif /* ADC_H_ */