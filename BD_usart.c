/* Includes ------------------------------------------------------------------*/
#include "BD_usart.h"

void OSL_USART_Logical(USARTx_Params_T *p)
{
	bool reset_flag = false;
	bool reinit_flag = false;	
	
	if (p->USARTx_init && !p->USARTx_init_was)
	{
		p->USARTx_Init_func();
		p->USARTx_init_was = true;
		p->BaudRate = p->husart_x->Init.BaudRate;
		p->Num_packets_get_prev = p->Num_packets_get;
		OSL_USART_Receive_En(p);
		//HAL_UART_Receive_DMA(p->husart_x, (uint8_t*)p->data_get_8, p->Num_packets_get_prev);
		//if (p->IDLE_flag == true) SET_BIT(p->husart_x->Instance->CR1,USART_CR1_IDLEIE);
		//__HAL_DMA_DISABLE_IT(p->husart_x->hdmarx, DMA_IT_HT);
	}

	if (p->BaudRate != p->husart_x->Init.BaudRate)
	{
		reinit_flag = true;
		reset_flag = true;
	}
	
	if (p->StopBits != p->husart_x->Init.StopBits)
	{
		reinit_flag = true;
		reset_flag = true;
	}
	
	if (p->Parity != p->husart_x->Init.Parity)
	{
		reinit_flag = true;
		reset_flag = true;
	}	

	if (reinit_flag) OSL_USART_ReInit(p);
	
	if (p->Num_packets_get != p->Num_packets_get_prev)
	{
		p->Num_packets_get_prev = p->Num_packets_get;
		reset_flag = true;
	}

	if (reset_flag)
	{
		reset_flag = false;
		OSL_USART_Reset_DMA_Interrupt(p, p->reset_Callback);
		OSL_USART_Receive_En(p);
	}
}

void OSL_USART_Params_INIT(USARTx_Params_T *p)
{
	p->USARTx_init_was = false;
	p->ext_init_flag = false;
	p->USARTx_function_break = 0;
	
	p->Rx_Calc = 0;
	p->Tx_Calc = 0;
	
	p->Rx_Calc_prev = 0;
	p->Tx_Calc_prev = 0;
}

// Колбек изменения скорости передачи данных
__weak void OSL_USART_BaudrateChangeCallback(USARTx_Params_T *USARTx_Params)
{
	UNUSED(USARTx_Params);
}

void OSL_USART_ReInit(USARTx_Params_T *p)
{
	p->husart_x->Init.BaudRate = p->BaudRate;
	p->husart_x->Init.StopBits = p->StopBits;
	p->husart_x->Init.Parity = p->Parity;

	if (p->husart_x->Init.Parity == UART_PARITY_NONE) p->husart_x->Init.WordLength = UART_WORDLENGTH_8B;
	else p->husart_x->Init.WordLength = UART_WORDLENGTH_9B;
	
	if (!p->One_Wire_Mode) HAL_UART_Init(p->husart_x);
	else HAL_HalfDuplex_Init(p->husart_x);
	
	OSL_USART_BaudrateChangeCallback(p);
}

void OSL_USART_Reset_Rx(DMA_HandleTypeDef *hdma, bool Callback_allowed)
{
  UART_HandleTypeDef* huart = ( UART_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

#if defined (F1XX)
  if((hdma->Instance->CCR & DMA_CCR_CIRC) == 0U)
  {
    huart->RxXferCount = 0U;
    CLEAR_BIT(huart->Instance->CR1, USART_CR1_PEIE);
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);
		huart->gState = HAL_UART_STATE_READY;		
    huart->RxState = HAL_UART_STATE_READY;
  }	
	
#elif defined(F4XX)
  if((hdma->Instance->CR & DMA_SxCR_CIRC) == 0U)
  {
    huart->RxXferCount = 0U;
    CLEAR_BIT(huart->Instance->CR1, USART_CR1_PEIE);
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);
    CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);
		huart->gState = HAL_UART_STATE_READY;
    huart->RxState = HAL_UART_STATE_READY;
  }
#endif
	if (Callback_allowed) HAL_UART_RxCpltCallback(huart);
}

void OSL_USART_Reset_DMA_Interrupt(USARTx_Params_T *p, bool Callback_call)
{
	if (p->IDLE_flag) CLEAR_BIT(p->husart_x->Instance->CR1,USART_CR1_IDLEIE);
	
#if defined (F1XX)
	__HAL_DMA_DISABLE_IT(p->husart_x->hdmarx, DMA_IT_TE | DMA_IT_TC);
	p->husart_x->hdmarx->State = HAL_DMA_STATE_READY;
	__HAL_DMA_CLEAR_FLAG(p->husart_x->hdmarx, __HAL_DMA_GET_TC_FLAG_INDEX(p->husart_x->hdmarx));
	__HAL_UNLOCK(p->husart_x->hdmarx);
#elif defined(F4XX)
	if(!(((p->husart_x->hdmarx->Instance->CR) & (uint32_t)(DMA_SxCR_DBM)) != RESET) )
	{
		if((p->husart_x->hdmarx->Instance->CR & DMA_SxCR_CIRC) == RESET)
		{
			p->husart_x->hdmarx->Instance->CR  &= ~(DMA_IT_TC);
			__HAL_DMA_DISABLE(p->husart_x->hdmarx);								//Сброс флага работы dma
			__HAL_UNLOCK(p->husart_x->hdmarx);
			p->husart_x->hdmarx->State = HAL_DMA_STATE_READY;
		}
	}
#endif
	p->husart_x->RxXferSize = (uint16_t)p->Num_packets_get_prev;

	if (p->husart_x->hdmarx->XferCpltCallback != NULL) OSL_USART_Reset_Rx(p->husart_x->hdmarx, Callback_call);
	//if (!Callback_call) HAL_UART_Receive_DMA(p->husart_x, (uint8_t*)p->data_get_8, p->Num_packets_get_prev);
}

void OSL_USART_Receive_En(USARTx_Params_T *p)
{
	HAL_UART_Receive_DMA(p->husart_x, (uint8_t*)p->data_get_8, p->Num_packets_get_prev);
	if (p->IDLE_flag == true) SET_BIT(p->husart_x->Instance->CR1,USART_CR1_IDLEIE); 
	__HAL_DMA_DISABLE_IT(p->husart_x->hdmarx, DMA_IT_HT);
}

void OSL_USART_Transmit_En(USARTx_Params_T *p, uint8_t Tx_data_lenth)
{
	if (p->IDLE_flag == true) CLEAR_BIT(p->husart_x->Instance->CR1,USART_CR1_IDLEIE);
	HAL_UART_Transmit_DMA(p->husart_x, (uint8_t*)p->data_send_8, Tx_data_lenth);
	__HAL_DMA_DISABLE_IT(p->husart_x->hdmatx, DMA_IT_HT);
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
