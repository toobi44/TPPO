/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "BD_tim.h"
/* USER CODE END Includes */

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

/* USER CODE BEGIN Private defines */
extern TIMx_Params_T TIME_SOURCE;
extern TIMx_Params_T TIME_SOURCE_SLOW;
extern TIMx_Params_T MDB_INT_TIM;

extern TIM_Calc_Params_T TIME_CALC_TEST;
extern TIM_Calc_Params_T TIME_CALC_TEST_Delay;
extern TIM_Calc_Params_T				LCD_PWR_ON_DELAY;
extern TIM_Calc_Params_T				LCD_PWR_OFF_DELAY;
extern TIM_Calc_Params_T				DISPLAY_POWER_DELAY;

extern float T_samp_FLOAT_Modbus;
/* USER CODE END Private defines */

void MX_TIM1_Init(void);
void MX_TIM2_Init(void);
void MX_TIM3_Init(void);
void MX_TIM4_Init(void);

/* USER CODE BEGIN Prototypes */
void USER_TIME_SOURCE_FAST_Logical_Init(TIMx_Params_T *p);
void USER_TIME_SOURCE_SLOW_Logical_Init(TIMx_Params_T *p);
void USER_MDB_INT_TIMER_Logical_Init(TIMx_Params_T *p);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
