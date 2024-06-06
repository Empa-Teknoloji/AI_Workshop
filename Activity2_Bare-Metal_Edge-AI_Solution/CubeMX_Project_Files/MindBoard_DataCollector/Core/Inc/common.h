
#ifndef INC_COMMON_H_
#define INC_COMMON_H_

#include "main.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

//Defines
#define EMPA_DataCollector
//#define EMPA_MqttAws
#define EMPA_Sensor


extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim1;


#define UART_WIFI                               huart2
#define UART_INST								USART2

#define TMR_ONESECOND                           &htim1

#define WIFI_SSID                               "EMPA_ARGE_4G"
#define WIFI_PASSWORD                           "Empa1982"


#define MQTT_CLIENT_ID                          ""
#define MQTT_USERNAME                           ""
#define MQTT_PASSWORD                           ""
#define MQTT_KEEP_ALIVE_TIME                    60                      // in seconds
#define MQTT_BROKER_ADDRESS                     "cf25e86f3af44eda80893f20d071f69c.s1.eu.hivemq.cloud"



typedef enum
{
	FALSE = 0,
	TRUE  = 1
}bool;

typedef enum
{
  EMPA_SECTION_SENSOR          = 0x00U,
  EMPA_SECTION_NANOEDGEAI,
  EMPA_SECTION_MQTT

} EMPA_SectionTypeDef;





#endif /* INC_COMMON_H_ */
