/* Includes ------------------------------------------------------------------*/
#include "osl_modbus_interface.h"
#include "user_g_Ram.h"

__weak bool OSL_MDB_Repeater_ID_Check_Callback(Modbus_Params_T *p){return 0;}

/**************************** Modbus Slave ****************************/

#if defined(MODBUS_SLAVE_USED)
__weak bool OSL_MDB_SL_Repeater_Get_Ok_Callback(Modbus_Params_T *p){return 0;}
__weak bool OSL_MDB_SL_Data_Frame_Callback(Modbus_Params_T *p){return 0;}

// Функция разбора кадра, принятого от мастера
void OSL_Mdb_RTU_TCP_Slave_Request_Rx(Modbus_Params_T *p)
{
	p->Error.all = 0;			// Сброс аварий текущего пакета
	p->Exception = MDB_EXC_CODE_FREE;

	// Проверка на минимальное количество байт
	if (p->Bytes_Delta < 5)
	{
		p->Error.bit.Short_Packet = true;
		p->Receive_State = MDB_REC_STATE_ERROR;
		return;
	}

  if (p->Rx_Buff == NULL)
  {
    p->Error.bit.Rx_Buf_Pointer = true;
    p->Receive_State = MDB_REC_STATE_ERROR;
    return; 
  }

	p->i = 0;
	if (p->Mdb_Protocol == MDB_PROTOCOL_TCP)
	{
		//Transaction Identifier
		p->Rx_Params.Trans_Ident = (uint16_t)(p->Rx_Buff[p->i]);
		p->Rx_Params.Trans_Ident += (uint16_t)(p->Rx_Buff[++p->i]<< 8);
			
		//Protocol Identifier
		p->Rx_Params.Prot_Ident = (uint16_t)(p->Rx_Buff[++p->i]<< 8);
		p->Rx_Params.Prot_Ident += (uint16_t)(p->Rx_Buff[++p->i]);
		if (p->Rx_Params.Prot_Ident != MDB_PROT_IDENT)
		{
			p->Error.bit.Protocol_Mdb = true;
			p->Receive_State = MDB_REC_STATE_ERROR;
			return;
		}

		//Length
		p->Rx_Params.Length = (uint16_t)(p->Rx_Buff[++p->i]<< 8);
		p->Rx_Params.Length += (uint16_t)(p->Rx_Buff[++p->i]);
		
		p->Rx_Params.ID = p->Rx_Buff[++p->i];			// Unit Identifier
	}
	else p->Rx_Params.ID = p->Rx_Buff[p->i];		// Slave ID

	if (!OSL_MDB_Repeater_ID_Check_Callback(p))
	{
		p->Repeater_flag = false;
		if (p->Rx_Params.ID != p->ID_Device)
		{
			p->Error.bit.ID_Mistake = true;
			p->Receive_State = MDB_REC_STATE_ERROR;
			return;
		}
	}

	// Function code
	p->Rx_Params.Func_Code = p->Rx_Buff[++p->i];
	if (p->Rx_Params.Func_Code != FC_READ_HR &&
			p->Rx_Params.Func_Code != FC_WRITE_MR)
	{
		p->Error.bit.Unknown_Command = true;
		p->Exception = MDB_EXC_CODE_CMD;
    p->Receive_State = MDB_REC_STATE_EXCEPTION;
	}

	// Data address
	p->Rx_Params.Data_addr = p->Rx_Buff[++p->i]<< 8;
	p->Rx_Params.Data_addr += p->Rx_Buff[++p->i];

	// Number_of_registers
	p->Rx_Params.Num_reg = p->Rx_Buff[++p->i]<< 8;
	p->Rx_Params.Num_reg += p->Rx_Buff[++p->i];
	
	if (!p->Rx_Params.Num_reg)
	{
		p->Error.bit.Zero_Num_Regs = true;
		p->Receive_State = MDB_REC_STATE_ERROR;
		return;
	}

	if (p->Rx_Params.Func_Code == FC_READ_HR)
	{
		if (!p->Rx_Params.Num_reg || (p->Rx_Params.Num_reg > MAX_TX_DATA_SIZE))
		{
			p->Error.bit.Tx_Buffer_overload = true;
			p->Exception = MDB_EXC_CODE_OVER_LIM;
			p->Receive_State = MDB_REC_STATE_EXCEPTION;
		}
	}
	else if (p->Rx_Params.Func_Code == FC_WRITE_MR)
	{
		// Byte Count
		p->Rx_Params.Bytes = p->Rx_Buff[++p->i];
		
		if (!p->Rx_Params.Bytes)
		{
			p->Error.bit.Zero_Num_Bytes = true;
			p->Receive_State = MDB_REC_STATE_ERROR;
			return;
		}

		// Writing data
		if (!p->Rx_Params.Num_reg || (p->Rx_Params.Num_reg > MAX_RX_DATA_SIZE))
		{
			p->Error.bit.Rx_Buffer_overload = true;
			p->Exception = MDB_EXC_CODE_OVER_LIM;
			p->Receive_State = MDB_REC_STATE_EXCEPTION;
		}
		else
		{
			for (p->k = 0; p->k < p->Rx_Params.Num_reg; p->k++) 
			{
				p->Mdb_Rx_Data[p->k] = p->Rx_Buff[++p->i]<< 8;
				p->Mdb_Rx_Data[p->k] += p->Rx_Buff[++p->i];
			}
		}
	}

	if (p->Mdb_Protocol == MDB_PROTOCOL_RTU)
	{
		// CRC
		p->Rx_Params.CRC_Res = p->Rx_Buff[++p->i] << 8;
		p->Rx_Params.CRC_Res += p->Rx_Buff[++p->i];

		if (CRC16(p->Rx_Buff, --p->i) != p->Rx_Params.CRC_Res)
		{
			p->Error.bit.CRC_Mistake = true;
			p->Receive_State = MDB_REC_STATE_ERROR;
			return;
		}
		else if (!p->Exception) p->Receive_State = MDB_REC_STATE_SUCCESSFUL;
	}
	else if (!p->Exception) p->Receive_State = MDB_REC_STATE_SUCCESSFUL;
}

//Разбор кадра Slave
void OSL_Mdb_Slave_Packet_Check(Modbus_Params_T *p)
{
	OSL_Mdb_RTU_TCP_Slave_Request_Rx(p);
	if (p->Receive_State == MDB_REC_STATE_SUCCESSFUL)							// Если на данный запрос требуется штатный
	{
		if (p->Repeater_flag)
		{
			// Необходмо перенаправить запрос другому Modbus
			if (!OSL_MDB_SL_Repeater_Get_Ok_Callback(p))
			{
				p->Repeater_flag = false;
				p->Error.bit.Unexpected_Rx = 1;
				p->Receive_State = MDB_REC_STATE_ERROR;
				return;
			}
			p->Receive_State = MDB_REC_STATE_REPEATER_MODE;
		}
		else if (OSL_Modbus_Var_Search(p) == PARAM_FOUND_OK)				// Если адрес найден, проверяем его на диапазон
		{
			p->Mdb_Conv_Data.Addr = p->Addr;
			p->Mdb_Conv_Data.Num_reg = p->Rx_Params.Num_reg;
			OSL_Modbus_Size_Convert(p, &p->Mdb_Conv_Data);
			if (p->Error.all)
			{
				if (p->Error.bit.No_access)	p->Exception = MDB_EXC_CODE_NO_ACCESS;
				else p->Exception = MDB_EXC_CODE_OVER_LIM;

				p->Receive_State = MDB_REC_STATE_EXCEPTION;
				OSL_Mdb_RTU_TCP_Slave_Exception_Tx(p);
			}
			else OSL_Mdb_RTU_TCP_Slave_Successful_Tx(p);
		}
	#if defined (MODBUS_GRAPHIC)
		else if ((p->Graphic != NULL) && p->Graphic->Monitor_flag &&
						 (p->Rx_Params.Data_addr >= p->Graphic->Addr_CH1 && p->Rx_Params.Data_addr < p->Graphic->Addr_CH2+p->Graphic->Buffer_size))
		{
			OSL_Modbus_Graphics_Send(p);
		}
	#endif
		// Проверка адреса на соответсвие диапазону. Если это переменные подчиненного устройства, передаем запрос ему
		// Адреса должны различаться с адресами графиков
		else if (OSL_MDB_SL_Data_Frame_Callback(p))  	// Дополнительная проверка корренного кадра
		{
			p->Receive_State = MDB_REC_STATE_DELAYED_ANSWER;
			return;
		}
		else
		{
			p->Error.bit.No_access = true;
			p->Exception = MDB_EXC_CODE_NO_ACCESS;
			OSL_Mdb_RTU_TCP_Slave_Exception_Tx(p);
		}
	}
	else if (p->Receive_State == MDB_REC_STATE_EXCEPTION)
	{
		if (p->Repeater_flag) 
		{	
			if (!OSL_MDB_SL_Repeater_Get_Ok_Callback(p))
			{
				p->Repeater_flag = false;
				p->Error.bit.Unexpected_Rx = 1;
				p->Receive_State = MDB_REC_STATE_ERROR;
				return;
			}
		}
		else OSL_Mdb_RTU_TCP_Slave_Exception_Tx(p);
		p->Mdb_Timer_Exception++;
	}
	else if (p->Receive_State == MDB_REC_STATE_ERROR)
	{
		p->Repeater_flag = false;
		p->Mdb_Timer_No_Answer++;
	}
}

// Функция формирования штатного ответа
void OSL_Mdb_RTU_TCP_Slave_Successful_Tx(Modbus_Params_T *p)
{
	p->Mdb_Timer_Success_Answer++;
	p->i = 0;
	
	if (p->Mdb_Protocol == MDB_PROTOCOL_TCP)
	{
		//Transaction
		p->Tx_Buff[p->i] = (uint8_t)(p->Rx_Params.Trans_Ident & 0xFF);
		p->Tx_Buff[++p->i] = ((uint8_t)((p->Rx_Params.Trans_Ident & 0xFF00)>>8));

		//Protocol Identifier
		p->Tx_Buff[++p->i] = ((uint8_t)((p->Rx_Params.Prot_Ident & 0xFF00)>>8));
		p->Tx_Buff[++p->i] = (uint8_t)(p->Rx_Params.Prot_Ident & 0xFF);

		//
		p->Tx_Params.Length = (p->Rx_Params.Num_reg * 2) + 3;
		p->Tx_Buff[++p->i] = (uint8_t)((p->Tx_Params.Length & 0xFF00)>>8);
		p->Tx_Buff[++p->i] = (uint8_t)(p->Tx_Params.Length & 0xFF);
		
		p->Tx_Buff[++p->i] = p->ID_Device;
	}
	else p->Tx_Buff[p->i] = p->ID_Device;

	//Function
	p->Tx_Buff[++p->i] = p->Rx_Params.Func_Code;
	
	if (p->Rx_Params.Func_Code == FC_READ_HR)
	{
		p->Tx_Buff[++p->i] = p->Rx_Params.Num_reg * 2;													// Byte Count																				

		for (p->k=0; p->k < p->Rx_Params.Num_reg; p->k++)												// Register value
		{
			p->Tx_Buff[++p->i] = (uint8_t)((p->Mdb_Tx_Data[p->k]&0xFF00)>>8);			// Register value Hi
			p->Tx_Buff[++p->i] = (uint8_t)(p->Mdb_Tx_Data[p->k]&0xFF);						// Register value Lo
		}
	}
	else if (p->Rx_Params.Func_Code == FC_WRITE_MR)
	{
		// Address
		p->Tx_Buff[++p->i] = (uint8_t)((p->Rx_Params.Data_addr&0xFF00)>>8);
		p->Tx_Buff[++p->i] = (uint8_t)(p->Rx_Params.Data_addr&0xFF);
		
		// Number of registers
		p->Tx_Buff[++p->i] = (uint8_t)((p->Rx_Params.Num_reg&0xFF00)>>8);
		p->Tx_Buff[++p->i] = (uint8_t)(p->Rx_Params.Num_reg&0xFF);
	}
	
	if (p->Mdb_Protocol == MDB_PROTOCOL_RTU)
	{
		p->Tx_Params.CRC_Res = CRC16(p->Tx_Buff, ++p->i);											// CRC
		p->Tx_Buff[p->i] = (uint8_t)((p->Tx_Params.CRC_Res&0xFF00)>>8);				// Старшая часть
		p->Tx_Buff[++p->i] = (uint8_t)(p->Tx_Params.CRC_Res&0xFF);						// Младшая часть
	}

	OSL_Modbus_to_Interface_Tx(p, ++p->i);
}

// Функия формирования ответа об исключении
void OSL_Mdb_RTU_TCP_Slave_Exception_Tx(Modbus_Params_T *p)
{
	p->Mdb_Timer_Exception++;
	p->i = 0;

	if (p->Mdb_Protocol == MDB_PROTOCOL_TCP)
	{
		//Transaction
		p->Tx_Buff[p->i] = (uint8_t)(p->Rx_Params.Trans_Ident&0xFF);
		p->Tx_Buff[++p->i] = (uint8_t)((p->Rx_Params.Trans_Ident&0xFF)>>8);
		
		//Protocol
		p->Tx_Buff[++p->i] = (uint8_t)(p->Rx_Params.Prot_Ident&0xFF);
		p->Tx_Buff[++p->i] = (uint8_t)((p->Rx_Params.Prot_Ident&0xFF)>>8);
		
		//Length
		p->Tx_Buff[++p->i] = 0;
		p->Tx_Buff[++p->i] = 3;
		
		p->Tx_Buff[++p->i] = p->ID_Device;
	}
	else p->Tx_Buff[p->i] = p->ID_Device;														// ID
	
	p->Tx_Buff[++p->i] = p->Rx_Params.Func_Code + 0x80;							// Команда + старший бит ошибки
	p->Tx_Buff[++p->i] = p->Exception;															// Exception

	if (p->Mdb_Protocol == MDB_PROTOCOL_RTU)
	{
		// CRC
		p->Tx_Params.CRC_Res = CRC16(p->Tx_Buff, ++p->i);	
		p->Tx_Buff[p->i] = (uint8_t)((p->Tx_Params.CRC_Res&0xFF00)>>8);		// Старшая часть
		p->Tx_Buff[++p->i] = (uint8_t)(p->Tx_Params.CRC_Res&0xFF);				// Младшая часть
	}
	OSL_Modbus_to_Interface_Tx(p, ++p->i);
}
#endif
/**************************** Modbus Slave ****************************/

/**************************** Modbus Master ***************************/
#if defined(MODBUS_MASTER_USED)
__weak bool OSL_MDB_MAS_Manual_Send_Callback(Modbus_Params_T *p, uint16_t Addr){return 0;}
__weak bool OSL_MDB_MAS_Manual_Get_Callback(Modbus_Params_T *p, uint16_t Addr){return 0;}
__weak void OSL_MDB_MAS_Exception_Callback(Modbus_Params_T *p){}
__weak bool OSL_MDB_MAS_Repeater_Get_Ok_Callback(Modbus_Params_T *p){return 0;}
__weak void OSL_MDB_MAS_TimeOut_NoAnswer(Modbus_Params_T *p, uint16_t Addr){}

// Колбек при сформированной отправке запроса слейву
__weak void OSL_Modbus_Master_Request_Callback(Modbus_Params_T *p, uint16_t Addr){};
static bool OSL_Modbus_Master_Change_ID_Par(Modbus_Params_T *p);

//Функция формирования периодических запросов мастером слейву
void OSL_Modbus_Master_Timeout_Request(Modbus_Params_T *p, TIMx_Params_T *v)
{
	if (p->Mas_Par->TIM_Answer_Time.State == TIME_CALC_ACTIVE)
	{
		OSL_TIM_Time_Calc (&p->Mas_Par->TIM_Answer_Time, v, VIEW_RESULT);													// Чтение значения таймера
		if (p->Mas_Par->TIM_Answer_Time.Result_time_us > p->Mas_Par->Answer_Lim_Time)							// Если время ожидания приема вышло
		{
			OSL_TIM_Time_Calc(&p->Mas_Par->TIM_Answer_Time, v, TIME_CALC_ACTIVE);
			if (p->Mas_Par->Mas_Request_flag)
			{
				if (!p->Manual_Request)
				{
					if ((p->Receive_State != MDB_REC_STATE_SUCCESSFUL   ) &&
							(p->Receive_State != MDB_REC_STATE_EXCEPTION    ))
					{
						OSL_MDB_MAS_TimeOut_NoAnswer(p, p->Mas_Par->Par_timer);
						p->Mdb_Timer_No_Answer++;			// Ответ не был получен за тайм-аут
					}
				}
				else if (p->Receive_State != MDB_REC_STATE_MANUAL_MODE)
				{
					OSL_MDB_MAS_TimeOut_NoAnswer(p, p->Mas_Par->Par_timer);
					p->Mdb_Timer_No_Answer++;			// Ответ не был получен за тайм-аут					
					p->Mas_Par->Oper_Request[p->Mas_Par->Par_timer].Par_Allow = false;
				}
				
				/*
				if (!p->Manual_Request)
				{
					if ((p->Receive_State != MDB_REC_STATE_SUCCESSFUL   ) &&
							(p->Receive_State != MDB_REC_STATE_EXCEPTION    ))
					{
						OSL_MDB_MAS_TimeOut_NoAnswer(p, p->Mas_Par->Par_timer);
						p->Mdb_Timer_No_Answer++;			// Ответ не был получен за тайм-аут
					}
				}
				else if (p->Receive_State != MDB_REC_STATE_MANUAL_MODE)
					p->Mas_Par->Oper_Request[p->Mas_Par->Par_timer].Par_Allow = false;
				*/

				p->Manual_Request = false;
			}
		
			if (OSL_Modbus_Master_Change_ID_Par(p))																										// Ищем регистр, разрешенный для запроса
			{
				if (!p->Manual_Request) OSL_Modbus_Master_Request(p);																		// Делаем запрос слейву согласно списку адресов
				else
				{
					if (!OSL_MDB_MAS_Manual_Send_Callback(p, p->Mas_Par->Par_timer))											// Делаем запрос слейву, формируемый пользователем
					{
						p->Mas_Par->Oper_Request[p->Mas_Par->Par_timer].Par_Allow = false;						
						p->Receive_State = MDB_REC_STATE_FREE;
						return;
					}
					p->Receive_State = MDB_REC_STATE_WAITING_ANSWER;																// Меняем статус на "Ожидание ответа"
					p->Mas_Par->Mas_Request_flag = true;
				}
			}
		}
	}
	else
	{
		p->Repeater_flag = false; // Режиме ретранслятора не допускается при использовании данной функции
		OSL_TIM_Time_Calc(&p->Mas_Par->TIM_Answer_Time, v, TIME_CALC_ACTIVE);
	}
}

// Функция изменения ID(не реализовано) и параметра для дальнейшего формирования запроса
// Возврат 0 - Параметр для запроса не найден(все параметры запрещены для запроса)
static bool OSL_Modbus_Master_Change_ID_Par(Modbus_Params_T *p)
{
	if (p->Mas_Par->Mas_Request_flag)				// Если ранее был запрос
	{
		p->Mas_Par->Par_State[p->Mas_Par->Par_timer].Error = p->Error;
		p->Mas_Par->Par_State[p->Mas_Par->Par_timer].Exception = p->Exception;
		p->Mas_Par->Par_State[p->Mas_Par->Par_timer].Receive_State = p->Receive_State;
		p->Mas_Par->Mas_Request_flag = false;
	}
	
	// Проверяем все параметры из списка на разрешение его запроса
	p->Mas_Par->tmp_Par = 0;
	do
	{
		p->Mas_Par->tmp_Par++;
		p->Mas_Par->Par_timer += p->Mas_Par->Oper_Request[p->Mas_Par->Par_timer].Var_Num;
		if (p->Mas_Par->Par_timer >= MODBUS_MASTER_PAR_NUM) p->Mas_Par->Par_timer = 0;

		if (p->Mas_Par->Oper_Request[p->Mas_Par->Par_timer].Cmd == MDB_MAS_REG_CMD_EN)
		{
			p->Mas_Par->Oper_Request[p->Mas_Par->Par_timer].Cmd = MDB_MAS_REG_CMD_FREE;
			p->Mas_Par->Oper_Request[p->Mas_Par->Par_timer].Par_Allow = 1;
		}
		else if (p->Mas_Par->Oper_Request[p->Mas_Par->Par_timer].Cmd == MDB_MAS_REG_CMD_DIS)
		{
			p->Mas_Par->Oper_Request[p->Mas_Par->Par_timer].Cmd = MDB_MAS_REG_CMD_FREE;
			p->Mas_Par->Oper_Request[p->Mas_Par->Par_timer].Par_Allow = 0;
		}
		
		// Если параметр разрешен для запроса
		if (p->Mas_Par->Oper_Request[p->Mas_Par->Par_timer].Par_Allow)
		{
			OSL_Modbus_Master_Request_Callback(p, p->Mas_Par->Par_timer);
			return 1;
		}
	}
	while (p->Mas_Par->tmp_Par < MODBUS_MASTER_PAR_NUM);
	return 0;
}

//Функция перекладки данных о выбранном в gRam регистре в переменные Modbus
void OSL_Modbus_Master_Request(Modbus_Params_T *p)
{
	p->Tx_Params.ID = p->ID_Device;																																								// ID
	p->Tx_Params.Data_addr = g_Ram_modbus_M[p->Mas_Par->Par_timer].Addr;																					// Par Addr
	
	// Command
	if (g_Ram_modbus_M[p->Mas_Par->Par_timer].Size->bit.Mdb_Mas_Write_Allowed) p->Tx_Params.Func_Code = FC_WRITE_MR;
	else p->Tx_Params.Func_Code = FC_READ_HR;

	p->Tx_Params.Num_reg = 0;
	for (p->i = 0; p->i < p->Mas_Par->Oper_Request[p->Mas_Par->Par_timer].Var_Num; p->i++)
	{
		if ((g_Ram_modbus_M[p->Mas_Par->Par_timer+p->i].Size->all) & GVAR_T_32) p->Tx_Params.Num_reg += 2;										// Num Regs
		else if ((g_Ram_modbus_M[p->Mas_Par->Par_timer+p->i].Size->all) & (GVAR_T_FL|GVAR_PR)) p->Tx_Params.Num_reg += 2;		// Num Regs
		else p->Tx_Params.Num_reg += 1;
	}
	
	p->Mas_Par->Mas_Conv_Data.Addr = p->Mas_Par->Par_timer;
	p->Mas_Par->Mas_Conv_Data.Num_reg = p->Tx_Params.Num_reg;															// Число регистров
	OSL_Modbus_Size_Convert(p, &p->Mas_Par->Mas_Conv_Data);																// Формирование данных на отправку
	
	p->Mas_Par->Mas_Request_flag = true;
	p->Receive_State = MDB_REC_STATE_WAITING_ANSWER;																			// Меняем статус на "Ожидание ответа"
	OSL_Modbus_Master_Tx(p);																															// Отправка запроса
}

//Функция перекладки переменных Modbus в массив для отправки интерфейсом
void OSL_Modbus_Master_Tx(Modbus_Params_T *p)
{
	p->i = 0;
	// ID	
	p->Tx_Buff[p->i] = p->Tx_Params.ID;

	// Command
	p->Tx_Buff[++p->i] = p->Tx_Params.Func_Code;

	// Data address
	p->Tx_Buff[++p->i] = (uint8_t)((p->Tx_Params.Data_addr&0xFF00)>>8);												// Старшая часть
	p->Tx_Buff[++p->i] = (uint8_t) (p->Tx_Params.Data_addr&0xFF);			
	
	// Number_of_registers
	p->Tx_Buff[++p->i] = (uint8_t)((p->Tx_Params.Num_reg&0xFF00)>>8);													// Старшая часть
	p->Tx_Buff[++p->i] = (uint8_t) (p->Tx_Params.Num_reg&0xFF);

	if (p->Tx_Params.Func_Code == FC_WRITE_MR)
	{
		// Bytes
		p->Tx_Params.Bytes = p->Tx_Params.Num_reg*2;
		p->Tx_Buff[++p->i] = p->Tx_Params.Bytes;

		for (p->k=0; p->k < p->Tx_Params.Num_reg; p->k++)																				// Посылаемые данные
		{
			p->Tx_Buff[++p->i] = (uint8_t)((p->Mdb_Tx_Data[p->k]&0xFF00)>>8);											// Старшая часть
			p->Tx_Buff[++p->i] = (uint8_t)(p->Mdb_Tx_Data[p->k]&0xFF);														// Младшая часть
		}
	}

	p->Tx_Params.CRC_Res = CRC16(p->Tx_Buff, ++p->i);	
	p->Tx_Buff[p->i] = (uint8_t)((p->Tx_Params.CRC_Res&0xFF00)>>8);														// Старшая часть
	p->Tx_Buff[++p->i] = (uint8_t)(p->Tx_Params.CRC_Res&0xFF);																// Младшая часть
	
	OSL_Modbus_to_Interface_Tx(p, ++p->i);
}

__weak void OSL_Modbus_Master_Successful_Request_Callback(Modbus_Params_T *p){}

// Проверка и обработка принятого пакета мастером
void OSL_Modbus_Master_Packet_Check(Modbus_Params_T *p)
{
	if (p->Mas_Par->Oper_Request[p->Mas_Par->Par_timer].Par_Allow)
	{
		OSL_Modbus_Master_Rx(p);
		if (p->Receive_State == MDB_REC_STATE_SUCCESSFUL)				// Если на данный запрос требуется штатный
		{
			if (p->Repeater_flag)
			{
				if (!OSL_MDB_MAS_Repeater_Get_Ok_Callback(p))
				{
					p->Error.bit.Unexpected_Rx = true;
					p->Receive_State = MDB_REC_STATE_ERROR;
					return;
				}
				p->Receive_State = MDB_REC_STATE_REPEATER_MODE;
				p->Mdb_Timer_Success_Answer++;
			}
			else if (p->Manual_Request)
			{
				if (!OSL_MDB_MAS_Manual_Get_Callback(p, p->Mas_Par->Par_timer)) 
				{
					p->Error.bit.Unexpected_Rx = true;
					p->Receive_State = MDB_REC_STATE_ERROR;
					return;
				}
				p->Receive_State = MDB_REC_STATE_MANUAL_MODE;
				p->Mdb_Timer_Success_Answer++;
			}
			#if defined(MODBUS_MASTER_PAR_NUM)
			//else if (OSL_Modbus_Var_Search(p) == PARAM_FOUND_OK)								// Если адрес найдет, проверяем его на диапазон
			else if (p->Tx_Params.Data_addr == p->Mdb_Conv_Data.g_Ram_modbus_x[p->Mas_Par->Par_timer].Addr)
			{
				p->Addr = p->Mas_Par->Par_timer;
				p->Mdb_Conv_Data.Addr = p->Mas_Par->Par_timer;
				p->Mdb_Conv_Data.Num_reg = p->Rx_Params.Num_reg;
				OSL_Modbus_Size_Convert(p, &p->Mdb_Conv_Data);
				if (!p->Error.all)
				{
					p->Mdb_Timer_Success_Answer++;
					if (p->Mdb_Conv_Data.g_Ram_modbus_x[p->Addr].Size->bit.Mdb_Mas_Write_Allowed)			// Если параметр был для записи
					{
						if (!p->Mdb_Conv_Data.g_Ram_modbus_x[p->Addr].Size->bit.Mdb_Write_Always)				// Если параметр был для разовой записи
						{
							p->Mas_Par->Oper_Request[p->Mas_Par->Par_timer].Par_Allow = false;
						}
					}
					OSL_Modbus_Master_Successful_Request_Callback(p);						// Колбек успешного приема
				}
				else
				{
					p->Error.bit.Unexpected_Rx = 1;
					p->Receive_State = MDB_REC_STATE_ERROR;
					p->Mdb_Timer_No_Answer++;
				}
			}
			#endif
			else
			{
				p->Error.bit.Unexpected_Rx = 1;
				p->Receive_State = MDB_REC_STATE_ERROR;
				p->Mdb_Timer_No_Answer++;
			}
		}
		else if (p->Receive_State == MDB_REC_STATE_EXCEPTION)
		{
			if (p->Repeater_flag) 
			{
				if (!OSL_MDB_MAS_Repeater_Get_Ok_Callback(p))
				{
					p->Repeater_flag = false;
					p->Error.bit.Unexpected_Rx = 1;
					p->Receive_State = MDB_REC_STATE_ERROR;
					p->Mdb_Timer_No_Answer++;
					return;
				}
			}
			else if (p->Manual_Request)
			{
				if (!OSL_MDB_MAS_Manual_Get_Callback(p, p->Mas_Par->Par_timer))
				{
					p->Repeater_flag = false;
					p->Error.bit.Unexpected_Rx = 1;
					p->Receive_State = MDB_REC_STATE_ERROR;
					p->Mdb_Timer_No_Answer++;
					return;
				}
			}
			p->Addr = p->Mas_Par->Par_timer;
			OSL_MDB_MAS_Exception_Callback(p);
			p->Mdb_Timer_Exception++;
		}
		else if (p->Receive_State == MDB_REC_STATE_ERROR)
		{
			p->Repeater_flag = false;
			p->Mdb_Timer_No_Answer++;
		}
	}
	else
	{
		p->Error.all = 0;
		p->Error.bit.Unexpected_Rx = 1;
		p->Manual_Request = false;
		p->Repeater_flag = false;
		p->Receive_State = MDB_REC_STATE_ERROR;
		p->Mdb_Timer_No_Answer++;
	}
}

// Функция разбора кадра, принятого от слейва
void OSL_Modbus_Master_Rx(Modbus_Params_T *p)
{
	p->Error.all = 0;																// Сброс аварий текущего пакета
	p->Exception = MDB_EXC_CODE_FREE;			// Сброс исключений текущего пакета

	// Minimum Receive Bytes
	if (p->Bytes_Delta < 5)
	{
		p->Error.bit.Short_Packet = true;
		p->Receive_State = MDB_REC_STATE_ERROR;
		return;
	}
	
	// ID
	p->i = 0;
	p->Rx_Params.ID = p->Rx_Buff[p->i];
	
	if (!OSL_MDB_Repeater_ID_Check_Callback(p)) //p->Repeater_flag = true;
	{
		p->Repeater_flag = false;
		if (p->Rx_Params.ID != p->ID_Device)
		{
			p->Error.bit.ID_Mistake = true;
			p->Receive_State = MDB_REC_STATE_ERROR;
			return;
		}
	}

	// Command
	p->Rx_Params.Func_Code = p->Rx_Buff[++p->i] & 0x7F;														// Забираем команду без учета исключения
	
	if (!p->Manual_Request && !p->Repeater_flag)
	{
		if (p->Tx_Params.Func_Code != p->Rx_Params.Func_Code)
		{
			p->Error.bit.Unexpected_Rx = true;
			p->Receive_State = MDB_REC_STATE_ERROR;
			return;
		}
	}
	
	if (p->Rx_Params.Func_Code != FC_READ_HR && p->Rx_Params.Func_Code != FC_WRITE_MR)
	{
		p->Error.bit.Unknown_Command = true;
		p->Receive_State = MDB_REC_STATE_ERROR;
		return;
	}
	
	//Excepition
	if (p->Rx_Buff[p->i] & 0x80)
	{
		p->Exception = (MDB_EXC_CODE_T)p->Rx_Buff[++p->i];
	
		if (p->Exception != MDB_EXC_CODE_NO_ACCESS &&
				p->Exception != MDB_EXC_CODE_OVER_LIM)
		{
			p->Error.bit.Unknown_Exception = true;
			p->Receive_State = MDB_REC_STATE_ERROR;
			return;
		}
		else p->Receive_State = MDB_REC_STATE_EXCEPTION;
	}
	else if (p->Rx_Params.Func_Code == FC_READ_HR)
	{
		// Bytes
		p->Rx_Params.Bytes = p->Rx_Buff[++p->i];

		if (!p->Rx_Params.Bytes)
		{
			p->Error.bit.Zero_Num_Bytes = true;
			p->Receive_State = MDB_REC_STATE_ERROR;
			return;
		}
		
		// Num Regs
		p->Rx_Params.Num_reg = p->Rx_Params.Bytes/2;

		// Reading data
		if (p->Rx_Params.Num_reg <= MAX_RX_DATA_SIZE)
		{
			for (p->k = 0; p->k < p->Rx_Params.Num_reg; p->k++)
			{
				p->Mdb_Rx_Data[p->k] = p->Rx_Buff[++p->i]<< 8;
				p->Mdb_Rx_Data[p->k] += p->Rx_Buff[++p->i];
			}
		}
		else
		{
			p->Error.bit.Rx_Buffer_overload = true;
			//p->Exception = MODBUS_EXCEPTION_CODE_OVER_LIM;
			return;
		}
	}
	else if (p->Rx_Params.Func_Code == FC_WRITE_MR)
	{
		// Data address	
		p->Rx_Params.Data_addr = p->Rx_Buff[++p->i]<< 8;
		p->Rx_Params.Data_addr += p->Rx_Buff[++p->i];

		// Number_of_registers
		p->Rx_Params.Num_reg = p->Rx_Buff[++p->i]<< 8;
		p->Rx_Params.Num_reg += p->Rx_Buff[++p->i];					
	}
	
	// CRC
	p->Rx_Params.CRC_Res = p->Rx_Buff[++p->i] << 8;
	p->Rx_Params.CRC_Res += p->Rx_Buff[++p->i];

	if (CRC16(p->Rx_Buff, --p->i) != p->Rx_Params.CRC_Res)
	{
		p->Error.bit.CRC_Mistake = true;
		p->Receive_State = MDB_REC_STATE_ERROR;
		return;
	}
	else if (!p->Exception) p->Receive_State = MDB_REC_STATE_SUCCESSFUL;
}
#endif
/**************************** Modbus Master ***************************/

/**************************** Size ***************************/
__weak void OSL_MODBUS_AddrRequestCallback(Modbus_Params_T *p, uint16_t Addr){}
__weak void OSL_MODBUS_AddrWriteCallback(Modbus_Params_T *p, uint32_t Var_Size){}
__weak void OSL_MODBUS_AddrAllWriteCallback(Modbus_Params_T *p, uint16_t Addr){}
__weak bool OSL_MDB_Lim_Check_CB(Modbus_Params_T *p, uint32_t Var_Size){return 0;}

#if defined(EXT_EEPROM_USED)
	#define MEMORY_MODE(X_VAR)\
	{\
		if (c->g_Ram_modbus_x[c->Tmp_Sum].Size->bit.Mem_Fix)\
		{\
			OSL_EEPROM_Add_to_Writing(c->g_Ram_modbus_x[c->Tmp_Sum].Size);\
		}\
	}
#elif defined(FLASH_USED)
	#define MEMORY_MODE(X_VAR)\
	{\
		if (c->g_Ram_modbus_x[c->Tmp_Sum].Size->bit.Mem_Fix) *p->Par_Rewrite_flag = true;\
	}
#else
	#define MEMORY_MODE(X_VAR)
#endif
	
#define MDB_CHECK_DATA_DEF(X_VAR, SIZE)\
{\
	if (p->Role == MDB_ROLE_SLAVE)\
	{\
		if (p->Rx_Params.Func_Code == FC_WRITE_MR)\
		{\
			if (!c->g_Ram_modbus_x[c->Tmp_Sum].Size->all & GVAR_M_W)\
			{\
				p->Error.bit.No_access = true;\
				return;\
			}\
			/* Если параметр не доступен для записи*/\
			if (!c->g_Ram_modbus_x[c->Tmp_Sum].Size->bit.Mdb_Sl_Write_Allowed)\
			{\
				p->Error.bit.No_access = true;\
				return;\
			}\
			/* Если превышен предел записи*/\
			if (!(X_VAR.Data >= X_VAR.Min && X_VAR.Data <= X_VAR.Max) || OSL_MDB_Lim_Check_CB(p, (uint32_t)SIZE))\
			{\
				p->Error.bit.Over_Lim = true;\
				return;\
			}\
			/* Если данные отличаются*/\
			if ( (*X_VAR.Value != X_VAR.Data) || c->g_Ram_modbus_x[c->Tmp_Sum].Size->bit.Mdb_Write_Always )\
			{\
				*X_VAR.Value = X_VAR.Data;\
				/* Запись разрешена*/\
				c->write_flag = true;	/*Выставляем флаг, что данные записывались*/\
				OSL_MODBUS_AddrWriteCallback(p, (uint32_t)SIZE);\
				OSL_Global_Par_Update_CB((uint32_t)SIZE, (uint32_t)p);/* Колбек записи в адрес (RW_M)*/\
				MEMORY_MODE(X_VAR);\
			}\
		}\
	}\
	else\
	{\
		if ((p->Rx_Params.Func_Code == FC_READ_HR) && (*X_VAR.Value != X_VAR.Data)) *X_VAR.Value = X_VAR.Data;\
	}\
}

#define STRUCT_VALUE_LOAD(X_VAR, TYPE){\
	switch (c->g_Ram_modbus_x[c->Tmp_Sum].Size->all & GVAR_ONLY_WPD)\
	{\
		case GVAR_M_P:				X_VAR.Value =  (*(OSL_Var_RO_##TYPE##_P_T	*)c->g_Ram_modbus_x[c->Tmp_Sum].Size).Var; break;\
		case GVAR_M_W:				X_VAR.Value =	&(*(OSL_Var_RW_##TYPE##_T		*)c->g_Ram_modbus_x[c->Tmp_Sum].Size).Var;\
													X_VAR.Min		=	 (*(OSL_Var_RW_##TYPE##_T		*)c->g_Ram_modbus_x[c->Tmp_Sum].Size).Min;\
													X_VAR.Max		=	 (*(OSL_Var_RW_##TYPE##_T		*)c->g_Ram_modbus_x[c->Tmp_Sum].Size).Max; break;\
		case GVAR_M_WP:				X_VAR.Value =	 (*(OSL_Var_RW_##TYPE##_P_T	*)c->g_Ram_modbus_x[c->Tmp_Sum].Size).Var;\
													X_VAR.Min		=	 (*(OSL_Var_RW_##TYPE##_P_T	*)c->g_Ram_modbus_x[c->Tmp_Sum].Size).Min;\
													X_VAR.Max		=	 (*(OSL_Var_RW_##TYPE##_P_T	*)c->g_Ram_modbus_x[c->Tmp_Sum].Size).Max; break;\
		case GVAR_M_WD:				X_VAR.Value =	&(*(OSL_Var_RW_##TYPE##_D_T	*)c->g_Ram_modbus_x[c->Tmp_Sum].Size).Var;\
													X_VAR.Min		=	 (*(OSL_Var_RW_##TYPE##_D_T	*)c->g_Ram_modbus_x[c->Tmp_Sum].Size).Min;\
													X_VAR.Max		=	 (*(OSL_Var_RW_##TYPE##_D_T	*)c->g_Ram_modbus_x[c->Tmp_Sum].Size).Max; break;\
		case GVAR_M_WPD:			X_VAR.Value =	 (*(OSL_Var_RW_##TYPE##_PD_T*)c->g_Ram_modbus_x[c->Tmp_Sum].Size).Var;\
													X_VAR.Min		=	 (*(OSL_Var_RW_##TYPE##_PD_T*)c->g_Ram_modbus_x[c->Tmp_Sum].Size).Min;\
													X_VAR.Max		=	 (*(OSL_Var_RW_##TYPE##_PD_T*)c->g_Ram_modbus_x[c->Tmp_Sum].Size).Max; break;\
		default:							X_VAR.Value =	&(*(OSL_Var_RO_##TYPE##_T		*)c->g_Ram_modbus_x[c->Tmp_Sum].Size).Var; break;\
	}\
}

void OSL_Modbus_Size_Convert(Modbus_Params_T *p, Modbus_Conv_Data_T *c)
{
	c->write_flag = false;
	c->Var_Timer = 0;

	for (uint8_t i = 0; i < c->Num_reg; i++)
	{
		c->Tmp_Sum = c->Addr+c->Var_Timer;
		
		if (!c->Conv_only_flag)
		{
			if (c->Tmp_Sum > (c->g_Ram_modbus_Size_x-1))
			{
				p->Error.bit.No_access = true;
				return;
			}
		}
		
		if (c->g_Ram_modbus_x[c->Tmp_Sum].Size == NULL)
		{
				p->Error.bit.Unexpected_Rx = true;
				return;
		}
		
		switch (c->g_Ram_modbus_x[c->Tmp_Sum].Size->all & GVAR_ONLY_TYPES_AND_SIGN)
		{
			// int8_t
			case (GVAR_T_8|GVAR_T_S):
			{
				STRUCT_VALUE_LOAD(c->tmp_str_8, INT8);

				if (!c->Conv_only_flag)	// Если данные не просто конвертируются, а есть запрос
				{
					c->tmp_str_8.Data = (int16_t)p->Mdb_Rx_Data[i];
					MDB_CHECK_DATA_DEF(c->tmp_str_8, c->g_Ram_modbus_x[c->Tmp_Sum].Size);
				}
				c->Out_Data[i] = (uint16_t)(*c->tmp_str_8.Value);
			}
			break;
			
			// uint8_t
			case (GVAR_T_8):
			{
				STRUCT_VALUE_LOAD(c->tmp_str_U8, UINT8);
				if (!c->Conv_only_flag)
				{
					c->tmp_str_U8.Data = (uint16_t)p->Mdb_Rx_Data[i];
					MDB_CHECK_DATA_DEF(c->tmp_str_U8, c->g_Ram_modbus_x[c->Tmp_Sum].Size);
				}
				c->Out_Data[i] = (uint16_t)(*c->tmp_str_U8.Value);
			}
			break;
			
			// int16_t
			case (GVAR_T_16|GVAR_T_S):
			{
				STRUCT_VALUE_LOAD(c->tmp_str_16, INT16);
				if (!c->Conv_only_flag)
				{
					c->tmp_str_16.Data = (int16_t)p->Mdb_Rx_Data[i];
					MDB_CHECK_DATA_DEF(c->tmp_str_16, c->g_Ram_modbus_x[c->Tmp_Sum].Size);
				}
				c->Out_Data[i] = (uint16_t)(*c->tmp_str_16.Value);
			}
			break;

			// uint16_t
			case (GVAR_T_16):
			{
				STRUCT_VALUE_LOAD(c->tmp_str_U16, UINT16);
				if (!c->Conv_only_flag)
				{
					c->tmp_str_U16.Data = (uint16_t)p->Mdb_Rx_Data[i];
					MDB_CHECK_DATA_DEF(c->tmp_str_U16, c->g_Ram_modbus_x[c->Tmp_Sum].Size);
				}
				c->Out_Data[i] = (uint16_t)(*c->tmp_str_U16.Value);
			}
			break;
			
			// int32_t
			case (GVAR_T_32|GVAR_T_S):
			{
				STRUCT_VALUE_LOAD(c->tmp_str_32, INT32);
				if (!c->Conv_only_flag)
				{
					c->tmp_str_32.Data = (int32_t)p->Mdb_Rx_Data[i];
					c->tmp_str_32.Data += (int32_t)p->Mdb_Rx_Data[i+1]<<16;
					MDB_CHECK_DATA_DEF(c->tmp_str_32, c->g_Ram_modbus_x[c->Tmp_Sum].Size);
				}
				c->Out_Data[i] = (uint16_t)(*c->tmp_str_32.Value);
				c->Out_Data[++i] = (uint16_t)(*c->tmp_str_32.Value>>16);
			}
			break;

			// uint32_t
			case (GVAR_T_32):
			{
				STRUCT_VALUE_LOAD(c->tmp_str_U32, UINT32);
				if (!c->Conv_only_flag)
				{
					c->tmp_str_U32.Data = (uint32_t)p->Mdb_Rx_Data[i];
					c->tmp_str_U32.Data += (uint32_t)p->Mdb_Rx_Data[i+1]<<16;
					MDB_CHECK_DATA_DEF(c->tmp_str_U32, c->g_Ram_modbus_x[c->Tmp_Sum].Size);
				}
				c->Out_Data[i] = (uint16_t)(*c->tmp_str_U32.Value);
				c->Out_Data[++i] = (uint16_t)(*c->tmp_str_U32.Value>>16);
			}
			break;

			// float
			case (GVAR_T_FL):
			{
				c->tmp_str_FL.Sign_Symbols = c->g_Ram_modbus_x[c->Tmp_Sum].Size->all & GVAR_ONLY_POINTS;
				STRUCT_VALUE_LOAD(c->tmp_str_FL, FLOAT);

				if (!c->Conv_only_flag)
				{
					switch (c->tmp_str_FL.Sign_Symbols)
					{
						case GVAR_P0: c->tmp_str_FL.Data = ((float)((int16_t)p->Mdb_Rx_Data[i]));						break;
						case GVAR_P1: c->tmp_str_FL.Data = ((float)((int16_t)p->Mdb_Rx_Data[i]))/10.0F;			break;
						case GVAR_P2: c->tmp_str_FL.Data = ((float)((int16_t)p->Mdb_Rx_Data[i]))/100.0F;		break;
						case GVAR_P3: c->tmp_str_FL.Data = ((float)((int16_t)p->Mdb_Rx_Data[i]))/1000.0F;		break;
						case GVAR_PR: c->tmp_str_FL.Data = *((float*)&p->Mdb_Rx_Data[i]);										break;
						default:
            {
              if (c->tmp_str_FL.Sign_Symbols & GVAR_PR)
              {
                c->tmp_str_FL.Data = *((float*)&p->Mdb_Rx_Data[i]);										          break;
              }
              else return;
            }
					}
					MDB_CHECK_DATA_DEF(c->tmp_str_FL, c->g_Ram_modbus_x[c->Tmp_Sum].Size);
				}

				switch (c->tmp_str_FL.Sign_Symbols)
				{
					case GVAR_P0: c->Out_Data[i] = (int16_t)(*c->tmp_str_FL.Value);												break;
					case GVAR_P1: c->Out_Data[i] = (int16_t)(*c->tmp_str_FL.Value*10.0F);									break;
					case GVAR_P2: c->Out_Data[i] = (int16_t)(*c->tmp_str_FL.Value*100.0F);								break;
					case GVAR_P3: c->Out_Data[i] = (int16_t)(*c->tmp_str_FL.Value*1000.0F);								break;
					case GVAR_PR: *(float*)&c->Out_Data[i++] = *c->tmp_str_FL.Value;											break;
          default:
          {
            if (c->tmp_str_FL.Sign_Symbols & GVAR_PR)
            {
              *(float*)&c->Out_Data[i++] = *c->tmp_str_FL.Value;											          break;
            }
            else return;
          }
				}
			}
			break;
		#if defined(IQ_MATH)	
			// Q16
			case (GVAR_T_Q16):
			{
				c->tmp_str_Q16.Sign_Symbols = *c->g_Ram_modbus_x[c->Tmp_Sum].Var->Size & ONLY_POINTS;
				MDB_READ_POINTER_DEF(c->tmp_str_Q16, fix16_t);

				if (!c->Conv_only_flag)
				{
					switch (c->tmp_str_Q16.Sign_Symbols)
					{
						case POINT_0: c->tmp_str_Q16.Data = INTtoQ16(p->Mdb_Rx_Data[i]);											break;
						case POINT_1: c->tmp_str_Q16.Data = Q16_DIV(INTtoQ16(p->Mdb_Rx_Data[i]),F16(10.0));		break;
						case POINT_2: c->tmp_str_Q16.Data = Q16_DIV(INTtoQ16(p->Mdb_Rx_Data[i]),F16(100.0));	break;
						case POINT_3: c->tmp_str_Q16.Data = Q16_DIV(INTtoQ16(p->Mdb_Rx_Data[i]),F16(1000.0));	break;
						case POINT_4: c->tmp_str_Q16.Data = Q16_DIV(INTtoQ16(p->Mdb_Rx_Data[i]),F16(10000.0));break;
					}
					MDB_CHECK_DATA_DEF(c->tmp_str_Q16, fix16_t);
				}

				switch (c->tmp_str_Q16.Sign_Symbols)
				{
					case POINT_0: c->Out_Data[i] = Q16toINT(*c->tmp_str_Q16.Value);													break;
					case POINT_1: c->Out_Data[i] = Q16toINT(Q16_MUL(*c->tmp_str_Q16.Value,F16(10.0)));			break;
					case POINT_2: c->Out_Data[i] = Q16toINT(Q16_MUL(*c->tmp_str_Q16.Value,F16(100.0)));			break;
					case POINT_3: c->Out_Data[i] = Q16toINT(Q16_MUL(*c->tmp_str_Q16.Value,F16(1000.0)));		break;
					case POINT_4: c->Out_Data[i] = Q16toINT(Q16_MUL(*c->tmp_str_Q16.Value,F16(10000.0)));		break;					
				}
			}
			break;
		#endif
			
			default:
			{
				if (!c->Conv_only_flag)
				{
					p->Error.bit.Unexpected_Rx = true;
					return;
				}
			}
			break;
		}
		c->Var_Timer++;
	}
	
	if (c->Conv_only_flag) return;
	if (c->write_flag) OSL_MODBUS_AddrAllWriteCallback(p, c->Addr);				// Вызываем Колбек о записи всех данных
	OSL_MODBUS_AddrRequestCallback(p, c->Addr);														// Вызываем Колбек с запрошенным адресом
}

bool OSL_Modbus_Var_Search(Modbus_Params_T *p)
{
	uint16_t tmp_Data_Addr;
	
	if (p->Role == MDB_ROLE_SLAVE) tmp_Data_Addr = p->Rx_Params.Data_addr;
	else tmp_Data_Addr = p->Tx_Params.Data_addr;
	
	for (uint16_t i = 0; i < p->Mdb_Conv_Data.g_Ram_modbus_Size_x; i++)
	{
		if (p->Mdb_Conv_Data.g_Ram_modbus_x[i].Addr == tmp_Data_Addr)
		{
			p->Addr = i;
			return PARAM_FOUND_OK;
		}
	}
	return PARAM_NOT_FOUND_OK;
}
/**************************** Size ***************************/


/**************************** Modbus Graphic ***************************/
#if defined (MODBUS_GRAPHIC)
void OSL_Modbus_Graphic_INIT(Graphic_Buffer_T *p)
{
	for (uint16_t i = 0; i < MAX_GRAPH_BUFFER; i++)
	{
		p->Gr_Buffer[0][i] = 0;
		p->Gr_Buffer[1][i] = 0;
	}
	p->Monitor_flag = 0;
	p->Point_offset = 0;
	p->Buffer_addr = 0;
	p->During_Addr = 0;
	p->Graphic_num = 0;
//Trigger mode
	p->Allow_flag = 0;	
	p->trigger_mode = false;
	p->trigger_RefreshBuffer_CMD = false;
	p->trigger_in_process = false;
	p->trigger_buffer_rewritten = false;	
	p->during_buffer_limit = 0;
	p->buffer_shift = 0;
	p->leftover_buffer_timer = 0;
	p->round_buffer_rewrite_flag = false;
	p->Trigger_State.all = 0;
	p->trigger_multiple_16_flag = false;
	p->size_convert_rez = 0;
	//for (uint16_t i = 0; i < BUFF_STEP; i++) p->size_convert_rez[i] = 0;
	
	p->Conv_Data.Conv_only_flag = true;
	p->Conv_Data.Out_Data = &p->size_convert_rez;
	p->Conv_Data.g_Ram_modbus_Size_x = g_Ram_modbus_Size_Graph;
	p->Conv_Data.g_Ram_modbus_x = &g_Ram_modbus_Graph[0];
}

uint16_t tmp_reading_buffer_adr;

void OSL_Modbus_Graphics_Send (Modbus_Params_T *p)
{	
	if ( p->Rx_Params.Data_addr == p->Graphic->Addr_CH1 )
	{
		p->Graphic->Graphic_num = 1;
		p->Graphic->During_Addr = p->Graphic->Addr_CH1;
	}
	else if ( p->Rx_Params.Data_addr == p->Graphic->Addr_CH2 )
	{
		p->Graphic->Graphic_num = 2;
		p->Graphic->During_Addr = p->Graphic->Addr_CH2;
	}
	
	tmp_reading_buffer_adr = (p->Rx_Params.Data_addr - p->Graphic->During_Addr) + p->Graphic->buffer_shift;
	if (tmp_reading_buffer_adr > (p->Graphic->Buffer_size-1) )					// Проверяем на переход через конец буфера
	{
		tmp_reading_buffer_adr = tmp_reading_buffer_adr - (p->Graphic->Buffer_size-1);
	}
	
	for (p->k = 0; p->k < 16; p->k++)
	{
		//p->Tx_Params.Reading_Data[p->k] = p->Graphic->Gr_Buffer[p->Graphic->Graphic_num-1][p->k + (p->Rx_Params.Data_addr - p->Graphic->During_Addr) ];
		p->Mdb_Tx_Data[p->k] = p->Graphic->Gr_Buffer[p->Graphic->Graphic_num-1][p->k + tmp_reading_buffer_adr ];

	}
	OSL_Modbus_Slave_Tx(p);
}

__weak void OSL_MODBUS_BufferUpdateCallback(Modbus_Params_T *p, uint8_t graph_num){}
__weak void OSL_MODBUS_GraphicTriggerSearchCallback(Modbus_Params_T *p, uint8_t graph_num) {}	
	
void OSL_Modbus_Graphic_Update(Modbus_Params_T *p)
{
	OSL_MODBUS_GraphicTriggerSearchCallback(p, p->Graphic->SetScreen_CMD);
	
	// Обработка команды из Viewer
	if (p->Graphic->RefreshBuffer_CMD && !p->Graphic->trigger_mode)	// При приходе команды и не режим триггера
	{
		p->Graphic->Buffer_addr = 0;																		// Сброс таймера буфера
		p->Graphic->Point_offset = 0;																		// Сброс таймера пропуска точек
	}
	p->Graphic->RefreshBuffer_CMD = false;
	
	// Обработка команды от триггера
	if (p->Graphic->trigger_multiple_16_flag)																										// если текущий адрес буфера кратен 16
	{						
		if ( p->Graphic->trigger_RefreshBuffer_CMD &&																							// если  команда на обновление триггера
				 p->Graphic->trigger_mode &&																														// если  режим триггера
				!p->Graphic->trigger_in_process &&
				 p->Graphic->trigger_buffer_rewritten)																								// если триггер уже не в процессе	 																				
		{ 								
			p->Graphic->Point_offset = 0;																														// Сброс таймера пропуска точек
			p->Graphic->trigger_in_process = true;																										// Триггер в процессе
			
			p->Graphic->during_buffer_limit = ((uint16_t)((float)p->Graphic->Buffer_size*p->Graphic->trigger_part))  & (~0xF);	// Посчитали, сколько еще точек нужно доснять
			//p->Graphic->during_buffer_limit &= ~0xF;																								// Обрезаем 4 бита, чтобы буфер был заполнен кратно 16

			p->Graphic->leftover_buffer_timer = 0;																										// Обнуляем таймер счета оставшихся для записи точек	
			p->Graphic->round_buffer_rewrite_flag = false;																						// Запрещаем круговую перезапись триггера 
		}		
		p->Graphic->trigger_RefreshBuffer_CMD = false;
	}

	if (p->Graphic->Monitor_flag && p->Graphic->Allow_flag)																				// Обновляем точки, если были записаны настройки графиков
	{											
		if (!p->Graphic->trigger_mode)																															// Если триггер не активен
		{									
			if (p->Graphic->Buffer_addr < p->Graphic->Buffer_size)
			{									
				p->Graphic->Point_offset++;									
				if ( p->Graphic->Point_offset >= p->Graphic->SetPeriod_CMD )														// Пропуск точек
				{
					p->Graphic->Point_offset = 0;
					p->Graphic->Buffer_addr_tmp = p->Graphic->Buffer_addr + p->Graphic->Data_Size;
					if (p->Graphic->Buffer_addr_tmp > p->Graphic->Buffer_size)
						p->Graphic->Data_Size = p->Graphic->Buffer_addr_tmp - p->Graphic->Buffer_size;
					
					OSL_MODBUS_BufferUpdateCallback(p, p->Graphic->SetScreen_CMD);
					p->Graphic->Buffer_addr += p->Graphic->Data_Size;
					if (p->Graphic->Buffer_addr >= p->Graphic->Buffer_size) p->Graphic->Trigger_State.all = ROUND_MODE_AND_BUFFER_FULL;
				}
			}
      else p->Graphic->Trigger_State.all = ROUND_MODE_AND_BUFFER_FULL;
		}
		else
		{
			if (p->Graphic->trigger_in_process)
			{
				p->Graphic->round_buffer_rewrite_flag = false;																					// Запрещаем перезапись буфера, когда он заполнится
				if ( p->Graphic->leftover_buffer_timer < p->Graphic->during_buffer_limit )
				{																																											// Плюсуем таймер пропуска точек
					p->Graphic->Point_offset++;
					if ( p->Graphic->Point_offset >= p->Graphic->SetPeriod_CMD )													// Пропуск точек
					{
						p->Graphic->Point_offset = 0;
						OSL_MODBUS_BufferUpdateCallback(p, p->Graphic->SetScreen_CMD);
						p->Graphic->Buffer_addr += p->Graphic->Data_Size;
						if (p->Graphic->Buffer_addr >= p->Graphic->Buffer_size) p->Graphic->Buffer_addr = 0;							
						p->Graphic->leftover_buffer_timer++;
					}
					//else p->Graphic->Point_offset++;
				}
				else 
				{
          p->Graphic->Trigger_State.all = TRIG_MODE_AND_BUFF_FULL;			
					p->Graphic->buffer_shift = p->Graphic->Buffer_addr;
					p->Graphic->trigger_in_process = false;
				}
			}
			else if (p->Graphic->round_buffer_rewrite_flag)
			{
				p->Graphic->Point_offset++;
				if (p->Graphic->Point_offset >= p->Graphic->SetPeriod_CMD)
				{				
					p->Graphic->Point_offset = 0;
					OSL_MODBUS_BufferUpdateCallback(p, p->Graphic->SetScreen_CMD);
					p->Graphic->Buffer_addr += p->Graphic->Data_Size;
					if (p->Graphic->Buffer_addr >= p->Graphic->Buffer_size)
					{	
						p->Graphic->trigger_buffer_rewritten = true;
						p->Graphic->Buffer_addr = 0;
					}
//						p->Graphic->trigger_multiple_16_flag = true;
					// Если младшие 4 бита 0, значит кратно 16 (можно отработать триггер при необходимости)
					if ( p->Graphic->Buffer_addr & 0xF ) p->Graphic->trigger_multiple_16_flag = false;
					else p->Graphic->trigger_multiple_16_flag = true;
				}					
				//else p->Graphic->Point_offset++;					
			}
		}
	}
}

void OSL_MODBUS_TriggerActivate(Modbus_Params_T *p)
{
	if (p->Mode == MODBUS_MODE_SLAVE && p->Graphic)		// Убрать для ошибки!!!!!!
	{
    /*
		if (!p->Graphic->trigger_mode)
		{
			p->Graphic->Trigger_State.all = 0;
			p->Graphic->Trigger_State.bit.Round_Rewrite = true;
		}
    */
		
		if (p->Graphic->Trigger_Activate_CMD)
		{
			if ( p->Graphic->Monitor_flag &&													// если настройки загружены
					 p->Graphic->trigger_mode &&													// и если режим триггера
					!p->Graphic->trigger_in_process)										  // и триггер уже не в процессе
			{
				p->Graphic->Point_offset = 0;
				p->Graphic->trigger_multiple_16_flag = false;
				//p->Graphic->Trigger_State.all = 0;
				//p->Graphic->Trigger_State.bit.Waiting = 1;							// Ожидаем триггера
        p->Graphic->Trigger_State.all = TRIG_MODE_AND_TRIG_WAIT;
        
				p->Graphic->round_buffer_rewrite_flag = true;					// Разрешаем круговую перезапись буфера, пока триггер не сработает
				p->Graphic->Buffer_addr = 0;														// Буфер нужно полностью один раз перезаписать 
				p->Graphic->trigger_buffer_rewritten = false;					// не зависимо от срабатывания триггера																														
			}
      else if (p->Graphic->Trigger_State.bit.Round_Rewrite &&
               p->Graphic->Monitor_flag)
      {
        p->Graphic->RefreshBuffer_CMD = true;
        p->Graphic->Trigger_State.all = ROUND_MODE_AND_BUFFER_NOT_FULL;
      }
      
			p->Graphic->Trigger_Activate_CMD = 0;
		}
	}
}

bool OSL_MODBUS_TriggerSet(Modbus_Params_T *p)
{
	p->Graphic->trigger_mode = true;

	if (p->Graphic->Trigger_State.bit.Trigger_Waiting) return TRIGGER_WORK_ALLOWED;
	else return TRIGGER_WORK_BANNED;
}
#endif

/**************************** Modbus Graphic ***************************/
