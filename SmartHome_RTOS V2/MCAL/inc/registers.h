/*
 * registers.h
 *
 * Created: 8/18/2021 10:14:58 PM
 *  Author: Mohamed
 */ 


#ifndef REGISTERS_H_
#define REGISTERS_H_

#include "dataTypes.h"


/*A general function to provide the address*/
#define SELECTOR(ADDRESS) (*((volatile u8*)ADDRESS))
#define SELECTOR_16(ADDRESS) (*((volatile u16*)ADDRESS))



/*Port A Register*/                               
/*if the DDRx is set to be output and we write High to the PORTx 
* this will activate the internal Pull up resistor.
*/
#define PORTA  SELECTOR(0x3B)    //1->high output				0->low output  
#define DDRA   SELECTOR(0x3A)   //1->to make it output			0->to make it input    
#define PINA   SELECTOR(0x39)  //this register to read a value from a pin 

/*Port B Register*/
#define PORTB	SELECTOR(0x38)
#define DDRB	SELECTOR(0x37)
#define PINB	SELECTOR(0x36)

/*Port C Register*/
#define PORTC	SELECTOR(0x35)
#define DDRC	SELECTOR(0x34)
#define PINC	SELECTOR(0x33)

/*Port D Register*/
#define PORTD	SELECTOR(0x32)
#define DDRD	SELECTOR(0x31)
#define PIND	SELECTOR(0x30)

/*Port F Register*/
#define PORTF	SELECTOR(0x62)
#define DDRF	SELECTOR(0x61)
#define PINF	SELECTOR(0x20)

/*ADC registers*/

#define ADMUX	 SELECTOR(0x27)
#define ADCSRA	 SELECTOR(0x26)
#define ADCH	 SELECTOR(0x25)
#define ADCL	 SELECTOR(0x24)
#define ADC		 SELECTOR_16(0x24)

/*EXTERNAL INTERRUPT REGESTERS*/

#define MCUCR  SELECTOR(0X55)
#define MCUCSR SELECTOR(0X54)
#define GICR   SELECTOR(0X5B)
#define	GIFR   SELECTOR(0X5A)
#define SREG   SELECTOR(0x5F)

/*Timers Registers*/
#define TCCR0	SELECTOR(0x53)
#define OCR0	SELECTOR(0x5C)
#define TCNT0	SELECTOR(0x52)
#define TIMSK	SELECTOR(0x59)
#define TIFR	SELECTOR(0x58)

/*UART registers*/
#define UDR		SELECTOR(0x2C) //tx and rx data register
#define UCSRA	SELECTOR(0x2B)
#define UCSRB	SELECTOR(0x2A)
#define UCSRC   SELECTOR(0x40)
#define UBRRH   SELECTOR(0x40)
#define UBRRL   SELECTOR(0x29)

/*SPI registers*/
#define SPDR   SELECTOR(0x2F)
#define SPSR   SELECTOR(0x2E)
#define SPCR   SELECTOR(0x2D)

/*Timer0 registers*/
#define TCCR0   SELECTOR(0x53)
#define TCNT0   SELECTOR(0x52)
#define OCR0    SELECTOR(0x5C)
#define TIMSK	SELECTOR(0x59)
#define TIFR	SELECTOR(0x58)
/*Timer1 registers*/
#define TCNT1   SELECTOR_16(0x4C)  //timer1 counter register
#define ICR1    SELECTOR_16(0x46)  //Timer1 – Input Capture Register
#define TCCR1A	SELECTOR_16(0x4F)
#define CCR1B	SELECTOR_16(0x4E)
#define OCR1A	SELECTOR_16(0x4A)  //Timer1 – Output Compare Register

#endif /* REGISTERS_H_ */