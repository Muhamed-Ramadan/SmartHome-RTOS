/*
 * KEYPAD.c
 *
 * Created: 8/20/2021 6:03:58 PM
 *  Author: ehab2_phjirwi
 */ 
#include "dataTypes.h"
#include "registers.h"
#include "DIO.h"
#include "KEYPAD.h"


#if KEYPAD_COL==4
u8 key_CHAR[KEYPAD_ROW][KEYPAD_COL]=
{
	{'7','8','9','/'},
	{'4','5','6','*'},
	{'1','2','3','-'},
	{'=','0','.','+'}
};
#elif KEYPAD_COL==3
u8 key_CHAR[KEYPAD_ROW][KEYPAD_COL]=
{
	{'1','2','3'},
	{'4','5','6'},
	{'7','8','9'},
	{'*','0','#'}
};

#endif



void KEYPAD_Init(void)
{
	pinsDirection(&ROW_DDR,ROW_PINS,OUTPUT);
	writePins(&ROW_PORT,ROW_PINS,HIGH);	
	pinsDirection(&COL_DDR,COL_PINS,INPUT);
	writePins(&COL_PORT,COL_PINS,HIGH);
	
}



u8 GetKey(void)
{
	u8 key=0;
	u8 row=0,col=0;
	
	for (row=FIRSTOFROW;row<=LASTOFROW;row++)
	{
		writePin(row,LOW);
		for (col=FIRSTOFCOL;col<=LASTOFCOL;col++)
		{
			if (!readPin(col))
			{
				while(!readPin(col));
				key=key_CHAR[row-ROWOFFSET][col-COLOFFSET];
				return key;
			}	
		}
		writePin(row,HIGH);
	}	
	return key;
}