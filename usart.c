/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
USARTx_Params_T USART_PULT_RS485;
__IO uint8_t USART_RS485_data_get_8 [USART_RS485_RX_BUFF],
						 USART_RS485_data_send_8[USART_RS485_TX_BUFF];
Modbus_Params_T RS485_Modbus;
/* USER CODE END 0 */

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = PULT_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(PULT_TX_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = PULT_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(PULT_RX_GPIO_Port, &GPIO_InitStruct);

    /* USART2 DMA Init */
    /* USART2_RX Init */
    hdma_usart2_rx.Instance = DMA1_Channel6;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_NORMAL;
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart2_rx);

    /* USART2_TX Init */
    hdma_usart2_tx.Instance = DMA1_Channel7;
    hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_tx.Init.Mode = DMA_NORMAL;
    hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart2_tx);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 12, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, PULT_TX_Pin|PULT_RX_Pin);

    /* USART2 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void USART_PULT_RS485_Init(USARTx_Params_T *p)
{
	OSL_USART_Params_INIT(p);
	
	p->USARTx_init = true;
	p->husart_x = &huart2;
	p->USARTx_Init_func = MX_USART2_UART_Init;
	p->IDLE_flag = true;

	p->data_send_8 = USART_RS485_data_send_8;
	p->data_get_8 = USART_RS485_data_get_8;
	p->One_Wire_Mode = false;
	p->Num_packets_get = USART_RS485_RX_BUFF;
	p->reset_Callback = true;
	OSL_USART_Logical(p);	
}

void Modbus_PULT_RS485_Slave_Init(Modbus_Params_T *p, USARTx_Params_T *USART)
{
	USART->BaudRate = 115200;
	USART->Parity = UART_PARITY_NONE;
	USART->StopBits = UART_STOPBITS_1;
	
	USART->Num_packets_get = USART_RS485_RX_BUFF;

	p->Rx_Buff = (uint8_t*)USART->data_get_8;
	p->Tx_Buff = (uint8_t*)USART->data_send_8;
	p->Interface = MDB_INT_RS485;
	p->Mdb_Protocol = MDB_PROTOCOL_RTU;	
	p->Role = MDB_ROLE_SLAVE;
	p->ID_Device = 1;

	p->RS_485_Port	= PULT_DIR_GPIO_Port;
	p->RS_485_Pin		= PULT_DIR_Pin;
	
	OSL_MODBUS_ClearGettedData(p, USART->Num_packets_get_prev);
	OSL_Modbus_INIT(p, USART);
	OSL_USART_Logical(USART);
  USART->ext_init_flag = 1;
	//p->Mas_Par = &Slave_Par;
	//p->Mas_Par->Answer_Lim_Time = 6666UL;				// 150 Ãö
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == USART_PULT_RS485.husart_x && USART_PULT_RS485.ext_init_flag) OSL_Modbus_Rx_Callback(&RS485_Modbus);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{  
	if (huart == USART_PULT_RS485.husart_x && USART_PULT_RS485.ext_init_flag) OSL_Modbus_Tx_Callback(&RS485_Modbus);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if (huart == USART_PULT_RS485.husart_x && USART_PULT_RS485.ext_init_flag) OSL_Modbus_Restore_Receiving(&RS485_Modbus);
}

void OSL_USART_BaudrateChangeCallback(USARTx_Params_T *USARTx_Params)
{
	if (USARTx_Params == &USART_PULT_RS485) OSL_Modbus_Baudrate_Timeout(&RS485_Modbus);
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
