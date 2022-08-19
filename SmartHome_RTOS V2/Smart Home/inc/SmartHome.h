/*
 * SmartHome.h
 *
 * Created: 01/09/2021 08:53:52 
 *  Author: Administrator
 */ 


#ifndef SMARTHOME_H_
#define SMARTHOME_H_








#define PASS_NUM			1
#define PASS_LENGTH			5

/*Event group 1*/
#define E_CTempUpdate				(1<<0)
#define	E_ThresholdTempUpdate		(1<<1)
#define E_CMoistUpdate				(1<<2)
#define	E_CHUMUpdate				(1<<3)
#define	E_CLightUpdate				(1<<4)
#define	E_FireDetect				(1<<5)
#define	E_FireAlarmDisp				(1<<6)
#define	E_GetPassDisp				(1<<7)
#define	E_TempConfigDisp			(1<<8)
#define E_MainDisp					(1<<9)
#define E_TempCheck					(1<<10)
#define E_MoistCheck				(1<<11)
#define E_lightCheck				(1<<12)
#define E_UIDisp					(1<<13)
#define E_SITDisp					(1<<14)

/*Event group 2*/
#define E_MoistConigDisp			(1<<0)
#define E_lightConigDisp			(1<<1)
#define E_GetMoist					(1<<2)
#define E_GetLight					(1<<3)
#define E_GetTemp					(1<<4)







#define MAIN_L1				(u8*)"soil moist=   % "
#define MAIN_L2				(u8*)"Light=   %      "
#define MAIN_L3				(u8*)"Humid=     %RH  "
#define MAIN_L4				(u8*)"temp=      Deg.c"

#define ALARM_L1			(u8*)"****************"
#define ALARM_L2			(u8*)"*** WARNING ****"
#define ALARM_L3			(u8*)"*FIRE DETECTED *"
#define ALARM_L4			(u8*)"****************"

#define UI_L1				(u8*)"1-Open Door     "
#define UI_L2				(u8*)"2-Setting       "
#define UI_L3				(u8*)"                "
#define UI_L4				(u8*)"#-Return to main"

#define SETTING_L1			(u8*)"1-Temp  Config  "
#define	SETTING_L2			(u8*)"2-LDR   Config  "
#define	SETTING_L3			(u8*)"3-Moist Config  "
#define	SETTING_L4			(u8*)"#-Return to main"

#define Getpass_L1			(u8*)"Enter Password  "
#define	Getpass_L2			(u8*)"                "
#define	Getpass_L3			(u8*)"*-OK            "
#define	Getpass_L4			(u8*)"#-Return to main"

#define GetTemp_L1			(u8*)"New temperature "
#define	GetTemp_L2			(u8*)"                "
#define	GetTemp_L3			(u8*)"*-OK            "
#define	GetTemp_L4			(u8*)"#-Return to main"

#define GetMoist_L1			(u8*)"New Moist       "
#define	GetMoist_L2			(u8*)"                "
#define	GetMoist_L3			(u8*)"*-OK            "
#define	GetMoist_L4			(u8*)"#-Return to main"

#define GetLight_L1			(u8*)"New light cutoff"
#define	GetLight_L2			(u8*)"                "
#define	GetLight_L3			(u8*)"*-OK            "
#define	GetLight_L4			(u8*)"#-Return to main"




/* LCD DISPLAY STATES */
typedef enum
{
	MAIN_STATE,
	CONFIG_STATE,
	GET_PASS_STATE,
	ALARM_STATE,
	UI_STATE,
	TEMP_CONFIG_STATE,
	LIGHT_CUTOFF_STATE,
	MOIST_CONFIG_STATE,
	
}SysState;

typedef struct 
{
	u8			C_TEMP[15];
	u8			T_TEMP[15];
	u8			C_HUM[15];
	u8			C_MOIST;
	u8			T_MOIST;
	u8			C_LDR;
	u8			T_LDR;
	SysState	LCDState;
}SysVar;



//smart home tasks
void	Project_Init		(void);

//
void	T_DisplayData		(void *pvinitData);

//fire alarm tasks
void	T_FireDetect		(void *pvinitData);
void	T_FireAlarmAction	(void *pvinitData);

//temp control tasks
void	T_TempAction		(void *pvinitData);
void	T_TempRead			(void *pvinitData);
// SOIL IRRIGATION and OUTER LIGHT Control 
void	T_SOIL_Input		(void* pvInitData);
void	T_LIGHT_Input		(void* pvInitData);
void	T_SOIL_Action		(void* pvInitData);
void	T_LIGHT_Action		(void* pvInitData);

// Security system tasks
void	T_PasswordGet		(void *pvinitData);void	T_PasswordCheck		(void *pvinitData);void	T_DoorAction		(void *pvinitData);

// User Input task
void T_UserInput			(void *pvinitData);
void T_UIDOOR				(void *pvinitData);
void T_UISETTING			(void *pvinitData);

// System setting
void T_SystemModify			(void *pvinitData);


// TV,INNER LIGHTS CONTROL
void IOTcharinput(void *pvinitData);



#endif /* SMARTHOME_H_ */