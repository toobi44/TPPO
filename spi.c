/**
  ******************************************************************************
  * @file    spi.c
  * @brief   This file provides code for the configuration
  *          of the SPI instances.
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
#include "spi.h"

/* USER CODE BEGIN 0 */
#include "user_g_Ram.h"

__IO uint8_t SPI_data_send_8 [100],
             SPI_data_get_8 [100];

SPIx_Params_T SPI_Params;

SPI_DEVICEx_Params_T *SPI_TASK_EEPROM_FLASH[2];		// FLASH
SPI_DEVICEx_Params_T *SPI_TASK_LED[1];						// LED

SPI_TASKx_T SPI_TASKS[2];
/* USER CODE END 0 */

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi1_tx;

/* SPI1 init function */
void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* SPI1 DMA Init */
    /* SPI1_RX Init */
    hdma_spi1_rx.Instance = DMA1_Channel2;
    hdma_spi1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_spi1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spi1_rx.Init.Mode = DMA_NORMAL;
    hdma_spi1_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_spi1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(spiHandle,hdmarx,hdma_spi1_rx);

    /* SPI1_TX Init */
    hdma_spi1_tx.Instance = DMA1_Channel3;
    hdma_spi1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_spi1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spi1_tx.Init.Mode = DMA_NORMAL;
    hdma_spi1_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_spi1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(spiHandle,hdmatx,hdma_spi1_tx);

  /* USER CODE BEGIN SPI1_MspInit 1 */

  /* USER CODE END SPI1_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

    /* SPI1 DMA DeInit */
    HAL_DMA_DeInit(spiHandle->hdmarx);
    HAL_DMA_DeInit(spiHandle->hdmatx);
  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void USER_SPI_EEPROM_INIT(SPI_DEVICEx_Params_T *p, SPIx_Params_T *SPIx)
{
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	p->data_mode = SPI_MODE_RX_TX;
	p->CS_GPIO_Port	= CS_EEPROM_GPIO_Port;
	p->CS_GPIO_Pin	= CS_EEPROM_Pin;
	p->GPIO_Active_Level = GPIO_PIN_RESET;
	p->data_send = &SPI_data_send_8[0];
	p->data_get = &SPI_data_get_8[0];
	p->Device_X_func_IT = OSL_EEPROM_Memory_Update;
	p->Device_X_func_CB = OSL_EEPROM_Callback;
	p->hspi_x = SPIx->hspi_x;
	
	p->Reg_Params.DataSize = SPI_DATASIZE_8BIT;
	p->Reg_Params.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	p->Reg_Params.CLKPolarity = SPI_POLARITY_LOW;
	p->Reg_Params.CLKPhase = SPI_PHASE_1EDGE;
	
	// Changing params (must be inited, may be reinited by developer):	
	p->Enable_Flag = true;
	p->Num_packets = NO_MATTER;
}


//SPI FLASH{
void USER_EXT_FLASH_INIT(SPI_DEVICEx_Params_T *p, SPIx_Params_T *SPIx)
{
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	p->data_mode = SPI_MODE_RX_TX;
	p->CS_GPIO_Port	= CS_FLASH_GPIO_Port;
	p->CS_GPIO_Pin	= CS_FLASH_Pin;
	p->GPIO_Active_Level = GPIO_PIN_RESET;
	p->data_send = &SPI_data_send_8[0];
	p->data_get = &SPI_data_get_8[0];
	p->Device_X_func_IT = OSL_EXT_FLASH_Memory_Update;
	p->Device_X_func_CB = OSL_EXT_FLASH_Callback;
	p->hspi_x = SPIx->hspi_x;
	
	p->Reg_Params.DataSize = SPI_DATASIZE_8BIT;
	p->Reg_Params.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	p->Reg_Params.CLKPolarity = SPI_POLARITY_LOW;
	p->Reg_Params.CLKPhase = SPI_PHASE_1EDGE;
	
	// Changing params (must be inited, may be reinited by developer):	
	p->Enable_Flag = true;
	p->Num_packets = NO_MATTER;
}
//SPI FLASH}

//SPI LED{
static SPI_DEVICE_X_RETURN_STATE_T USER_SPI_LED_IT(void);
//static void USER_SPI_LED_CB(void);

SPI_DEVICEx_Params_T SPI_LED = {
/*data_mode = */											SPI_MODE_TX,\
/*CS_GPIO_Port = */										CS_LED_GPIO_Port,\
/*CS_GPIO_Pin = */										CS_LED_Pin,\
/*GPIO_Active_Level = */							GPIO_PIN_RESET,\
/*data_send = */											&SPI_data_send_8[0],\
/*data_get = */												&SPI_data_get_8[0],\
/*Device_X_func_IT = */								&USER_SPI_LED_IT,\
/*Device_X_func_CB = */								NULL,\
/*hspi_x = */													&hspi1,\
/*Reg_Params.DataSize = */						SPI_DATASIZE_8BIT,\
/*Reg_Params.BaudRatePrescaler = */		SPI_BAUDRATEPRESCALER_256,\
/*Reg_Params.CLKPolarity = */					SPI_POLARITY_LOW,\
/*Reg_Params.CLKPhase = */						SPI_PHASE_1EDGE,\
/*Enable_Flag = */										true,\
/*Num_packets = */										1,\
};

//SPI_LED_State_T	SPI_LED_State = {2};                                 2xSZ7czp
//bool sleep_mode_flag = 0;

extern uint8_t polling_fault;
uint8_t usb_en_flag = 0;

SPI_DEVICE_X_RETURN_STATE_T USER_SPI_LED_IT(void)
{
	OSL_SPI_DEVICEx_Logical(&SPI_LED);	
	//SPI_LED.data_send[0] = SPI_LED_State.all;
	
	//SPI_LED.data_send[0] = grMAIN.LED_State.Var.all;
	if (!polling_fault) SPI_LED.data_send[0] = (grMAIN.LED_State.Var.all & 0x1F) | (usb_en_flag<<7);
	else SPI_LED.data_send[0] = 3 | (usb_en_flag<<7);
	
	OSL_SPI_Data(&SPI_LED);
	return SPI_DEVICE_X_RETURN_STATE_BUSY;
}

/*
void USER_SPI_LED_CB(void)
{
	LED_flag = 1;
}
*/
//SPI LED}



//TASKS{
void USER_SPI_TASK_0_LED_INIT(SPI_TASKx_T *p, SPI_DEVICEx_Params_T **TASK_x)
{
	TASK_x[0] = &SPI_LED;
	p->Elements_num = 1;
	p->SPI_DEVICEx_Params_p = TASK_x;
}

//Memory

void USER_SPI_TASK_1_EEPROM_FLASH_INIT(SPI_TASKx_T *p, SPI_DEVICEx_Params_T **TASK_x)
{
	TASK_x[0] = &EEPROM_Params.SPI_DEVICEx;
	TASK_x[1] = &EXT_FLASH_Params.SPI_DEVICEx;
	p->Elements_num = 2;
	p->SPI_DEVICEx_Params_p = TASK_x;
}
//TASKS}


void USER_SPI_MASTER_Init(SPIx_Params_T *p)
{
	OSL_SPIx_Params_INIT(p);

	p->Tasks_num = 2;
	p->Zero_task = &SPI_TASKS[0];		//[0]
	p->hspi_x = &hspi1;
	
	//TASKS INIT
	USER_SPI_TASK_0_LED_INIT(&SPI_TASKS[0], &SPI_TASK_LED[0]);
	USER_SPI_TASK_1_EEPROM_FLASH_INIT(&SPI_TASKS[1], &SPI_TASK_EEPROM_FLASH[0]);

	// EEPROM INIT
	USER_SPI_EEPROM_INIT(&EEPROM_Params.SPI_DEVICEx, p);
	OSL_EEPROM_Init_Params(&EEPROM_Params);

	// EXT FLASH INIT
	USER_EXT_FLASH_INIT(&EXT_FLASH_Params.SPI_DEVICEx, p);
	OSL_EXT_FLASH_Init_Params(&EXT_FLASH_Params);
	
	EXT_FLASH_Params.Oper_Sector = 0;
	EXT_FLASH_Params.Sector_Buff[0].Start_Sector_Flash_Addr = grMAIN.Kit[0].Ext_Flash_Addr.Min;
	EXT_FLASH_Params.Sector_Buff[0].Stop_Sector_Flash_Addr = grMAIN.Kit[0].Ext_Flash_Addr.Max;
	
	EXT_FLASH_Params.Sector_Buff[1].Start_Sector_Flash_Addr = grMAIN.Kit[1].Ext_Flash_Addr.Min;
	EXT_FLASH_Params.Sector_Buff[1].Stop_Sector_Flash_Addr = grMAIN.Kit[1].Ext_Flash_Addr.Max;	
}



uint32_t TxRx_timer = 0;
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi == &hspi1)
	{
		TxRx_timer++;
		OSL_SPI_X_Callback(&SPI_Params);
	}
}

uint32_t Rx_timer = 0;
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi == &hspi1)
	{
		Rx_timer++;
		OSL_SPI_X_Callback(&SPI_Params);
	}
}

uint32_t Tx_timer = 0;
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi == &hspi1)
	{
		Tx_timer++;
		OSL_SPI_X_Callback(&SPI_Params);
	}
}

uint8_t error_tim = 0;

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	if (hspi == &hspi1)
	{
		SPI_Params.Busy_Flag = 0;
		SPI_Params.hspi_x->ErrorCode = 0;
		error_tim++;
	}	
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
