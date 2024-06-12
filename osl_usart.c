/* Includes ------------------------------------------------------------------*/
#include "osl_usart.h"

#if defined (F1XX)
static inline void USART_RX(DMA_TypeDef* DMAx, uint32_t DMAch, uint16_t sz)
{
  LL_DMA_DisableChannel(DMAx, DMAch);
  LL_DMA_SetDataLength(DMAx, DMAch, sz);
  LL_DMA_EnableChannel(DMAx, DMAch);
}

static inline void USART_TX(DMA_TypeDef* DMAx, uint32_t DMAch, uint16_t sz)
{
  LL_DMA_DisableChannel(DMAx, DMAch);
  LL_DMA_SetDataLength(DMAx, DMAch, sz);//+2
  LL_DMA_EnableChannel(DMAx, DMAch);
}
#endif

__weak void OSL_UART_RxCpltCallback(USART_TypeDef *USART_x){}

void OSL_USART_Logical(USARTx_Params_T *p)
{
	bool reset_flag = false;
	bool reinit_flag = false;	
	
	if (p->USARTx_init && !p->USARTx_init_was)
	{
    #if defined (F1XX)
      //добавить включение флагов прирывания уарта по прирываниям ДМА ТС и ТЕ, очистить флаги ДМА
      LL_DMA_DisableChannel(p->DMAx, p->DMAch_RX);
      LL_DMA_DisableChannel(p->DMAx, p->DMAch_TX);
      
      LL_USART_EnableDMAReq_RX(p->USART_x);
      LL_USART_EnableDMAReq_TX(p->USART_x);
      LL_USART_EnableIT_ERROR(p->USART_x);      //interrupt FE, ORE, NE
      LL_USART_EnableIT_PE(p->USART_x);         //interrupt PE
      
      LL_DMA_EnableIT_TC(p->DMAx, p->DMAch_RX);
      LL_DMA_EnableIT_TE(p->DMAx, p->DMAch_RX);
      LL_DMA_EnableIT_TC(p->DMAx, p->DMAch_TX);
      LL_DMA_EnableIT_TE(p->DMAx, p->DMAch_TX);    
      
      LL_DMA_ClearFlag_GI5(p->DMAx);
      LL_DMA_ClearFlag_GI4(p->DMAx);
      LL_DMA_ClearFlag_TC4(p->DMAx);
      LL_DMA_ClearFlag_TE4(p->DMAx);
      LL_DMA_ClearFlag_TC5(p->DMAx);
      LL_DMA_ClearFlag_TE5(p->DMAx);
          
      p->USARTx_init_was = true;

      p->BaudRate = p->BaudRateRef;
      p->Num_packets_get_prev = p->Num_packets_get;
   
      LL_DMA_ConfigAddresses(p->DMAx, p->DMAch_TX, (uint32_t)p->data_send_8, LL_USART_DMA_GetRegAddr(p->USART_x), LL_DMA_GetDataTransferDirection(p->DMAx, p->DMAch_TX));
      LL_DMA_ConfigAddresses(p->DMAx, p->DMAch_RX,  LL_USART_DMA_GetRegAddr(p->USART_x), (uint32_t)p->data_get_8, LL_DMA_GetDataTransferDirection(p->DMAx, p->DMAch_RX));    
      
      OSL_USART_Send_data(p, 2);
      USART_RX(p->DMAx, p->DMAch_RX, p->Num_packets_get_prev);
    #elif defined (F4XX)
      p->USARTx_Init_func();
      p->USARTx_init_was = true;
      p->BaudRate = p->husart_x->Init.BaudRate;
      p->Num_packets_get_prev = p->Num_packets_get;
      HAL_UART_Receive_DMA(p->husart_x, (uint8_t*)p->data_get_8, p->Num_packets_get_prev);
    #endif
	}

  #if defined (F1XX)
    if (p->BaudRate != p->BaudRateRef)
    {
      reinit_flag = true;
      reset_flag = true;
    }  
    if (p->StopBits != LL_USART_GetStopBitsLength(p->USART_x)) 
    {
      reinit_flag = true;
      reset_flag = true;
    }  
    if (p->Parity != LL_USART_GetParity(p->USART_x))
    {
      reinit_flag = true;
      reset_flag = true;
    }	
  #elif defined (F4XX)
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
  #endif

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
  #if defined (F1XX)  
    LL_USART_SetBaudRate(p->USART_x, p->APBx_Freq, p->BaudRateRef);
    p->BaudRate = p->BaudRateRef;
    LL_USART_SetStopBitsLength(p->USART_x, p->StopBits);
    LL_USART_SetParity(p->USART_x, p->Parity);
      
    if (p->Parity == LL_USART_PARITY_NONE) LL_USART_SetDataWidth(p->USART_x, LL_USART_DATAWIDTH_8B);
    else LL_USART_SetDataWidth(p->USART_x, LL_USART_DATAWIDTH_9B);

    if (p->One_Wire_Mode) LL_USART_EnableHalfDuplex(p->USART_x);
    else LL_USART_DisableHalfDuplex(p->USART_x);
  #elif defined (F4XX)
  	p->husart_x->Init.BaudRate = p->BaudRate;
    p->husart_x->Init.StopBits = p->StopBits;
    p->husart_x->Init.Parity = p->Parity;

    if (p->husart_x->Init.Parity == UART_PARITY_NONE) p->husart_x->Init.WordLength = UART_WORDLENGTH_8B;
    else p->husart_x->Init.WordLength = UART_WORDLENGTH_9B;
    
    if (!p->One_Wire_Mode) HAL_UART_Init(p->husart_x);
    else HAL_HalfDuplex_Init(p->husart_x);
  #endif  

	OSL_USART_BaudrateChangeCallback(p);
}

void OSL_USART_GetData(USARTx_Params_T *p)
{
  #if defined (F1XX)
    USART_RX(p->DMAx, p->DMAch_RX, p->Num_packets_get_prev);
  #elif defined (F4XX)
    HAL_UART_Receive_DMA(p->husart_x, (uint8_t*)p->data_get_8, p->Num_packets_get_prev);
  #endif    
}

void OSL_USART_Send_data(USARTx_Params_T *p, uint8_t array_lenth)
{
	#if defined (F1XX)
    USART_TX(p->DMAx, p->DMAch_TX, array_lenth);
  #elif defined (F4XX)
    HAL_UART_Transmit_DMA(p->husart_x, (uint8_t*)p->data_send_8, array_lenth);
  #endif
}


void OSL_USART_Reset_DMA_Interrupt(USARTx_Params_T *p, bool Callback_call)
{  
#if defined (F1XX)
  if (Callback_call) OSL_UART_RxCpltCallback(p->USART_x);
  
#elif defined (F4XX) 
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
  p->husart_x->RxXferSize = (uint16_t)p->Num_packets_get_prev;
  
	if (p->husart_x->hdmarx->XferCpltCallback != NULL)
  {
    UART_HandleTypeDef* huart = ( UART_HandleTypeDef* )((DMA_HandleTypeDef* )p->husart_x->hdmarx)->Parent;
    if((p->husart_x->hdmarx->Instance->CR & DMA_SxCR_CIRC) == 0U)
    {
      huart->RxXferCount = 0U;
      CLEAR_BIT(huart->Instance->CR1, USART_CR1_PEIE);
      CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);
      CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);
      huart->RxState = HAL_UART_STATE_READY;
    }
    if (Callback_call) HAL_UART_RxCpltCallback(huart);
  }
  
  if (!Callback_call) HAL_UART_Receive_DMA(p->husart_x, (uint8_t*)p->data_get_8, p->Num_packets_get_prev);  
#endif     
}




/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
