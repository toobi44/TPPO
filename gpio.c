/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */
#include "tim.h"
#include "user_g_Ram.h"
#include "usart.h"

const uint8_t display_array_one_line[][16] = {
	{"   œ◊ ELSY FC   "},			//0
	{" ¬ÂÒËˇ œŒ:     "},			//1
	{"                "},			//2
	{"ERROR:HARD FAULT"},			//3
	{"   ERROR:FPU    "},			//4
	{"PC:             "},			//5
	{"LR:             "},			//6
	{" ERROR:MemManage"},			//7
	{" ERROR:BusFault "},			//8
	{"ERROR:UsageFault"},			//9
	{" ERROR:NMI      "},			//10
	{"  œ≈–≈«¿√–”« ¿  "},			//11
	{"   ”—“–Œ…—“¬¿   "},			//12
	{"Œ“—”“—“¬»≈ —¬ﬂ«»"},			//13
	{"   — œÀ¿“Œ… IO  "},			//14
	{" —¡Œ… œÀ¿“€ TSPO"},			//15
};

DINP_T DigInp[3] = {
	{/*.ButStr= BUT_STR_INIT_DEF,*/ 				.GPIO_Port = BUT_C1_GPIO_Port, 	.GPIO_Pin = BUT_C1_Pin,	/*.Inv_Flag = false*/},
	{/*.ButStr= BUT_STR_INIT_DEF,*/ 				.GPIO_Port = BUT_C0_GPIO_Port, 	.GPIO_Pin = BUT_C0_Pin,	/*.Inv_Flag = false*/},
	{/*.ButStr= BUT_STR_INIT_DEF,*/ 				.GPIO_Port = BUT_C2_GPIO_Port, 	.GPIO_Pin = BUT_C2_Pin,	/*.Inv_Flag = false*/},
};
					  //DB C
BUT_T BUT_Inp[4][3] = {
	{{.ButStr= BUT_STR_INIT_DEF}, {.ButStr= BUT_STR_INIT_DEF}, {.ButStr= BUT_STR_INIT_DEF},},
	{{.ButStr= BUT_STR_INIT_DEF}, {.ButStr= BUT_STR_INIT_DEF}, {.ButStr= BUT_STR_INIT_DEF},},
	{{.ButStr= BUT_STR_INIT_DEF}, {.ButStr= BUT_STR_INIT_DEF}, {.ButStr= BUT_STR_INIT_DEF},},
	{{.ButStr= BUT_STR_INIT_DEF}, {.ButStr= BUT_STR_INIT_DEF}, {.ButStr= BUT_STR_INIT_DEF},},
};
// œÓˇ‰ÓÍ Í‡Í Ì‡ ÒıÂÏÂ

DOUT_T DigOut[4] = {
	{.GPIO_Port= LCD_DB4_GPIO_Port,			.GPIO_Pin = LCD_DB4_Pin,				/*.State = false*/},
	{.GPIO_Port= LCD_DB5_GPIO_Port, 		.GPIO_Pin = LCD_DB5_Pin, 				/*.State = false*/},
	{.GPIO_Port= LCD_DB6_GPIO_Port, 		.GPIO_Pin = LCD_DB6_Pin, 				/*.State = false*/},
	{.GPIO_Port= LCD_DB7_GPIO_Port, 		.GPIO_Pin = LCD_DB7_Pin, 				/*.State = false*/},
};
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, CS_LED_Pin|CS_FLASH_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, PULT_DIR_Pin|LCD_DB5_Pin|LCD_DB6_Pin|LCD_DB7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_EEPROM_GPIO_Port, CS_EEPROM_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_EN_Pin|LCD_DB4_Pin|LCD_PWR_Pin|LCD_RS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PAPin PAPin PAPin PAPin
                           PAPin PAPin */
  GPIO_InitStruct.Pin = CS_LED_Pin|PULT_DIR_Pin|CS_FLASH_Pin|LCD_DB5_Pin
                          |LCD_DB6_Pin|LCD_DB7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin PBPin */
  GPIO_InitStruct.Pin = CS_EEPROM_Pin|LCD_EN_Pin|LCD_DB4_Pin|LCD_RS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin */
  GPIO_InitStruct.Pin = BUT_C0_Pin|BUT_C1_Pin|BUT_C2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = LCD_PWR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_PWR_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */
uint16_t DB_OUT_cnt = 0;
uint16_t DB_tmp = 0;
uint16_t BUT_bit = 0;
uint16_t C_INP_cnt = 0;

float push_period_ = 0.05F;
float non_push_period_ = 0.05F;

void DIG_INP_Init(void)
{
	for (DB_OUT_cnt = 0; DB_OUT_cnt < 4; DB_OUT_cnt++)
	{
		HAL_GPIO_WritePin(DigOut[DB_OUT_cnt].GPIO_Port, DigOut[DB_OUT_cnt].GPIO_Pin, GPIO_PIN_SET);
		for (C_INP_cnt = 0; C_INP_cnt < 3; C_INP_cnt++)
		{
			BUT_Inp[DB_OUT_cnt][C_INP_cnt].ButStr.T_samp = &T_samp_FLOAT_Modbus;
			BUT_Inp[DB_OUT_cnt][C_INP_cnt].ButStr.push_period = &push_period_;
			BUT_Inp[DB_OUT_cnt][C_INP_cnt].ButStr.non_push_period = &non_push_period_;
		}
	}
}

//BUT_State_T BUT_State = {0};
GPIO_PinState gpio_state_tmp;
uint16_t INP_tmp_State = 0;

void DIG_INP_Update(void)
{
	for (DB_OUT_cnt = 0; DB_OUT_cnt < 4; DB_OUT_cnt++)		// √ÓÚÓ‚ËÏ ‚˚ıÓ‰˚ DB ÔÓÒÎÂ ‚ÓÁÏÓÊÌÓÈ ‡·ÓÚ˚ LCD
		HAL_GPIO_WritePin(DigOut[DB_OUT_cnt].GPIO_Port, DigOut[DB_OUT_cnt].GPIO_Pin, GPIO_PIN_SET);
	
	INP_tmp_State = 0;
	BUT_bit = 1;
	for (DB_OUT_cnt = 0; DB_OUT_cnt < 4; DB_OUT_cnt++)
	{
		HAL_GPIO_WritePin(DigOut[DB_OUT_cnt].GPIO_Port, DigOut[DB_OUT_cnt].GPIO_Pin, GPIO_PIN_RESET);				// ¬˚ÒÚ‡‚ËÎË DBx
		for (C_INP_cnt = 0; C_INP_cnt < 3; C_INP_cnt++)
		{
			gpio_state_tmp = HAL_GPIO_ReadPin(DigInp[C_INP_cnt].GPIO_Port, DigInp[C_INP_cnt].GPIO_Pin);
			OSL_but_state_monitor(gpio_state_tmp, &BUT_Inp[DB_OUT_cnt][C_INP_cnt].ButStr);
			if (BUT_Inp[DB_OUT_cnt][C_INP_cnt].ButStr.dur_state == BUT_PUSHED) INP_tmp_State |= BUT_bit;
			BUT_bit<<=1;
		}
		HAL_GPIO_WritePin(DigOut[DB_OUT_cnt].GPIO_Port, DigOut[DB_OUT_cnt].GPIO_Pin, GPIO_PIN_SET);			// —ÌˇÎË DBx
	}
	grMAIN.BUT_State.Var.all = INP_tmp_State;
}
/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
