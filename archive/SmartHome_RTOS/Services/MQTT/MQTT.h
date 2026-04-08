/*
 * MQTT.h
 *
 * Created: 8/29/2021 1:10:44 PM
 *  Author: MahmoudH
 */ 


#ifndef MQTT_H_
#define MQTT_H_



/*
	Assumptions:
	1- sizes (VAR+PAL) < 128 byte -> RL is one byte
	2- QoS = 0
*/

void sendUTFStr(u8* str);
void MQTT_Connect(u8* id);
void MQTT_Publish(u8* topic, u8* msgPtr, u8 msgLen);
void MQTT_Subscribe(u8* topic);

#endif /* MQTT_H_ */