#ifndef __usart_H_dim
#define __usart_H_dim
#ifdef __cplusplus
 extern "C" {
#endif
 
#include "osl_user_config.h"
#include "osl_general.h"
//#include "stm32f1xx_ll_usart.h"
//#include "stm32f1xx.h"
#if defined (F1XX)   
  #include "stm32f1xx_ll_dma.h"
  #include "stm32f1xx_ll_usart.h"    
#endif   
 
  
	 
typedef struct {
	// Function inited (must be inited by special function ones):
	uint32_t									BaudRate;					// Скорость передачи
	uint32_t									Parity;						// Паритет
	uint32_t									StopBits;					// Число стоповых бит
	uint32_t									USARTx_function_break;
	uint32_t									Rx_Calc;
	uint32_t									Tx_Calc;
	uint32_t									Rx_Calc_prev;
	uint32_t									Tx_Calc_prev;
	bool											USARTx_init_was;
	bool											ext_init_flag;  
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	bool											USARTx_init;													// Allowes USARTx to be inited
	bool											One_Wire_Mode;	
  
  #if defined (F1XX)
    USART_TypeDef*            USART_x;
    DMA_TypeDef*              DMAx;
    uint32_t									DMAch_TX;
    uint32_t									DMAch_RX;
    uint32_t									APBx_Freq;
    uint32_t                  BaudRateRef;
  #elif defined (F4XX)
    UART_HandleTypeDef*				husart_x;	                         // Structer of USARTx
    void											(*USARTx_Init_func)(void);						// USART init function
  #endif

	__IO uint8_t							*data_send_8;													// Указатель на буфер отправки
	__IO uint8_t							*data_get_8;													// Указатель на буфер приема
	// Changing params (must be inited, may be reinited by developer):
	bool											reset_Callback;	
	uint16_t									Num_packets_get;
	// Logic inited params (mustn`t be inited, reinited):
	uint16_t									Num_packets_get_prev;
} USARTx_Params_T;

void OSL_USART_Logical(USARTx_Params_T *p);
void OSL_USART_Params_INIT (USARTx_Params_T *p);
void OSL_USART_ReInit(USARTx_Params_T *p);
void OSL_USART_GetData(USARTx_Params_T *p);
void OSL_USART_Send_data(USARTx_Params_T *p, uint8_t array_lenth);
void OSL_USART_Reset_DMA_Interrupt(USARTx_Params_T *p, bool Callback_call);
void OSL_USART_BaudrateChangeCallback(USARTx_Params_T *USARTx_Params);

//void USART_RX(DMA_TypeDef* DMAx, uint32_t DMAch, uint16_t sz);
//void USART_TX(DMA_TypeDef* DMAx, uint32_t DMAch, uint16_t sz);

#ifdef __cplusplus
}
#endif
#endif
