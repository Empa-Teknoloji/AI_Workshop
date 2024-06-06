/*
 * EMPA_DataCollector.c
 *
 *  Created on: May 19, 2024
 *      Author: bilalk
 */
#include "Empa_DataCollector.h"

extern uint8_t flag_working;
extern uint8_t counter_timer;
extern uint16_t counter_state;
extern imu_data imu_sensor_data;
uint32_t pins_led[3]  = { LED6_Pin, LED7_Pin, LED8_Pin};
extern Flag_DataCollector;

extern TIM_HandleTypeDef htim1;

SensorMode sensor_mode = ONLY_ACCELEROMETER;
extern EMPA_SectionTypeDef EMPA_Section;

int32_t imu_sensor_array[768];
int32_t gyro_sensor_array[384];
int32_t acc_sensor_array[384];

void EMPA_DataCollectorProcess(void) {
	EMPA_Section = EMPA_SECTION_NANOEDGEAI;


	if ((HAL_GPIO_ReadPin(USER_GPIO_Port, USER_Pin) == GPIO_PIN_RESET)
			&& (flag_working == 0)) {
		HAL_TIM_Base_Start_IT(&htim1);
		while (HAL_GPIO_ReadPin(USER_GPIO_Port, USER_Pin) == GPIO_PIN_RESET)
			;
		if (counter_timer > 2) {
			if (counter_state == 0) {
				counter_state += 2;
			} else
				counter_state--;
			sensor_mode = counter_state;
			HAL_GPIO_WritePin(GPIOB, pins_led[counter_state], GPIO_PIN_RESET);
			HAL_Delay(500);
			HAL_GPIO_WritePin(GPIOB, pins_led[counter_state], GPIO_PIN_SET);
			HAL_Delay(500);
			HAL_TIM_Base_Stop_IT(&htim1);
			counter_timer = 0;
			flag_working = 1;
		} else {
			HAL_TIM_Base_Stop_IT(&htim1);
			counter_timer = 0;
			HAL_GPIO_WritePin(GPIOB, LED6_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, LED7_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, LED8_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, pins_led[counter_state], GPIO_PIN_SET);
			if (counter_state == 0) {
				HAL_GPIO_WritePin(GPIOB, pins_led[counter_state + 2],
						GPIO_PIN_RESET);
			} else {
				HAL_GPIO_WritePin(GPIOB, pins_led[counter_state - 1],
						GPIO_PIN_RESET);
			}

		}
	}
	while (flag_working == 1) {
		switch (sensor_mode) {
		case ONLY_GYRO:
			fill_gyro_buffer();
			for (int i = 0; i < 384; i++) {
				printf("%ld", gyro_sensor_array[i]);
				if (i != 383) {
					printf(" ");
				}
			}
			printf(" \n\r");

			break;
		case ONLY_ACCELEROMETER:

			fill_accelerator_buffer();
 			for (int i = 0; i < 384; i++) {
				printf("%ld", acc_sensor_array[i]);
				if (i != 383) {
					printf(" ");
				}
			}
			printf("\r\n");

			break;
		case ACCELEROMETER_AND_GYRO:
			fill_accelerator_and_gyro_buffer();
			for (int i = 0; i < 768; i++) {
				printf("%ld", imu_sensor_array[i]);
				if (i != 767) {
					printf(" ");
				}
			}
			printf(" \n\r");

			break;
		default:
			printf("ERROR.\n");
			break;
		}
	}
}


void fill_accelerator_buffer(void)
{
	  for(int i = 0; i < 128; i ++)
	  {
		  ism330_sensor_process(&imu_sensor_data);
		  acc_sensor_array[i*3] = (float)imu_sensor_data.acc.x;
		  acc_sensor_array[i*3+1] = (float)imu_sensor_data.acc.y;
		  acc_sensor_array[i*3+2] = (float)imu_sensor_data.acc.z;
		  HAL_Delay(8);
	  }
}

void fill_gyro_buffer(void)
{
	  for(int i = 0; i < 128; i ++)
	  {
		  ism330_sensor_process(&imu_sensor_data);
		  gyro_sensor_array[i*3] = imu_sensor_data.gyro.x;
		  gyro_sensor_array[i*3+1] = imu_sensor_data.gyro.y;
		  gyro_sensor_array[i*3+2] = imu_sensor_data.gyro.z;
		  HAL_Delay(8);
	  }
}
void fill_accelerator_and_gyro_buffer(void)
{
	  for(int i = 0; i < 128; i ++)
	  {
		  ism330_sensor_process(&imu_sensor_data);
		  imu_sensor_array[i*6] = 	imu_sensor_data.acc.x;
		  imu_sensor_array[i*6+1] = imu_sensor_data.acc.y;
		  imu_sensor_array[i*6+2] = imu_sensor_data.acc.z;
		  imu_sensor_array[i*6+3] = imu_sensor_data.gyro.x;
		  imu_sensor_array[i*6+4] = imu_sensor_data.gyro.y;
		  imu_sensor_array[i*6+5] = imu_sensor_data.gyro.z;
		  HAL_Delay(8);
	  }
}



