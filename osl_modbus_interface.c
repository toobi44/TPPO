/* Includes ------------------------------------------------------------------*/
#include "osl_modbus_interface.h"

/************************Initialization***************************/
static inline void OSL_Mdb_Rx_Tx_INIT(Mdb_Rx_Tx_T *p)
{
	p->Trans_Ident = 0;
	p->Prot_Ident = 0;
	p->Length = 0;
	p->ID = 0;
	p->Func_Code = 0;
	p->Data_addr = 0;
	p->Num_reg = 0;
	p->CRC_Res = 0;
	p->Bytes = 0;
}

#if defined (BT_MODBUS) || defined (RS485_MODBUS)
void OSL_Modbus_INIT(Modbus_Params_T *p, USARTx_Params_T *USART)
#else	
void OSL_Modbus_INIT(Modbus_Params_T *p)
#endif
{
	p->Inductrial_Protocol = IND_PROT_MDB;
#if defined (BT_MODBUS) || defined (RS485_MODBUS)
	if (p->Interface == MDB_INT_RS485 || p->Interface == MDB_INT_BT)
	{
    p->USART = USART;
#if defined (F1XX)
	#if defined (OSL_LL_DMA)
    p->Byte_timer = LL_DMA_GetDataLength(p->USART->DMAx, p->USART->DMAch_RX);
	#else
		p->Byte_timer = &USART->husart_x->hdmarx->Instance->CNDTR;
	#endif	
#elif defined(F4XX)	
		p->Byte_timer = &USART->husart_x->hdmarx->Instance->NDTR;
#endif
		p->Byte_timer_prev = USART->Num_packets_get;
	}
#endif

#if defined(MODBUS_SLAVE_USED)
	if (p->Role == MDB_ROLE_SLAVE)
	{
		p->Mdb_Conv_Data.g_Ram_modbus_x = &g_Ram_modbus[0];
		p->Mdb_Conv_Data.g_Ram_modbus_Size_x = g_Ram_modbus_Size;
	}
#endif	
#if defined(MODBUS_MASTER_USED)	&& defined MODBUS_MASTER_PAR_NUM
	if (p->Role == MDB_ROLE_MASTER)
	{
		p->Mdb_Conv_Data.g_Ram_modbus_x = &g_Ram_modbus_M[0];
		p->Mdb_Conv_Data.g_Ram_modbus_Size_x = g_Ram_modbus_Size_M;
	}
#endif	
	
	p->Mdb_Conv_Data.Conv_only_flag = false;
	p->Mdb_Conv_Data.Out_Data = &p->Mdb_Tx_Data[0];
#if defined (BT_MODBUS) || defined (RS485_MODBUS)
	OSL_Modbus_Baudrate_Timeout(p);
#endif	
	if (p->Interface == MDB_INT_USB) p->Byte_timer_prev = 0;
	OSL_TIM_Time_Calc_Init(&p->TIM_Reset_Time);
	p->Addr = 0;
	p->Mdb_Timer_Break_Receive = 0;
	p->Mdb_Timer_No_Answer = 0;
	p->Mdb_Timer_Success_Answer = 0;
	p->Mdb_Timer_Exception = 0;
	p->usb_Timer = 0;
	
	p->Exception = MDB_EXC_CODE_FREE;
	p->Error.all = 0;
	p->Receive_State = MDB_REC_STATE_FREE;
	
	for (uint8_t i = 0; i < MAX_RX_DATA_SIZE; i++) p->Mdb_Rx_Data[i] = 0;
	for (uint8_t i = 0; i < MAX_TX_DATA_SIZE; i++) p->Mdb_Tx_Data[i] = 0;
	
	OSL_Mdb_Rx_Tx_INIT(&p->Rx_Params);
	OSL_Mdb_Rx_Tx_INIT(&p->Tx_Params);
#if defined(MODBUS_MASTER_USED) && defined MODBUS_MASTER_PAR_NUM
	if (p->Role == MDB_ROLE_MASTER) OSL_Modbus_Master_INIT(p);
#endif	
	p->journal_flag = false;
	p->Repeater_flag = false;

#if defined (EXT_EEPROM_USED)
	p->Planned_oper_mode = &EEPROM_Params.Planned_oper_mode;
#endif

#if defined (RS485_MODBUS)
	if (p->Interface == MDB_INT_RS485)
	{
	#if defined (OSL_LL_GPIO)
		LL_GPIO_ResetOutputPin(p->RS_485_Port, p->RS_485_Pin);
	#else
		HAL_GPIO_WritePin(p->RS_485_Port, p->RS_485_Pin, GPIO_PIN_RESET);
	#endif
	}
#endif
#if defined(OSL_USER_MDB_ETH_EN)
	if (p->Interface != MDB_INT_ETH) 
	{
		p->server = NULL;
		p->client = NULL;
	}
#endif
}

#if defined(MODBUS_MASTER_USED)
void OSL_Modbus_Master_INIT(Modbus_Params_T *p)
{
	p->Mas_Par->Par_timer = 0;

	for (uint16_t tmp_Par_Num = 0; tmp_Par_Num < MODBUS_MASTER_PAR_NUM; tmp_Par_Num++)
	{
		p->Mas_Par->Par_State[tmp_Par_Num].Error.all = 0;
		p->Mas_Par->Par_State[tmp_Par_Num].Receive_State = MDB_REC_STATE_FREE;
		p->Mas_Par->Par_State[tmp_Par_Num].Exception = MDB_EXC_CODE_FREE;
		p->Mas_Par->Oper_Request[tmp_Par_Num].Par_Allow = 0;
	}
	
	OSL_TIM_Time_Calc_Init(&p->Mas_Par->TIM_Answer_Time);
	p->Mas_Par->Low_priority_Addr = 0;
	p->Mas_Par->Low_priority_Flag = 0;
	p->Mas_Par->Par_Allow_During = 0;

	p->Mas_Par->Mas_Conv_Data.Conv_only_flag = true;
	p->Mas_Par->Mas_Conv_Data.Out_Data = &p->Mdb_Tx_Data[0];
	
	p->Mas_Par->Mas_Conv_Data.g_Ram_modbus_Size_x = g_Ram_modbus_Size_M;
	p->Mas_Par->Mas_Conv_Data.g_Ram_modbus_x = &g_Ram_modbus_M[0];
	p->Mas_Par->Mas_Request_flag = false;
}
#endif
/************************Initialization***************************/


/************************End Frame Search***************************/
//Функция поиска окончания конца кадра по тайм-ауту
//В зависимости от скорости и выбранного тайм-аута требует достаточной частоты обработки
//Подходит для общения с неизвестным внешним устройством
void OSL_MDB_Frame_end_Timeout(Modbus_Params_T *p, TIMx_Params_T *v)
{
#if defined (OSL_DMA_LL)
	p->Byte_timer = LL_DMA_GetDataLength(p->USART->DMAx, p->USART->DMAch_RX);
	if (!p->Byte_timer) return;
	if (p->Byte_timer != p->Byte_timer_prev)
	{
		p->Byte_timer_prev = p->Byte_timer;
		OSL_TIM_Time_Calc (&p->TIM_Reset_Time, v, TIME_CALC_ACTIVE);
	}
	OSL_TIM_Time_Calc(&p->TIM_Reset_Time, v, VIEW_RESULT);
	if ( p->TIM_Reset_Time.Result_time_us > p->Reset_Timeout || ( !p->Byte_timer && p->Interface != MODBUS_INTERFACE_USB ) )					// Если все данные пришли, а прерывания не было
	{
	#if defined (BT_MODBUS) || defined (RS485_MODBUS)
		//if ( (p->Interface != MODBUS_INTERFACE_USB && !p->USART->husart_x->ErrorCode) ||  p->Interface == MODBUS_INTERFACE_USB)       
	#else
		if (p->Interface == MODBUS_INTERFACE_USB)
	#endif		
		{
			p->Receive_State = MODBUS_RECEIVE_STATE_FREE;
			p->TIM_Reset_Time.Result_time_us = 0;
			p->TIM_Reset_Time.State = WAITING_INIT_TIME;
			OSL_Modbus_Reset_Receiving(p);
		}
	}
#else	
	if (*p->Byte_timer != p->Byte_timer_prev)
	{
		p->Byte_timer_prev = *p->Byte_timer;
		OSL_TIM_Time_Calc (&p->TIM_Reset_Time, v, TIME_CALC_ACTIVE);
	}
	OSL_TIM_Time_Calc(&p->TIM_Reset_Time, v, VIEW_RESULT);

	if ( p->TIM_Reset_Time.Result_time_us > p->Reset_Timeout || ( !*p->Byte_timer && p->Interface != MDB_INT_USB ) )					// Если все данные пришли, а прерывания не было
	{
	#if defined (BT_MODBUS) || defined (RS485_MODBUS)
		if ( (p->Interface != MDB_INT_USB && !p->USART->husart_x->ErrorCode) ||  p->Interface == MDB_INT_USB)
	#else
		if (p->Interface == MODBUS_INTERFACE_USB)
	#endif		
		{
			p->Receive_State = MDB_REC_STATE_FREE;
			p->TIM_Reset_Time.Result_time_us = 0;
			p->TIM_Reset_Time.State = WAITING_INIT_TIME;
			OSL_MDB_Reset_Receive(p);
		}
	}
#endif
}

#if defined(BT_MODBUS) || defined(RS485_MODBUS)
//Функция поиска окончания конца кадра с помощью аппаратных средств USART
void OSL_MDB_Frame_end_USART_IDLE(Modbus_Params_T *p)
{
	//if (p->Interface == MDB_INT_USB) return;
	if ((READ_REG(p->USART->husart_x->Instance->SR) & USART_SR_IDLE) && 
			(READ_REG(p->USART->husart_x->Instance->CR1) & USART_CR1_IDLEIE))		//Disable receive
	{
		p->Receive_State = MDB_REC_STATE_FREE;
		p->Mdb_Timer_Break_Receive++;
		p->Bytes_Delta = p->USART->Num_packets_get_prev - *p->Byte_timer;
		OSL_USART_Reset_DMA_Interrupt(p->USART, p->USART->reset_Callback);
	}
}
#endif

#if defined(OSL_USER_MDB_ETH_EN)
__weak Modbus_Params_T* OSL_TCP_MDB_ADDR_CB(struct tcp_pcb *tpcb){return NULL;}

void OSL_TCP_MDB_End_Frame(struct tcp_pcb *tpcb, eth_struct *es)
{
#if defined(MODBUS_SLAVE_USED)
	Modbus_Params_T *mdb = OSL_TCP_MDB_ADDR_CB(tpcb);
	
	mdb->Mdb_Timer_Break_Receive++;
	mdb->ptr = es->p;
	mdb->es_tcp = es;
	mdb->Rx_Buff = mdb->ptr->payload;
	mdb->Bytes_Delta = es->p->len;
	
	if (!mdb) return;																						// Колбек не прописан корректно
	if (!mdb->client) return;
	if (mdb->client != tpcb) return;

	if (mdb->Receive_State != MDB_REC_STATE_DELAYED_ANSWER)
	{
		if (mdb->Role == MDB_ROLE_SLAVE)
		{
			mdb->Receive_State = MDB_REC_STATE_FREE;
			OSL_Mdb_Slave_Packet_Check(mdb);
			eth_tcp_next_frame(mdb->ptr, tpcb, es);									// Разрешаем прием следующего кадра		
		}
	}
	else eth_tcp_next_frame(mdb->ptr, tpcb, es);								// Разрешаем прием следующего кадра		
#endif
}
#endif
/************************End Frame Search***************************/


/************************Reset Receive***************************/
void OSL_MDB_Reset_Receive(Modbus_Params_T *p)
{
	p->Mdb_Timer_Break_Receive++;
#if defined(BT_MODBUS) || defined(RS485_MODBUS)
	if (p->Interface == MDB_INT_BT || p->Interface == MDB_INT_RS485)
	{
	#if defined (OSL_LL_DMA)
		p->Byte_timer = LL_DMA_GetDataLength(p->USART->DMAx, p->USART->DMAch_RX);
		p->Bytes_Delta = p->USART->Num_packets_get_prev - p->Byte_timer;
	#else
		p->Bytes_Delta = p->USART->Num_packets_get_prev - *p->Byte_timer;
	#endif
		OSL_USART_Reset_DMA_Interrupt(p->USART, p->USART->reset_Callback);
	}
#endif
	
#ifdef USB_MODBUS
	if (p->Interface == MDB_INT_USB)
	{
		p->Bytes_Delta = p->usb_Timer;
		if (p->Role == MDB_ROLE_SLAVE) OSL_Mdb_Slave_Packet_Check(p);
		OSL_MODBUS_ClearGettedData(p, NO_MATTER);
	}
#endif
}
/************************Reset Receive***************************/


/************************Callback***************************/
#if defined(BT_MODBUS) || defined(RS485_MODBUS)
void OSL_Modbus_Rx_Callback(Modbus_Params_T *p)
{
	#if defined (OSL_LL_GPIO)
    LL_DMA_DisableChannel(p->USART->DMAx, p->USART->DMAch_RX);
    LL_DMA_SetDataLength(p->USART->DMAx, p->USART->DMAch_RX, 0);
    LL_DMA_EnableChannel(p->USART->DMAx, p->USART->DMAch_RX);
    p->Byte_timer = LL_DMA_GetDataLength(p->USART->DMAx, p->USART->DMAch_RX);
  #else
    //OSL_USART_GetData(p->USART);
  #endif
		
#if defined(MODBUS_SLAVE_USED)
	if (p->Role == MDB_ROLE_SLAVE)
	{
		if (p->Receive_State != MDB_REC_STATE_USART_FAULT)
			OSL_Mdb_Slave_Packet_Check(p);
		
		// Включение приема, так как ответа на кадр не будет
		if (p->Receive_State >= MDB_REC_STATE_USART_FAULT)
		{
			if (p->Interface != MDB_INT_USB)
			{
				OSL_USART_Receive_En(p->USART);
				OSL_MODBUS_ClearGettedData(p, p->USART->Num_packets_get_prev);
			}
		}
	}
#endif
		
#if defined(MODBUS_MASTER_USED)
	if (p->Role == MDB_ROLE_MASTER && (p->Receive_State != MDB_REC_STATE_USART_FAULT))
		OSL_Modbus_Master_Packet_Check(p);
#endif
}

void OSL_Modbus_Tx_Callback(Modbus_Params_T *p)
{
	if (p->Interface == MDB_INT_RS485)
	{
	#if defined (OSL_LL_GPIO)
		LL_GPIO_ResetOutputPin(p->RS_485_Port, p->RS_485_Pin);
	#else
		HAL_GPIO_WritePin(p->RS_485_Port, p->RS_485_Pin, GPIO_PIN_RESET);
	#endif
	}
	
	if (p->Interface == MDB_INT_RS485 ||
			p->Interface == MDB_INT_BT)
	{
		OSL_USART_Receive_En(p->USART);
		OSL_MODBUS_ClearGettedData(p, p->USART->Num_packets_get_prev);
	}
}

// Функция восстановление связи
void OSL_Modbus_Restore_Receiving(Modbus_Params_T *p)
{
	p->TIM_Reset_Time.Result_time_us = 0;
	p->TIM_Reset_Time.State = WAITING_INIT_TIME;
	p->Receive_State = MDB_REC_STATE_USART_FAULT;
	OSL_MDB_Reset_Receive(p);
}

void OSL_Modbus_Baudrate_Timeout(Modbus_Params_T *p)
{
#ifdef BT_MODBUS	
	if (p->Interface == MODBUS_INTERFACE_BT)
	{
		p->Reset_Timeout = BT_TIMEOUT_BAUD_115200;				//30000UL;
		return;
	}
#endif

#ifdef RS485_MODBUS
	if (p->Interface == MDB_INT_RS485)
	{
		if 			(p->USART->BaudRate == 9600)		p->Reset_Timeout = RS_TIMEOUT_BAUD_9600;			//10000UL;
		else if (p->USART->BaudRate == 19200)		p->Reset_Timeout = RS_TIMEOUT_BAUD_19200;			//8000UL;
		else if (p->USART->BaudRate == 115200)	p->Reset_Timeout = RS_TIMEOUT_BAUD_115200;		//3000UL;    115384
		else if (p->USART->BaudRate == 230400)	p->Reset_Timeout = RS_TIMEOUT_BAUD_230400;		//500UL;
		return;
	}
#endif

#endif
}
/************************Callback***************************/


/********************Protocol->Interface*******************/
//Функция передачи данных от протокола интерфейсу

void OSL_Modbus_to_Interface_Tx(Modbus_Params_T *p, uint8_t Tx_data_lenth)
{
#ifdef USB_MODBUS
	if (p->Interface == MDB_INT_USB)
	{
		CDC_Transmit_FS(p->Tx_Buff, Tx_data_lenth);
		return;
	}
#endif
	
#ifdef BT_MODBUS
	if (p->Interface == MODBUS_INTERFACE_BT)
	{
		if (p->Repeater_flag || p->Manual_Request || p->Mode == MODBUS_MODE_MASTER)
			OSL_USART_Reset_DMA_Interrupt(p->USART, 0);
		OSL_USART_Transmit_En(p->USART, Tx_data_lenth);
		return;		
	}
#endif

#ifdef RS485_MODBUS
	if (p->Interface == MDB_INT_RS485)
	{
		if (p->Repeater_flag || p->Manual_Request || p->Role == MDB_ROLE_MASTER)
			OSL_USART_Reset_DMA_Interrupt(p->USART, 0);

	#if defined (OSL_LL_GPIO)
		LL_GPIO_SetOutputPin(p->RS_485_Port, p->RS_485_Pin);
	#else	
		HAL_GPIO_WritePin(p->RS_485_Port, p->RS_485_Pin, GPIO_PIN_SET);
	#endif
		OSL_USART_Transmit_En(p->USART, Tx_data_lenth);
		return;
	}
#endif
	
#ifdef OSL_USER_MDB_ETH_EN
	if (p->Interface == MDB_INT_ETH)
	{
		tcp_write(p->client, p->Tx_Buff, Tx_data_lenth, TCP_WRITE_FLAG_COPY);
		
		if (p->Receive_State == MDB_REC_STATE_REPEATER_MODE ||
				p->Receive_State == MDB_REC_STATE_DELAYED_ANSWER)
		{
			tcp_output(p->client);
			p->Receive_State = MDB_REC_STATE_FREE;
		}
	}
#endif
}
/********************Protocol->Interface*******************/


/********************Clear Rx Buff*******************/
void OSL_MODBUS_ClearGettedData(Modbus_Params_T *p, uint16_t Num_packets_get_prev)
{
	if (p->Interface == MDB_INT_RS485 || p->Interface == MDB_INT_BT)
	{
		for (uint16_t i = 0; i < Num_packets_get_prev; i++) p->Rx_Buff[i] = 0;
  #if defined (F1XX)
    p->Byte_timer_prev = Num_packets_get_prev; 
  #elif defined (F4XX)
    p->Byte_timer_prev = *p->Byte_timer;
  #endif		
	}
	else if (p->Interface == MDB_INT_USB)
	{
		for (uint16_t i = 0; i < p->usb_Timer; i++) p->Rx_Buff[i] = 0;
		*p->Byte_timer = 0;
		p->Byte_timer_prev = 0;
	}
}
/********************Clear Rx Buff*******************/
