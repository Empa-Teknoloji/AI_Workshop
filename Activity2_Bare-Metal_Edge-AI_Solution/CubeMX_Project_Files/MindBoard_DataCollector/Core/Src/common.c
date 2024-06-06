#include "common.h"
#include "myESP32AT.h"

#define MQTT_DATA_BUFF_SIZE                     1
#define MQTT_DATA_PACKET_BUFF_SIZE              400
EMPA_SectionTypeDef EMPA_Section = EMPA_SECTION_SENSOR;

volatile FlagStatus  mqtt_timer_en = RESET;
volatile  uint8_t  mqtt_timer = 0;
extern char mqttPacketBuffer[MQTT_DATA_PACKET_BUFF_SIZE];
char mqttDataBuffer[MQTT_DATA_BUFF_SIZE];

int cnt_callback = 0;
int cnt_uart_callback = 0;



/*TIMER MODULE*/
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//	  if (htim->Instance == TIM16)							// Each 1 second
//	  {
//		if(mqtt_timer_en == SET)
//		{
//		  mqtt_timer++;
//		}
//		else
//		{
//		  mqtt_timer = 0;
//		}
//
//	  }
//}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    cnt_callback++;

  if(huart->Instance == UART_INST)
  {
	cnt_uart_callback++;
    UART_MqttCallbackPacketCapture(huart, mqttDataBuffer, mqttPacketBuffer);

  }
}
