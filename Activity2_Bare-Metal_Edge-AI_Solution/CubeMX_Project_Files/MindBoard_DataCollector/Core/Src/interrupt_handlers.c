/*
 * interrupt_handlers.c
 *
 *  Created on: May 17, 2024
 *      Author: bilalk
 */

#include "interrupt_handlers.h"

extern uint8_t counter_timer;
extern uint8_t flag_working;
extern uint16_t counter_state;
extern uint8_t AUDIO_MODE;
uint8_t imp34_cnt = 0;
extern volatile FlagStatus  mqtt_timer_en;
extern volatile  uint8_t  mqtt_timer;
extern EMPA_SectionTypeDef EMPA_Section;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM1)
	{
		if(EMPA_Section == EMPA_SECTION_NANOEDGEAI)
		{
			counter_timer++;
		}
		else if(EMPA_Section == EMPA_SECTION_MQTT)
		{
			if(mqtt_timer_en == SET)
			{
			  mqtt_timer++;
			}
			else
			{
			  mqtt_timer = 0;
			}
		}
	}
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == USER_Pin)
	{
		if(flag_working == 0)
		{
			counter_state++;
			if(counter_state>2)
				counter_state = 0;
		}
		else
		{
			flag_working = 0;
		}

	}
}



