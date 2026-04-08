/*
 * SmartHome.h
 *
 * Created: 01/09/2021 08:53:52 
 *  Author: Administrator
 */ 


#ifndef SMARTHOME_H_
#define SMARTHOME_H_





void	Get_Password		(u8*);
void	Check_Password		(u8*);
void	AnalogSensors		(void);
void	FireAlarm			(void);
void	turnOnFireAlarm		(void);
void	temp				(void);
void	IOTcharinput		(u8 data);

#define PASS_NUM			1
#define PASS_LENGTH			5

//smart home tasks
void	Project_Init		(void);


//fire alarm tasks
void	T_FireDetect		(void *pvinitData);
void	T_FireAlarmAction	(void *pvinitData);

//temp control tasks
void	T_TempAction		(void *pvinitData);
void	T_TempRead			(void *pvinitData);

// SOIL IRRIGATION and OUTER LIGHT Control 
void	T_SOIL_LIGHT_Input	(void* pvInitData);
void	T_SOIL_LIGHT_Action	(void* pvInitData);

// Security system tasks
void	T_PasswordGet		(void *pvinitData);void	T_PasswordCheck		(void *pvinitData);void	T_DoorAction		(void *pvinitData);


#endif /* SMARTHOME_H_ */