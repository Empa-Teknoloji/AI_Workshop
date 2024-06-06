#include "EMPA_MqttAws.h"
#include "myESP32AT.h"

#define FW_VERSION_MAJOR                        (uint8_t)0
#define FW_VERSION_MINOR                        (uint8_t)1
#define FW_VERSION_PATCH                        (uint8_t)0

#define ENABLE_ESP32                        	SET

APP_mainStateTypdef mainState = STATE_MQTT_INIT;
extern EMPA_SectionTypeDef EMPA_Section;

char mqttPacketBuffer[MQTT_DATA_PACKET_BUFF_SIZE] = {0};
extern volatile FlagStatus flag_mqtt_rx_done;
extern volatile uint8_t capturedPacketDataSize;
extern char mqttDataBuffer[1];
FlagStatus volatile  flag_waitMqttData = RESET;

uint8_t cntTryFunc = 0;
MQTT_MsgDataTypeDef             mqttMsgData;
MQTT_MacIdTypeDef               headBoard;
MQTT_FwVersionDataTypeDef       fwVersion = {FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_PATCH};



#define TOPIC_PUB_DATA			"EMPA"
#define ENABLE_ESP32            SET

char topicBuffer[256];

extern int32_t temperature;
extern int32_t humidity;
extern char DEVICE_ID[];
long int counter_mqtt = 0;

MQTT_Config mqttConfig = {
    .mqttPacketBuffer = mqttPacketBuffer,
    .mode_wifi = STATION_MODE,
    .OSC_enable = SC_DISABLE,
    .wifiID = "Airties_tc",
    .wifiPassword = "tcsjklm.1",
    .timezone = 3,
    .mode_mqtt = MQTT_TLS_1,
    .clientID = "",
    .username = "",
    .mqttPassword = "",
    .keepAlive = 300,
    .cleanSession = CLS_1,
    .qos = QOS_0,
    .retain = RTN_0,
    .brokerAddress = "",
    .reconnect = 0,
	.subtopic = "EMPA_SUB_TOPIC",
	.pubtopic = "EMPA_PUB_TOPIC"
};

void MY_MqttAwsProcess(void)
{
	EMPA_Section = EMPA_SECTION_MQTT;
	HAL_GPIO_WritePin(MDL_EN_GPIO_Port, MDL_EN_Pin, ENABLE_ESP32);
	//HAL_Delay(3000);
	while(ENABLE_ESP32)
	{
		switch(mainState)
		{
		case STATE_MQTT_COLLECT :
			EMPA_SensorProcess();
			if(flag_mqtt_rx_done == 1)
			{
				mainState = STATE_MQTT_SUB_RX_MSG;
			}
			else
			{
				mainState = STATE_MQTT_PUB_TX_MSG;
			}
			break;
			case STATE_MQTT_INIT :

			if (MQTT_Init(&mqttConfig) == FUNC_SUCCESSFUL) {
				LED_Mqttconnected(SET);
				if (Wifi_MqttSubInit(mqttPacketBuffer, mqttConfig.subtopic, QOS_0)
						== FUNC_OK) {
					cntTryFunc = 0;
					mainState = STATE_MQTT_COLLECT;
				} else {
					cntTryFunc++;
					mainState =
							(cntTryFunc == MAX_TRY_FUNC) ?
									STATE_MQTT_ERROR : STATE_MQTT_INIT;
				}
			} else {
				mainState = STATE_MQTT_INIT;
			}
			break;
			case STATE_MQTT_IDLE :
				//SleepMode();
				HAL_Delay(1000);
				break;
			case STATE_MQTT_SUB_RX_MSG :

				if(flag_waitMqttData)
				{
					Wifi_WaitMqttData();
					flag_waitMqttData = RESET;
				}
				if(flag_mqtt_rx_done == SET)
				{
					LED_MqttRXBlink();
					flag_mqtt_rx_done = RESET;
					memset(mqttMsgData.data, 0, strlen(mqttMsgData.data));
					UART_MqttPacketParser(&mqttMsgData, mqttPacketBuffer, strlen(mqttPacketBuffer));
				    printf("\nReceived message is : %s\r\n",mqttMsgData.data);
					mainState = STATE_MQTT_SUB_RX_MSG;
				}
				counter_mqtt--;
				if(counter_mqtt == 0)
					mainState = STATE_MQTT_COLLECT;
				break;
			case STATE_MQTT_PUB_TX_MSG :
				LED_MqttTXBlink();
				  memset(mqttPacketBuffer, 0, strlen(mqttPacketBuffer));
				  char dizi[100] = {0};
				  sprintf(dizi, "\n%s -> Temperature : %0.2f C Humidity : %0.2f\r\n",
						  DEVICE_ID,temperature/1000.0f, humidity/1000.0f);
				  Wifi_MqttPubRaw2(mqttPacketBuffer, mqttConfig.pubtopic, strlen(dizi), dizi, QOS_0, RTN_0, POLLING_MODE);
				  flag_waitMqttData = SET;
				  counter_mqtt = 16000000;
			      mainState = STATE_MQTT_SUB_RX_MSG;
				break;
			case STATE_MQTT_ERROR :
				//Error Log
				cntTryFunc = 0;
				mainState = STATE_MQTT_IDLE;
				break;
		}
	}
}


