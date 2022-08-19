/*
 * DIO.h
 *
 * Created: 8/18/2021 11:18:14 PM
 *  Author: Mohamed
 */ 


#ifndef DIO_H_
#define DIO_H_


#define PORTA_OFFSET  0
#define PORTB_OFFSET  8
#define PORTC_OFFSET  16
#define PORTD_OFFSET  24

typedef enum{PAU0=PORTA_OFFSET,PAU1,PAU2,PAU3,PAU4,PAU5,PAU6,PAU7}PortA;
typedef enum{PBU0=PORTB_OFFSET,PBU1,PBU2,PBU3,PBU4,PBU5,PBU6,PBU7}PortB;
typedef enum{PCU0=PORTC_OFFSET,PCU1,PCU2,PCU3,PCU4,PCU5,PCU6,PCU7}PortC;
typedef enum{PDU0=PORTD_OFFSET,PDU1,PDU2,PDU3,PDU4,PDU5,PDU6,PDU7}PortD;
	
	
typedef enum{LOW,HIGH}LOGIC;
typedef enum{INPUT,OUTPUT}DIRECTION;

	
/*for single bit in the port*/
void writePin(u8 pinNo, u8 logic);
void pinDirection(u8 pinNo, u8 direction);
BOOL readPin(u8 pinNo);

/*for custom number of bits*/
void writePins(volatile u8* port,u8 mask, u8 logic);
void pinsDirection(volatile u8* ddr,u8 mask, u8 direction);
/*for all bits in the port*/
void writeAllPin(volatile u8* port,u8 logic);
void allPinsDirection(volatile u8* ddr, u8 direction);

	
#endif /* DIO_H_ */