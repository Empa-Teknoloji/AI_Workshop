/**
  ******************************************************************************
  * @file    custom_ispu_mems_conf_app.h
  * @author  MEMS Software Solutions Team
  * @brief   Custom sensor configuration file
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
#ifndef CUSTOM_ISPU_MEMS_CONF_APP_H
#define CUSTOM_ISPU_MEMS_CONF_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/
#ifdef USE_CUSTOM_MOTION_SENSOR_ISM330IS_0
#if (USE_CUSTOM_MOTION_SENSOR_ISM330IS_0 == 1)
#ifndef CUSTOM_ACC_INSTANCE_0
#define CUSTOM_ACC_INSTANCE_0 CUSTOM_ISM330IS_0
#define CUSTOM_ACC_INSTANCE_NAME_0 "ISM330IS (DIL24)"
#define CUSTOM_ACC_INSTANCE_ODR_0 {10, 12.5, 26, 52, 104, 208, 416, 833, 1667, 3333, 6667} /* Hz */
#define CUSTOM_ACC_INSTANCE_FS_0 {4, 2, 4, 8, 16} /* g */
#endif /* CUSTOM_ACC_INSTANCE_0 */
#ifndef CUSTOM_GYR_INSTANCE_0
#define CUSTOM_GYR_INSTANCE_0 CUSTOM_ISM330IS_0
#define CUSTOM_GYR_INSTANCE_NAME_0 "ISM330IS (DIL24)"
#define CUSTOM_GYR_INSTANCE_ODR_0 {10, 12.5, 26, 52, 104, 208, 416, 833, 1667, 3333, 6667} /* Hz */
#define CUSTOM_GYR_INSTANCE_FS_0 {4, 250, 500, 1000, 2000} /* dps */
#endif /* CUSTOM_GYR_INSTANCE_0 */
#endif /* USE_CUSTOM_MOTION_SENSOR_ISM330IS_0 */
#endif /* USE_CUSTOM_MOTION_SENSOR_ISM330IS_0 */

#ifdef USE_CUSTOM_MOTION_SENSOR_LSM6DSO16IS_0
#if (USE_CUSTOM_MOTION_SENSOR_LSM6DSO16IS_0 == 1)
#ifndef CUSTOM_ACC_INSTANCE_0
#define CUSTOM_ACC_INSTANCE_0 CUSTOM_LSM6DSO16IS_0
#define CUSTOM_ACC_INSTANCE_NAME_0 "LSM6DSO16IS (DIL24)"
#define CUSTOM_ACC_INSTANCE_ODR_0 {10, 12.5, 26, 52, 104, 208, 416, 833, 1667, 3333, 6667} /* Hz */
#define CUSTOM_ACC_INSTANCE_FS_0 {4, 2, 4, 8, 16} /* g */
#endif /* CUSTOM_ACC_INSTANCE_0 */
#ifndef CUSTOM_GYR_INSTANCE_0
#define CUSTOM_GYR_INSTANCE_0 CUSTOM_LSM6DSO16IS_0
#define CUSTOM_GYR_INSTANCE_NAME_0 "LSM6DSO16IS (DIL24)"
#define CUSTOM_GYR_INSTANCE_ODR_0 {10, 12.5, 26, 52, 104, 208, 416, 833, 1667, 3333, 6667} /* Hz */
#define CUSTOM_GYR_INSTANCE_FS_0 {4, 250, 500, 1000, 2000} /* dps */
#endif /* CUSTOM_GYR_INSTANCE_0 */
#endif /* USE_CUSTOM_MOTION_SENSOR_LSM6DSO16IS_0 */
#endif /* USE_CUSTOM_MOTION_SENSOR_LSM6DSO16IS_0 */

#ifdef __cplusplus
}
#endif

#endif /* CUSTOM_ISPU_MEMS_CONF_APP_H */
