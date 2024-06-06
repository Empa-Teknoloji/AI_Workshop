/*
 * EMPA_MqttAws.h
 *
 *  Created on: May 19, 2024
 *      Author: bilalk
 */

#ifndef INC_EMPA_MQTTAWS_H_
#define INC_EMPA_MQTTAWS_H_

#include "myESP32AT.h"

#define MAX_TRY_FUNC 3

typedef enum
{
	STATE_MQTT_IDLE,
	STATE_MQTT_INIT,
	STATE_MQTT_SUB_RX_MSG,
	STATE_MQTT_PUB_TX_MSG,
	STATE_MQTT_ERROR,
	STATE_MQTT_COLLECT

}APP_mainStateTypdef;


void MY_MqttAwsProcess(void);


#endif /* INC_EMPA_MQTTAWS_H_ */
