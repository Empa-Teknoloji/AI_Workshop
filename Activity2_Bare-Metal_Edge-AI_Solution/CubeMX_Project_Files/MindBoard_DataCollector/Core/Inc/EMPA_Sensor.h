/*
 * EMPA_Sensor.h
 *
 *  Created on: May 19, 2024
 *      Author: bilalk
 */

#ifndef INC_EMPA_SENSOR_H_
#define INC_EMPA_SENSOR_H_

#include "custom_bus.h"
#include "ism330is.h"
#include "main.h"
typedef struct {
	int32_t x;
	int32_t y;
	int32_t z;
} acc;

typedef struct {
	int32_t x;
	int32_t y;
	int32_t z;
} gyro;

typedef struct {
	acc acc;
	gyro gyro;
} imu_data;

uint8_t EMPA_SensorInit(void);
uint8_t EMPA_SensorProcess(void);
void 	EMPA_SensorPrint(void);
uint8_t sht40_sensor_process(int32_t* temp, int32_t* hum);
uint8_t ism330_sensor_process(imu_data *sensor_data);
uint8_t imp34_sensor_process(void);

#endif /* INC_EMPA_SENSOR_H_ */
