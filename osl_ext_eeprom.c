/* Includes -------------------------------------------------------------------*/
#include "osl_ext_eeprom.h"
#include "BD_crc.h"

EEPROM_T EEPROM_Params;
#define E_PAR EEPROM_Params

__weak void OSL_EEPROM_ERROR_CB(void){}
__weak void OSL_EEPROM_CRC_ERROR_CB(uint16_t Array_Addr){}
__weak void OSL_EEPROM_LIM_ERROR_CB(uint16_t Array_Addr){}
__weak void OSL_EEPROM_VAR_UPDATE_CB(uint32_t Var_Addr){}

void OSL_EEPROM_Next_Op_Mode_Search(EEPROM_T *p)
{
	p->tmb_bit = 1;
  p->During_action = EEPROM_DUR_ACT_FREE;
	for (p->i = 0; p->i < 4; p->i++)
	{
		if (p->tmb_bit & p->Planned_oper_mode.all) // Ищем флаг отложенного действия
		{
			p->During_oper_mode = (EEPROM_DUR_OP_MODE_T)p->i;		// Выставляем текущий режим работы
			p->Planned_oper_mode.all &= ~p->tmb_bit;																													// Сбрасываем бит отложенного действия

			p->Par_Updated_flag = false;
			p->Repeat_par_write_timer = 0;
			p->Repeat_par_read_timer = 0;
			p->Repeat_stat_read_timer = 0;
			break;
		}
		p->tmb_bit <<= 1;																																				// Двигаем бит маски
	}
	
	// Выставляем стартовое действие
	switch (p->During_oper_mode)
	{
		// Чтение всех параметров
		case (EEPROM_DUR_OP_MODE_PAR_READING):
		{
			p->During_action = EEPROM_DUR_ACT_STAT_READING;
			p->Array_Addr = 0;
		}
		break;

		// Запись параметра(ов)
		case (EEPROM_DUR_OP_MODE_PAR_WRITING):
		{
			if (OSL_EEPROM_Var_Search(p)) p->During_action = EEPROM_DUR_ACT_FLAG_WRITING;		// Если параметр найден в списке, пишем его
      else 
      {
        E_PAR.writing_buf[E_PAR.writing_buf_index] = NULL;							// Сброс указателя текущего элемента												
				E_PAR.writing_elements_num--;																		// Убираем элемент из очереди
        p->During_action = EEPROM_DUR_ACT_FREE;
        p->During_oper_mode = EEPROM_DUR_OP_MODE_NEXT;
      }   
    }
		break;
		
		case (EEPROM_DUR_OP_MODE_DEFAULT):
		{
			p->Par_Readen_flag = 0;
			p->Array_Addr = 0;
			OSL_EEPROM_Default_Params(p);
			p->During_action = EEPROM_DUR_ACT_DATA_READING;
		}
		break;
		
		default: return;
	}
}

/* Exported function prototypes -----------------------------------------------*/
SPI_DEVICE_X_RETURN_STATE_T OSL_EEPROM_Memory_Update(void)
{
	switch (E_PAR.During_oper_mode)
	{
		case (EEPROM_DUR_OP_MODE_PAR_READING):			// Чтение всех параметров(инициализация переменных)
		case (EEPROM_DUR_OP_MODE_PAR_WRITING):			// Запись параметра(ов)
		case (EEPROM_DUR_OP_MODE_DEFAULT):					// Сброс по умолчанию
		{
			switch (E_PAR.During_action)
			{
				case EEPROM_DUR_ACT_STAT_READING: OSL_EEPROM_Read_Status_IT(&E_PAR);		break;			// Чтение статуса
				case EEPROM_DUR_ACT_DATA_READING: OSL_EEPROM_Read_Param_IT(&E_PAR);			break;			// Чтение данных
				case EEPROM_DUR_ACT_FLAG_WRITING:	OSL_EEPROM_CMD_Write_en_IT(&E_PAR);		break;			// Запись флага данных (при считывании плохих данных)
				case EEPROM_DUR_ACT_DATA_WRITING:	OSL_EEPROM_Write_Param_IT(&E_PAR);		break;			// Запись данных				
				default: return SPI_DEVICE_X_RETURN_STATE_FREE;	// SPI шина свободна
			}
			return SPI_DEVICE_X_RETURN_STATE_BUSY;						// SPI шина занята этим устройством
		}

		case (EEPROM_DUR_OP_MODE_NEXT):			// Выбор следующего режима работы
		{
			OSL_EEPROM_Next_Op_Mode_Search(&E_PAR);
			return SPI_DEVICE_X_RETURN_STATE_FREE;
		}
		
		case (EEPROM_DUR_OP_MODE_ERROR):			// Ошибка(работа с устройством блокирована)
		{
			if (E_PAR.Array_Addr < g_Ram_memory_Size-1)					// Сбрасываем оставшиеся переменные по умолчанию
			{
				E_PAR.Array_Addr++;
        E_PAR.Par_Updated_flag = false;
				OSL_EEPROM_Default_Params(&E_PAR);
			}
			else E_PAR.Par_Readen_flag = true;									// Все данные сброшены по умолчанию
			return SPI_DEVICE_X_RETURN_STATE_FREE;
		}

		default: return SPI_DEVICE_X_RETURN_STATE_FREE;	// SPI шина свободна
	}
}

void OSL_EEPROM_Callback(void)
{
	switch (E_PAR.During_action)
	{
		// Чтение статуса завершено
		case EEPROM_DUR_ACT_STAT_READING:
		{
			if (OSL_EEPROM_Read_Stat_Callback(&E_PAR) == EEPROM_READY)
			{
				E_PAR.Repeat_stat_read_timer = 0;
				E_PAR.During_action = EEPROM_DUR_ACT_DATA_READING;
			}
			else if (E_PAR.Repeat_stat_read_timer++ > EEPROM_STATUS_READ_LIMIT)
			{
				if (E_PAR.During_oper_mode != EEPROM_DUR_OP_MODE_PAR_READING) E_PAR.Array_Addr = 0;				// Текущая переменная также будет сброшена по ум-ю
				E_PAR.During_oper_mode = EEPROM_DUR_OP_MODE_ERROR;		// Ошибка, работа с EEPROM не доступна				
        E_PAR.Array_Addr_Error = E_PAR.Array_Addr;
        OSL_EEPROM_ERROR_CB();
			}
		}
		break;
		
		// Чтение данных завершено
		case EEPROM_DUR_ACT_DATA_READING:
		{
			switch (OSL_EEPROM_Read_Params_Callback(&E_PAR))
			{
				// Данные прошли проверки
				case OSL_EEPROM_MEMORY_CHECK_OK:
				{
					E_PAR.Par_Updated_flag = false;																			// Сброс флага обновления параметра							
					E_PAR.Repeat_par_write_timer = 0;																		// 
					
					switch (E_PAR.During_oper_mode)
					{
						case (EEPROM_DUR_OP_MODE_PAR_READING):
						{
							if (E_PAR.Array_Addr < g_Ram_memory_Size-1)							// Если есть еще параметры, читаем
							{
								E_PAR.During_action = EEPROM_DUR_ACT_DATA_READING;
								E_PAR.Array_Addr++;
							}
							else			// Чтение данных завершено
							{
								E_PAR.During_oper_mode = EEPROM_DUR_OP_MODE_NEXT;				// Проверяем, есть ли другие отложенные действия
								E_PAR.Array_Addr = 0;
								E_PAR.Par_Readen_flag = true;
							}
						}
						break;
						
						case (EEPROM_DUR_OP_MODE_PAR_WRITING):
						{
							for (E_PAR.i = 0; E_PAR.i < EEPROM_WRITING_BUF_SIZE; E_PAR.i++)	// Ищем элемент для записи
							{
								E_PAR.writing_buf[E_PAR.writing_buf_index] = NULL;							// Сброс указателя текущего элемента												
								E_PAR.writing_elements_num--;																		// Убираем элемент из очереди
								if (E_PAR.writing_elements_num)																	// Если элементы в очереди еще остались, ищем следующий
								{
									E_PAR.writing_buf_index++;
									// Если индекс превышает размер буфера, идем на нулевой
									if (E_PAR.writing_buf_index >= EEPROM_WRITING_BUF_SIZE) E_PAR.writing_buf_index = 0;
									if (E_PAR.writing_buf[E_PAR.writing_buf_index] != NULL) 			// Если элемент существует
									{
										E_PAR.Planned_oper_mode.bit.Par_writing = 1;								// Планируем запись параметра
										break;
									}
								}
								else break;
							}

							E_PAR.During_oper_mode = EEPROM_DUR_OP_MODE_NEXT;							// Проверяем, есть ли другие отложенные действия
						}
						break;

						case (EEPROM_DUR_OP_MODE_DEFAULT):
						{
							for (E_PAR.i = 0; E_PAR.i < g_Ram_memory_Size; E_PAR.i++)
							{
								if (E_PAR.Array_Addr < g_Ram_memory_Size-1)
								{
									E_PAR.Array_Addr++;
									if (OSL_EEPROM_Default_Params(&E_PAR))
									{
										E_PAR.During_action = EEPROM_DUR_ACT_DATA_READING;					// Будем переписывать параметр
										break;
									}
								}
								else 
								{
									E_PAR.Par_Readen_flag = 1;
									E_PAR.During_oper_mode = EEPROM_DUR_OP_MODE_NEXT;				// Проверяем, есть ли другие отложенные действия
									break;
								}
							}
						}
						break;
						
						default: break;
					}
				}
				break;
				
				// Ошибка CRC
				case OSL_EEPROM_MEMORY_CHECK_CRC_ERROR:
				{
					E_PAR.Global_CRC_error_timer++;
					OSL_EEPROM_CRC_ERROR_CB(E_PAR.Array_Addr);
					
					if (E_PAR.Repeat_par_read_timer < EEPROM_PAR_READ_LIMIT)				// Читаем повторно
					{
						E_PAR.Repeat_par_read_timer++;
						E_PAR.During_action = EEPROM_DUR_ACT_DATA_READING;
					}
					else																		// Сброс значения по умолчанию
					{
						if (E_PAR.Repeat_par_write_timer < EEPROM_PAR_WRITE_LIMIT)														// Проверка, сколько было попыток перезаписи
						{
							switch (E_PAR.During_oper_mode)
							{
								case (EEPROM_DUR_OP_MODE_PAR_READING):
								{
									if (!E_PAR.Par_Updated_flag) OSL_EEPROM_Default_Params(&E_PAR);		// Сбрасываем параметр по умолчанию, если еще не сбрасывали
								}
								break;							
								default: break;
							}
							
							E_PAR.Repeat_par_write_timer++;																		// Плюсуем таймер перезаписи параметра
							E_PAR.Repeat_par_read_timer = 0;
							E_PAR.During_action = EEPROM_DUR_ACT_FLAG_WRITING;								// Будем переписывать параметр
						}
						else
						{
							if (E_PAR.During_oper_mode != EEPROM_DUR_OP_MODE_PAR_READING) E_PAR.Array_Addr = 0;				// Текущая переменная также будет сброшена по ум-ю
							E_PAR.During_oper_mode = EEPROM_DUR_OP_MODE_ERROR;		// Ошибка, работа с EEPROM не доступна
							E_PAR.Array_Addr_Error = E_PAR.Array_Addr;
              OSL_EEPROM_ERROR_CB();
						}
					}
				}
				break;
				
				// Переменная находится за пределами
				case OSL_EEPROM_MEMORY_CHECK_LIM_ERROR:
				{
					E_PAR.Global_LIM_error_timer++;
					OSL_EEPROM_LIM_ERROR_CB(E_PAR.Array_Addr);
					if (E_PAR.Repeat_par_write_timer < EEPROM_PAR_WRITE_LIMIT)
					{
						switch (E_PAR.During_oper_mode)
						{
							case (EEPROM_DUR_OP_MODE_PAR_READING):
							{
								if (!E_PAR.Par_Updated_flag) OSL_EEPROM_Default_Params(&E_PAR);		// Сбрасываем параметр по умолчанию, если еще не сбрасывали
							}
							break;						
							default: break;
						}
						
						E_PAR.Repeat_par_write_timer++;																			// Плюсуем таймер перезаписи параметра
						E_PAR.During_action = EEPROM_DUR_ACT_FLAG_WRITING;									// Будем переписывать параметр
					}
					else
					{
						if (E_PAR.During_oper_mode != EEPROM_DUR_OP_MODE_PAR_READING) E_PAR.Array_Addr = 0;				// Текущая переменная также будет сброшена по ум-ю
						E_PAR.During_oper_mode = EEPROM_DUR_OP_MODE_ERROR;		// Ошибка, работа с EEPROM не доступна
						E_PAR.Array_Addr_Error = E_PAR.Array_Addr;
            OSL_EEPROM_ERROR_CB();
					}
				}
				break;
				
				// Значение перменной в памяти не соответсвует треубемому(при сбросе по умолчанию)
				case OSL_EEPROM_MEMORY_CHECK_DATA_ERROR:
				{
					E_PAR.Global_DATA_error_timer++;
					//OSL_EEPROM_LIM_ERROR_CB(E_PAR.Array_Addr);
					if (E_PAR.Repeat_par_write_timer < EEPROM_PAR_WRITE_LIMIT)
					{
						/*
						switch (E_PAR.During_oper_mode)
						{
							case (EEPROM_DUR_OP_MODE_PAR_READING):
							{
								if (!E_PAR.Par_Updated_flag) OSL_EEPROM_Default_Params(&E_PAR);		// Сбрасываем параметр по умолчанию, если еще не сбрасывали
							}
							break;						
							default: break;
						}
						*/
						
						E_PAR.Repeat_par_write_timer++;																			// Плюсуем таймер перезаписи параметра
						E_PAR.During_action = EEPROM_DUR_ACT_FLAG_WRITING;									// Будем переписывать параметр
					}
					else
					{
						if (E_PAR.During_oper_mode != EEPROM_DUR_OP_MODE_PAR_READING) E_PAR.Array_Addr = 0;				// Текущая переменная также будет сброшена по ум-ю
						E_PAR.During_oper_mode = EEPROM_DUR_OP_MODE_ERROR;		// Ошибка, работа с EEPROM не доступна
						E_PAR.Array_Addr_Error = E_PAR.Array_Addr;
            OSL_EEPROM_ERROR_CB();
					}
				}
				break;
				
				case OSL_EEPROM_MEMORY_CHECK_PAR_ERROR:	break;
			}
		}
		break;
		
		// Выставлен флаг записи
		case EEPROM_DUR_ACT_FLAG_WRITING:
		{
			E_PAR.During_action = EEPROM_DUR_ACT_DATA_WRITING;				// Далее пишем данные
		}
		break;
		
		// Данные записаны
		case EEPROM_DUR_ACT_DATA_WRITING:
		{
			E_PAR.During_action = EEPROM_DUR_ACT_STAT_READING;				// Далее читаем статус и проверяем данные
		}
		break;
	}
}

void OSL_EEPROM_Init_Params(EEPROM_T *p)
{
	for (uint16_t i = 0; i < EEPROM_WRITING_BUF_SIZE; i++) p->writing_buf[i] = NULL;
	p->writing_buf_index = 0;
	p->writing_elements_num = 0;
	p->Readen_Status.all = 0;
	p->During_oper_mode = EEPROM_DUR_OP_MODE_NEXT;
	p->Planned_oper_mode.all = 0;
	p->Planned_oper_mode.bit.Par_reading = 1;
	p->During_action = EEPROM_DUR_ACT_FREE;

	p->Global_par_write_timer = 0;
	p->Global_par_read_timer = 0;
	p->Global_stat_read_timer = 0;
	p->Global_CRC_error_timer = 0;
	p->Global_LIM_error_timer = 0;
	p->Global_DATA_error_timer = 0;
	
	p->Repeat_par_write_timer = 0;
	p->Repeat_par_read_timer = 0;
	p->Repeat_stat_read_timer = 0;	

	p->Array_Addr = 0;
	p->Array_Addr_Error = 0;
	p->Par_Updated_flag = false;
	p->Par_Readen_flag = false;
}

/* Private function prototypes ------------------------------------------------*/
bool OSL_EEPROM_Var_Search(EEPROM_T *p)
{
	for (uint16_t i = 0; i < g_Ram_memory_Size; i++)
	{
		if (g_Ram_memory[i].Size == p->writing_buf[p->writing_buf_index])
		{
			p->Array_Addr = i;
			return MEM_PARAM_FOUND_OK;
		}
	}
	return MEM_PARAM_NOT_FOUND;
}

bool OSL_EEPROM_Add_to_Writing(OSL_G_VAR_FLAGS_T* Size)
{
	if (!Size->bit.Mode_Default && !Size->bit.Mem_Fix)		// Если параметр запрещен для записи в EEPROM
	{
		OSL_EEPROM_ERROR_CB();
		return EEPROM_BAD_SIZE;
	}
		
	if (E_PAR.During_oper_mode == EEPROM_DUR_OP_MODE_ERROR)
  {
    OSL_Global_Par_Update_CB((uint32_t)Size, (uint32_t)&EEPROM_Params);
		OSL_EEPROM_VAR_UPDATE_CB((uint32_t)Size);
    return EEPROM_BUFFER_OVERLOADED;
  }
  
	uint16_t tmp_index = E_PAR.writing_buf_index;  // Будем проверять следующий элемент буфера, возможно он свободен
	
	if (tmp_index >= EEPROM_WRITING_BUF_SIZE) tmp_index = 0;
	
	for (E_PAR.i = 0; E_PAR.i < EEPROM_WRITING_BUF_SIZE; E_PAR.i++)				// Ищем элемент для записи
	{
		if (!E_PAR.writing_buf[tmp_index])				// Если ячейка буфера свободна, кладем в нее элемент на запись
		{
			E_PAR.Planned_oper_mode.bit.Par_writing = 1;
			E_PAR.writing_buf[tmp_index] = Size;
			E_PAR.writing_elements_num++;					// Говорим, что элементов в очереди на запись прибавилось
			return EEPROM_DATA_LOADED;
		}
		
		tmp_index++;
		if (tmp_index >= EEPROM_WRITING_BUF_SIZE) tmp_index = 0;
	}
	
	E_PAR.During_oper_mode = EEPROM_DUR_OP_MODE_ERROR;					// Сбой EEPROM
	E_PAR.Array_Addr_Error = E_PAR.Array_Addr;
  OSL_EEPROM_ERROR_CB();
	return EEPROM_BUFFER_OVERLOADED;
}

// Форматируем указатель согласно типу структуры и выдаем значение в переменную
#define OSL_MEMORY_GET_TYPED_VALUE_DEFINE(TYPE_DATA, TYPE_NAME, POINTER_ARRAY){\
	switch (POINTER_ARRAY.Size->all & GVAR_ONLY_WPD){\
		case GVAR_M_WD:\
			p->tmp_U32.Copy.Readen_Value = (uint32_t)(*(OSL_Var_RW_##TYPE_NAME##_D_T	*)POINTER_ARRAY.Size).Var;\
			for (p->i=0; p->i<sizeof(TYPE_DATA); p->i++) p->SPI_DEVICEx.data_send[3+p->i] = p->tmp_U32.Copy.Buff[p->i]; break;\
		case GVAR_M_WPD:\
			p->tmp_U32.Copy.Readen_Value = (uint32_t)*(*(OSL_Var_RW_##TYPE_NAME##_PD_T*)POINTER_ARRAY.Size).Var;\
			for (p->i=0; p->i<sizeof(TYPE_DATA); p->i++) p->SPI_DEVICEx.data_send[3+p->i] = p->tmp_U32.Copy.Buff[p->i]; break;\
		default: return;\
	}\
}

void OSL_EEPROM_Write_Param_IT(EEPROM_T *p)
{
	p->Global_par_write_timer++;
	p->Memory_Addr = g_Ram_memory[p->Array_Addr].Addr*8;				// 4 байта переменная + 2 байта CRC

	OSL_SPI_DEVICEx_Logical(&p->SPI_DEVICEx);

	p->SPI_DEVICEx.data_send[0] = _WRITE_;																										// Говорим что будем записывать	
	p->SPI_DEVICEx.data_send[1] = (uint8_t)(p->Memory_Addr>>8);																// Говорим, в какие адреса будем писать
	p->SPI_DEVICEx.data_send[2] = (uint8_t)p->Memory_Addr;

	switch (g_Ram_memory[p->Array_Addr].Size->all & GVAR_ONLY_TYPES)									// Читаем тип данных переменной
	{
		case (GVAR_T_8):
		{
			OSL_MEMORY_GET_TYPED_VALUE_DEFINE(uint8_t, UINT8, g_Ram_memory[p->Array_Addr]);											// Кладем данные на отправку
			//*((uint16_t*)&p->SPI_DEVICEx.data_send[4]) = CRC16((uint8_t*)&p->SPI_DEVICEx.data_send[3], 1);		// Кладем CRC
			p->CRC_Res.Uns16 = CRC16((uint8_t*)&p->SPI_DEVICEx.data_send[3], 1);																// Вычисляем CRC
			p->SPI_DEVICEx.data_send[4] = p->CRC_Res.Array[0];
			p->SPI_DEVICEx.data_send[5] = p->CRC_Res.Array[1];
			p->SPI_DEVICEx.Num_packets = 6;
		}
		break;
		
		case (GVAR_T_16):
		{
			OSL_MEMORY_GET_TYPED_VALUE_DEFINE(uint16_t, UINT16, g_Ram_memory[p->Array_Addr]);									// Кладем данные на отправку
			//*((uint16_t*)&p->SPI_DEVICEx.data_send[5]) = CRC16((uint8_t*)&p->SPI_DEVICEx.data_send[3], 2);		// Кладем CRC
			p->CRC_Res.Uns16 = CRC16((uint8_t*)&p->SPI_DEVICEx.data_send[3], 2);			// Вычисляем CRC
			p->SPI_DEVICEx.data_send[5] = p->CRC_Res.Array[0];
			p->SPI_DEVICEx.data_send[6] = p->CRC_Res.Array[1];
			p->SPI_DEVICEx.Num_packets = 7;
		}
		break;
		
		case(GVAR_T_32):
		case(GVAR_T_FL):
		case(GVAR_T_Q16):
		{
			OSL_MEMORY_GET_TYPED_VALUE_DEFINE(uint32_t, UINT32, g_Ram_memory[p->Array_Addr]);									// Кладем данные на отправку																		
			//*((uint16_t*)&p->SPI_DEVICEx.data_send[7]) = CRC16((uint8_t*)&p->SPI_DEVICEx.data_send[3], 4);		// Кладем CRC
			p->CRC_Res.Uns16 = CRC16((uint8_t*)&p->SPI_DEVICEx.data_send[3], 4);			// Вычисляем CRC
			p->SPI_DEVICEx.data_send[7] = p->CRC_Res.Array[0];
			p->SPI_DEVICEx.data_send[8] = p->CRC_Res.Array[1];
			p->SPI_DEVICEx.Num_packets = 9;
		}
		break;
		
		default:
    {
      E_PAR.During_oper_mode = EEPROM_DUR_OP_MODE_ERROR;
      p->Array_Addr_Error = p->Array_Addr;
      OSL_EEPROM_ERROR_CB(); 
      return;
    }
	}
	OSL_SPI_Data(&p->SPI_DEVICEx);
}

void OSL_EEPROM_CMD_Write_en_IT(EEPROM_T *p)
{
	OSL_SPI_DEVICEx_Logical(&p->SPI_DEVICEx);
	p->SPI_DEVICEx.data_send[0] = _WREN_;
	p->SPI_DEVICEx.Num_packets = 1;	
	OSL_SPI_Data(&p->SPI_DEVICEx);	
}

void OSL_EEPROM_Read_Param_IT(EEPROM_T *p)
{
	p->Global_par_read_timer++;	
	
	p->Memory_Addr = g_Ram_memory[p->Array_Addr].Addr*8;
	OSL_SPI_DEVICEx_Logical(&p->SPI_DEVICEx);

	p->SPI_DEVICEx.data_send[0] = _READ_;																							// Говорим что будем записывать	
	p->SPI_DEVICEx.data_send[1] = (uint8_t)(p->Memory_Addr>>8);												// Говорим, в какие адреса будем писать
	p->SPI_DEVICEx.data_send[2] = (uint8_t)p->Memory_Addr;
	
	switch (g_Ram_memory[p->Array_Addr].Size->all & GVAR_ONLY_TYPES)					// Читаем размер
	{
		case (GVAR_T_8):	p->SPI_DEVICEx.Num_packets = 6;	break;			//4
		case (GVAR_T_16):	p->SPI_DEVICEx.Num_packets = 7;	break;			//5
		case (GVAR_T_32):
		case (GVAR_T_FL):
		case (GVAR_T_Q16):p->SPI_DEVICEx.Num_packets = 9;	break;			//7
		default: return;
	}
	OSL_SPI_Data(&p->SPI_DEVICEx);
}

void OSL_EEPROM_Read_Status_IT(EEPROM_T *p)
{
	p->Global_stat_read_timer++;																										// Плюсуем таймер су
	OSL_SPI_DEVICEx_Logical(&p->SPI_DEVICEx);																				// Применяем настройки
	p->SPI_DEVICEx.data_send[0] = _RDSR_;																						// Команда на чтение статуса
	p->SPI_DEVICEx.Num_packets = 2;																									// Число байт в пакете
	OSL_SPI_Data(&p->SPI_DEVICEx);
}

//CALLBACK
bool OSL_EEPROM_Read_Stat_Callback(EEPROM_T *p)
{
	p->Readen_Status.all = p->SPI_DEVICEx.data_get[1];
	if (!p->Readen_Status.bit.RDY_inv) return EEPROM_READY;
	else return EEPROM_BUSY;
}

#define OSL_MEMORY_READ_TYPED_STRUCT_DEFINE(OUT_VAR, TYPE_NAME, POINTER_ARRAY){\
	switch (POINTER_ARRAY->all & GVAR_ONLY_WPD) {\
		case GVAR_M_WD:				OUT_VAR.Fix_Value =	&(*(OSL_Var_RW_##TYPE_NAME##_D_T	*)POINTER_ARRAY).Var;\
													OUT_VAR.Min				=	 (*(OSL_Var_RW_##TYPE_NAME##_D_T	*)POINTER_ARRAY).Min;\
													OUT_VAR.Max				=	 (*(OSL_Var_RW_##TYPE_NAME##_D_T	*)POINTER_ARRAY).Max;\
													OUT_VAR.Fact			=	 (*(OSL_Var_RW_##TYPE_NAME##_D_T	*)POINTER_ARRAY).Default; break;\
		case GVAR_M_WPD:			OUT_VAR.Fix_Value =	 (*(OSL_Var_RW_##TYPE_NAME##_PD_T	*)POINTER_ARRAY).Var;\
													OUT_VAR.Min				=	 (*(OSL_Var_RW_##TYPE_NAME##_PD_T	*)POINTER_ARRAY).Min;\
													OUT_VAR.Max				=	 (*(OSL_Var_RW_##TYPE_NAME##_PD_T	*)POINTER_ARRAY).Max;\
													OUT_VAR.Fact			=	 (*(OSL_Var_RW_##TYPE_NAME##_PD_T	*)POINTER_ARRAY).Default; break;\
		default:\
      E_PAR.During_oper_mode = EEPROM_DUR_OP_MODE_ERROR;\
      E_PAR.Array_Addr_Error = E_PAR.Array_Addr;\
      OSL_EEPROM_ERROR_CB();\
    break;\
	}\
}

#define OSL_MEMORY_CHECK_CRC_LIM_DEFINE(CHECK_STR, SIZE, VAR_BYTES, NORMAL_FUNC)\
p->CRC_Res.Uns16 = CRC16((uint8_t*)&p->SPI_DEVICEx.data_get[3], VAR_BYTES);\
if (p->SPI_DEVICEx.data_get[3+VAR_BYTES] != p->CRC_Res.Array[0]) return OSL_EEPROM_MEMORY_CHECK_CRC_ERROR;\
if (p->SPI_DEVICEx.data_get[4+VAR_BYTES] != p->CRC_Res.Array[1]) return OSL_EEPROM_MEMORY_CHECK_CRC_ERROR;\
if (E_PAR.During_oper_mode == EEPROM_DUR_OP_MODE_DEFAULT)\
{\
	if (CHECK_STR.Copy.Readen_Value != *CHECK_STR.Fix_Value) return OSL_EEPROM_MEMORY_CHECK_DATA_ERROR;\
}\
if ((CHECK_STR.Copy.Readen_Value >= CHECK_STR.Min && CHECK_STR.Copy.Readen_Value <= CHECK_STR.Max) && \
		(NORMAL_FUNC))\
{\
	if (!E_PAR.Par_Updated_flag)\
	{\
		E_PAR.Par_Updated_flag = true;\
		*CHECK_STR.Fix_Value = CHECK_STR.Copy.Readen_Value;\
		OSL_Global_Par_Update_CB((uint32_t)SIZE, (uint32_t)&EEPROM_Params);\
		OSL_EEPROM_VAR_UPDATE_CB((uint32_t)SIZE);\
	}\
	return OSL_EEPROM_MEMORY_CHECK_OK;\
}\
else return OSL_EEPROM_MEMORY_CHECK_LIM_ERROR;

OSL_EEPROM_MEMORY_CHECK_T OSL_EEPROM_Read_Params_Callback(EEPROM_T *p)
{
	switch (g_Ram_memory[p->Array_Addr].Size->all & GVAR_ONLY_TYPES_AND_SIGN)
	{
		case (GVAR_T_8|GVAR_T_S):
			OSL_MEMORY_READ_TYPED_STRUCT_DEFINE(p->tmp_8, INT8, g_Ram_memory[p->Array_Addr].Size);
			p->tmp_8.Copy.Readen_Value = (int8_t)p->SPI_DEVICEx.data_get[3];
			OSL_MEMORY_CHECK_CRC_LIM_DEFINE(p->tmp_8, g_Ram_memory[p->Array_Addr].Size, 1, 1);
		break;
		
		case (GVAR_T_8):
			OSL_MEMORY_READ_TYPED_STRUCT_DEFINE(p->tmp_U8, UINT8, g_Ram_memory[p->Array_Addr].Size);
			p->tmp_U8.Copy.Readen_Value = p->SPI_DEVICEx.data_get[3];
			OSL_MEMORY_CHECK_CRC_LIM_DEFINE(p->tmp_U8, g_Ram_memory[p->Array_Addr].Size, 1, 1);
		break;

		case (GVAR_T_16|GVAR_T_S):
			OSL_MEMORY_READ_TYPED_STRUCT_DEFINE(p->tmp_16, INT16, g_Ram_memory[p->Array_Addr].Size);
			for (p->i=0; p->i<2; p->i++) p->tmp_16.Copy.Buff[p->i] = p->SPI_DEVICEx.data_get[3+p->i];
			OSL_MEMORY_CHECK_CRC_LIM_DEFINE(p->tmp_16, g_Ram_memory[p->Array_Addr].Size, 2, 1);
		break;

		case (GVAR_T_16):
			OSL_MEMORY_READ_TYPED_STRUCT_DEFINE(p->tmp_U16, UINT16, g_Ram_memory[p->Array_Addr].Size);			
			for (p->i=0; p->i<2; p->i++) p->tmp_U16.Copy.Buff[p->i] = p->SPI_DEVICEx.data_get[3+p->i];
			OSL_MEMORY_CHECK_CRC_LIM_DEFINE(p->tmp_U16, g_Ram_memory[p->Array_Addr].Size, 2, 1);
		break;

		case (GVAR_T_32|GVAR_T_S):
			OSL_MEMORY_READ_TYPED_STRUCT_DEFINE(p->tmp_32, INT32, g_Ram_memory[p->Array_Addr].Size);
			for (p->i=0; p->i<4; p->i++) p->tmp_32.Copy.Buff[p->i] = p->SPI_DEVICEx.data_get[3+p->i];
			OSL_MEMORY_CHECK_CRC_LIM_DEFINE(p->tmp_32, g_Ram_memory[p->Array_Addr].Size, 4, 1);
		break;

		case (GVAR_T_32):
			OSL_MEMORY_READ_TYPED_STRUCT_DEFINE(p->tmp_U32, UINT32, g_Ram_memory[p->Array_Addr].Size);
			for (p->i=0; p->i<4; p->i++) p->tmp_U32.Copy.Buff[p->i] = p->SPI_DEVICEx.data_get[3+p->i];
			OSL_MEMORY_CHECK_CRC_LIM_DEFINE(p->tmp_U32, g_Ram_memory[p->Array_Addr].Size, 4, 1);
		break;
		
		case (GVAR_T_FL):
			OSL_MEMORY_READ_TYPED_STRUCT_DEFINE(p->tmp_FL, FLOAT, g_Ram_memory[p->Array_Addr].Size);
			for (p->i=0; p->i<4; p->i++) p->tmp_FL.Copy.Buff[p->i] = p->SPI_DEVICEx.data_get[3+p->i];
			OSL_MEMORY_CHECK_CRC_LIM_DEFINE(p->tmp_FL, g_Ram_memory[p->Array_Addr].Size, 4, (__isnormalf(p->tmp_FL.Copy.Readen_Value) || (!p->tmp_FL.Copy.Readen_Value)));
		break;

	#if defined(IQ_MATH)
		case (GVAR_T_Q16):
			OSL_MEMORY_READ_TYPED_STRUCT_DEFINE(tmp_Q16, !!!, g_Ram_memory[p->Array_Addr].Size);
			tmp_Q16.Readen_Value  = (fix16_t)(((uint32_t)p->SPI_DEVICEx.data_get[3])<<24);
			tmp_Q16.Readen_Value += (fix16_t)(((uint32_t)p->SPI_DEVICEx.data_get[4])<<16);
			tmp_Q16.Readen_Value += (fix16_t)(((uint32_t)p->SPI_DEVICEx.data_get[5])<<8);
			tmp_Q16.Readen_Value += (fix16_t)p->SPI_DEVICEx.data_get[6];
			OSL_MEMORY_CHECK_MIN_MAX_DEFINE(tmp_FL, g_Ram_memory[p->Reading_Array_Addr].Size);
		break;	
	#endif
		default:
    {
      p->Array_Addr_Error = p->Array_Addr;
      OSL_EEPROM_ERROR_CB(); 
      E_PAR.During_oper_mode = EEPROM_DUR_OP_MODE_ERROR;
      return OSL_EEPROM_MEMORY_CHECK_PAR_ERROR;
    }
	}
}

#define OSL_MEMORY_DEFAULT_SET(VAR, TYPE_NAME, SIZE){\
	switch (g_Ram_memory[p->Array_Addr].Size->all & GVAR_ONLY_WPD)\
  {\
		case GVAR_M_WD:				VAR.Fix_Value =	&(*(OSL_Var_RW_##TYPE_NAME##_D_T	*)g_Ram_memory[p->Array_Addr].Size).Var;\
													VAR.Fact			=	 (*(OSL_Var_RW_##TYPE_NAME##_D_T	*)g_Ram_memory[p->Array_Addr].Size).Default; break;\
		case GVAR_M_WPD:			VAR.Fix_Value =	 (*(OSL_Var_RW_##TYPE_NAME##_PD_T	*)g_Ram_memory[p->Array_Addr].Size).Var;\
													VAR.Fact			=	 (*(OSL_Var_RW_##TYPE_NAME##_PD_T	*)g_Ram_memory[p->Array_Addr].Size).Default; break;\
		default:\
    {\
      E_PAR.Array_Addr_Error = E_PAR.Array_Addr;\
      OSL_EEPROM_ERROR_CB();\
      E_PAR.During_oper_mode = EEPROM_DUR_OP_MODE_ERROR;\
     return EEPROM_DEF_NOT_UPDATED;\
    }\
  }\
	*VAR.Fix_Value = VAR.Fact;\
	if (!E_PAR.Par_Updated_flag)\
	{\
		E_PAR.Par_Updated_flag = true;\
		OSL_Global_Par_Update_CB((uint32_t)SIZE, (uint32_t)&EEPROM_Params);\
		OSL_EEPROM_VAR_UPDATE_CB((uint32_t)SIZE);\
		return EEPROM_DEF_UPDATED;\
	}\
	else return EEPROM_DEF_NOT_UPDATED;\
}

bool OSL_EEPROM_Default_Params(EEPROM_T *p)
{
	switch (g_Ram_memory[p->Array_Addr].Size->all & GVAR_ONLY_TYPES_AND_SIGN)
	{
		case (GVAR_T_8|GVAR_T_S):	OSL_MEMORY_DEFAULT_SET(p->tmp_8,		INT8,		g_Ram_memory[p->Array_Addr].Size);
		case (GVAR_T_8):					OSL_MEMORY_DEFAULT_SET(p->tmp_U8,		UINT8,	g_Ram_memory[p->Array_Addr].Size);
		case (GVAR_T_16|GVAR_T_S):OSL_MEMORY_DEFAULT_SET(p->tmp_16,		INT16,	g_Ram_memory[p->Array_Addr].Size);
		case (GVAR_T_16):					OSL_MEMORY_DEFAULT_SET(p->tmp_U16,	UINT16,	g_Ram_memory[p->Array_Addr].Size);
		case (GVAR_T_32|GVAR_T_S):OSL_MEMORY_DEFAULT_SET(p->tmp_32,		INT32,	g_Ram_memory[p->Array_Addr].Size);
		case (GVAR_T_32):					OSL_MEMORY_DEFAULT_SET(p->tmp_U32,	UINT32,	g_Ram_memory[p->Array_Addr].Size);
		case (GVAR_T_FL):					OSL_MEMORY_DEFAULT_SET(p->tmp_FL,		FLOAT,	g_Ram_memory[p->Array_Addr].Size);
	#if defined(IQ_MATH)		
		case (GVAR_T_Q16):				OSL_MEMORY_DEFAULT_SET(tmp_Q16,	!!!);			break;
	#endif
		default:
    {
      p->Array_Addr_Error = p->Array_Addr;
      OSL_EEPROM_ERROR_CB();
      E_PAR.During_oper_mode = EEPROM_DUR_OP_MODE_ERROR;
      return EEPROM_DEF_NOT_UPDATED;
    }
	}
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
