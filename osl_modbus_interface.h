#ifndef __modbus_interface_H_dim
#define __modbus_interface_H_dim
#ifdef __cplusplus
 extern "C" {
#endif

#include "osl_user_config.h"	 
#include "osl_modbus_protocol.h"
	 
#ifdef USB_MODBUS
	#include "usbd_cdc_if.h"
#endif
 
#if defined (BT_MODBUS) || defined (RS485_MODBUS)
	#include "usart.h"

	void OSL_Modbus_Rx_Callback(Modbus_Params_T *p);
	void OSL_Modbus_Tx_Callback(Modbus_Params_T *p);

 #if defined (RS485_MODBUS)
		#include "gpio.h"
	#endif
#endif

// TIMEOUTS
#if defined (BT_MODBUS)
	#define BT_TIMEOUT_BAUD_115200		30000UL	 
#endif
	 
#if defined (RS485_MODBUS)
	#define RS_TIMEOUT_BAUD_9600			15000UL	 
	#define RS_TIMEOUT_BAUD_19200			10000UL 
	#define RS_TIMEOUT_BAUD_115200		3000UL
	#define RS_TIMEOUT_BAUD_230400		1500UL
#endif	

#if defined (MEMORY_USED)
	#include "osl_memory.h"
#endif

void OSL_MDB_Frame_end_USART_IDLE(Modbus_Params_T *p);
	
void OSL_Modbus_to_Interface_Tx(Modbus_Params_T *p, uint8_t Tx_data_lenth);
void OSL_MDB_Reset_Receive(Modbus_Params_T *p);

void OSL_Modbus_Baudrate_Timeout(Modbus_Params_T *p);
void OSL_Modbus_Master_Rx(Modbus_Params_T *p);

#if defined(KERNEL_USED)
void OSL_Modbus_Timeout_Reset_KERNEL(Modbus_Params_T *p, TIMx_Params_T *v);
void OSL_Modbus_Baudrate_Timeout_Kernel(Modbus_Params_T *p);
#endif

#if defined(OSL_USER_MDB_ETH_EN)
void OSL_TCP_MDB_End_Frame(struct tcp_pcb *tpcb, eth_struct *es);
void eth_tcp_next_frame(struct pbuf *ptr, struct tcp_pcb *tpcb, eth_struct *es);
#endif

#if defined(MODBUS_MASTER_USED)
void OSL_Modbus_Master_Tx(Modbus_Params_T *p);
#endif

#ifdef __cplusplus
}
#endif
#endif
