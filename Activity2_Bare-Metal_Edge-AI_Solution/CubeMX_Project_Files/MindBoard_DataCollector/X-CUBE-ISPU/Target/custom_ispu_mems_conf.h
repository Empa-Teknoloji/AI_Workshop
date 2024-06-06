/**
  ******************************************************************************
  * @file    custom_ispu_mems_conf.h
  * @author  MEMS Software Solutions Team
  * @brief   This file contains definitions of the MEMS components bus interfaces for custom boards
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CUSTOM_ISPU_MEMS_CONF_H
#define CUSTOM_ISPU_MEMS_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"
#include "custom_bus.h"
#include "custom_errno.h"

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

#define USE_CUSTOM_MOTION_SENSOR_ISM330IS_0  0U

#define CUSTOM_ISM330IS_0_I2C_INIT BSP_I2C1_Init
#define CUSTOM_ISM330IS_0_I2C_DEINIT BSP_I2C1_DeInit
#define CUSTOM_ISM330IS_0_I2C_READREG BSP_I2C1_ReadReg
#define CUSTOM_ISM330IS_0_I2C_WRITEREG BSP_I2C1_WriteReg

#ifdef __cplusplus
}
#endif

#endif /* CUSTOM_ISPU_MEMS_CONF_H*/
