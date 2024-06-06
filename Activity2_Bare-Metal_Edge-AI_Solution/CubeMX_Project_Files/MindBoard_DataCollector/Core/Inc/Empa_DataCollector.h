/*
 * Empa_DataCollector.h
 *
 *  Created on: May 19, 2024
 *      Author: bilalk
 */

#ifndef INC_EMPA_DATACOLLECTOR_H_
#define INC_EMPA_DATACOLLECTOR_H_

#include "custom_bus.h"
#include "common.h"
#include "main.h"

#include "EMPA_Sensor.h"
typedef enum {
    ONLY_ACCELEROMETER,
    ONLY_GYRO,
    ACCELEROMETER_AND_GYRO
} SensorMode;

void fill_accelerator_and_gyro_buffer(void);
void fill_accelerator_buffer(void);
void fill_gyro_buffer(void);

#endif /* INC_EMPA_DATACOLLECTOR_H_ */
