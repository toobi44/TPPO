#include "user_g_Ram.h"		
#include "tim.h"
#include "usart.h"
#include "spi.h"
#include "gpio.h"
#include "rtc.h"

#define GetAdr(Elem)				((uint32_t)&(((grMAIN_T*)0)->Elem))

uint8_t tmp_Buff[16];
uint8_t tmp_ind = 0;
uint8_t polling_fault = 0;
uint8_t kit_ind = 0;

#define WINSTAR_CODE_DATA 			1
#define WINSTAR_CODE_INSTR			2
#define WINSTAR_CODE_ADDR				3
#define WINSTAR_CODE_DISABLE		4

bool LCD_disable_flag = 0;
extern bool LCD_polling_str;

void OSL_DISPLAY_DISABLE_Callback(void)
{
	LCD_PWR_off();
	OSL_TIM_Time_Calc(&LCD_PWR_OFF_DELAY, &TIME_SOURCE_SLOW, TIME_CALC_ACTIVE);
}

uint16_t LCD_disable_cnt = 0;

// Колбек при успешном ответе слейва мастеру
void OSL_Modbus_Master_Successful_Request_Callback(Modbus_Params_T *p)
{
	if (p == &Modbus_PULT_RS485_MAS)
	{
		OSL_TIM_Time_Calc(&FAULT_POLLING, &TIME_SOURCE_SLOW, TIME_CALC_ACTIVE);
    switch (p->Addr)
    {
      case MAS_MDB_LED_STATE_RO:			// LCD состояние светодиодов
      {
				if (grMAIN.LED_State.Var.bit.cmd == 3)
				{
					__disable_irq();
					HAL_RTCEx_BKUPWrite(&hrtc, BOOT_BLOCK_ONES, 1);
					HAL_NVIC_SystemReset();
					while(1) {};
				}
			}
			break;
			
      case MAS_MDB_LCD_CODE_DATA_ARRAY_RO:			// LCD строка считана
      {
				if (LCD_polling_str) return;
				switch (grMAIN.grLCD.Code_Symbols.Var.Code_Symbols.Code)
				{
					case WINSTAR_CODE_DATA:		// Данные
					{
						if (LCD_disable_flag)
						{
							LCD_disable_flag = 0;
							OSL_LCD_DisableDisplay();
							return;
						}
						else if (LCD_PWR_OFF_DELAY.State == TIME_CALC_ACTIVE ||
										 LCD_PWR_ON_DELAY.State == TIME_CALC_ACTIVE) return;
						
						tmp_ind = 0;
						for (uint8_t i=0; i< grMAIN.grLCD.Code_Symbols.Var.Code_Symbols.Symbols; i+=2)
						{
							tmp_Buff[i] = grMAIN.grLCD.Buff[tmp_ind].Var.Symbols[0];
							tmp_Buff[i+1] = grMAIN.grLCD.Buff[tmp_ind].Var.Symbols[1];
							tmp_ind++;
						}
						
						OSL_LCD_StringUpdate(grMAIN.grLCD.Row_Column.Var.Row_Column.Row, grMAIN.grLCD.Row_Column.Var.Row_Column.Column, (uint8_t*)&tmp_Buff[0], grMAIN.grLCD.Code_Symbols.Var.Code_Symbols.Symbols, OSL_LCD_MODE_STATIC, NO_MATTER);	
						Modbus_PULT_RS485_MAS.Mas_Par->Oper_Request[MAS_MDB_LCD_CODE_DATA_ARRAY_RO].Cmd = MDB_MAS_REG_CMD_DIS;
						Modbus_PULT_RS485_MAS.Mas_Par->Oper_Request[MAS_MDB_LCD_DATA_READEN_FLAG_WR].Cmd = MDB_MAS_REG_CMD_EN;
						grMAIN.grLCD.Buff_Flag.Var = 1;
					}
					break;
					
					case WINSTAR_CODE_INSTR:		// Команда
					{
						if (LCD_disable_flag)
						{
							LCD_disable_flag = 0;
							OSL_LCD_DisableDisplay();
							return;
						}
						else if (LCD_PWR_OFF_DELAY.State == TIME_CALC_ACTIVE ||
										 LCD_PWR_ON_DELAY.State == TIME_CALC_ACTIVE) return;
						
						OSL_LCD_WriteIns_Func(grMAIN.grLCD.Code_Symbols.Var.Code_Symbols.Symbols);
						Modbus_PULT_RS485_MAS.Mas_Par->Oper_Request[MAS_MDB_LCD_CODE_DATA_ARRAY_RO].Cmd = MDB_MAS_REG_CMD_DIS;
						Modbus_PULT_RS485_MAS.Mas_Par->Oper_Request[MAS_MDB_LCD_DATA_READEN_FLAG_WR].Cmd = MDB_MAS_REG_CMD_EN;
						grMAIN.grLCD.Buff_Flag.Var = 1;
					}
					break;
					
					case WINSTAR_CODE_ADDR:		// Адрес
					{
						if (LCD_disable_flag)
						{
							LCD_disable_flag = 0;
							OSL_LCD_DisableDisplay();
							return;
						}
						else if (LCD_PWR_OFF_DELAY.State == TIME_CALC_ACTIVE ||
										 LCD_PWR_ON_DELAY.State == TIME_CALC_ACTIVE) return;
						
						OSL_LCD_SetAddr(grMAIN.grLCD.Row_Column.Var.Row_Column.Row, grMAIN.grLCD.Row_Column.Var.Row_Column.Column);
						Modbus_PULT_RS485_MAS.Mas_Par->Oper_Request[MAS_MDB_LCD_CODE_DATA_ARRAY_RO].Cmd = MDB_MAS_REG_CMD_DIS;
						Modbus_PULT_RS485_MAS.Mas_Par->Oper_Request[MAS_MDB_LCD_DATA_READEN_FLAG_WR].Cmd = MDB_MAS_REG_CMD_EN;						
						grMAIN.grLCD.Buff_Flag.Var = 1;
					}
					break;
					
					case WINSTAR_CODE_DISABLE:		// Отключить дисплей
					{
						if (LCD_disable_flag) return;				
						if (LCD_PWR_OFF_DELAY.State == TIME_CALC_ACTIVE ||
								LCD_PWR_ON_DELAY.State == TIME_CALC_ACTIVE) return;						

						OSL_LCD_StringUpdate(0, 0, (uint8_t*)&Clear_Text[0], 16, OSL_LCD_MODE_STATIC, NO_MATTER);
						OSL_LCD_StringUpdate(1, 0, (uint8_t*)&Clear_Text[0], 16, OSL_LCD_MODE_STATIC, NO_MATTER);

						Modbus_PULT_RS485_MAS.Mas_Par->Oper_Request[MAS_MDB_LCD_CODE_DATA_ARRAY_RO].Cmd = MDB_MAS_REG_CMD_DIS;
						Modbus_PULT_RS485_MAS.Mas_Par->Oper_Request[MAS_MDB_LCD_DATA_READEN_FLAG_WR].Cmd = MDB_MAS_REG_CMD_EN;					
						grMAIN.grLCD.Buff_Flag.Var = 1;
						LCD_disable_cnt++;
						LCD_disable_flag = 1;
					}
					break;					
					default: break;
				}
      }
      break;

      case MAS_MDB_LCD_DATA_READEN_FLAG_WR:			// Флаг записан, можно читать следующую строку
      {
				p->Mas_Par->Oper_Request[MAS_MDB_LCD_CODE_DATA_ARRAY_RO].Cmd = MDB_MAS_REG_CMD_EN;
			}
      break;
			
      case MAS_MDB_POLLING_FAULT_RES_RW:
      {
				if (grMAIN.Polling_Fault_Res.Size.bit.Mdb_Mas_Write_Allowed)				// Была запись
				{
					grMAIN.Polling_Fault_Res.Size.bit.Mdb_Mas_Write_Allowed = 0;			// Будем читать
					p->Mas_Par->Oper_Request[MAS_MDB_POLLING_FAULT_RES_RW].Cmd = MDB_MAS_REG_CMD_EN;
				}
				else
				{
					if (polling_fault)
					{
						polling_fault = 0;
						grMAIN.Polling_Fault_Res.Size.bit.Mdb_Mas_Write_Allowed = 1;
						grMAIN.Polling_Fault_Res.Var = 1;
						p->Mas_Par->Oper_Request[MAS_MDB_POLLING_FAULT_RES_RW].Cmd = MDB_MAS_REG_CMD_EN;
					}
					else if (grMAIN.Polling_Fault_Res.Var == 1)
					{
						for (uint8_t i = 0; i< MODBUS_MASTER_PAR_NUM; i++) 				// Отключаем обработку всех регистров
							p->Mas_Par->Oper_Request[i].Cmd = MDB_MAS_REG_CMD_DIS;
							
						grMAIN.Reg.Cmd.Size.bit.Mdb_Mas_Write_Allowed = 0;
						p->Mas_Par->Oper_Request[MAS_MDB_BUTT_STATE_WR].Cmd = MDB_MAS_REG_CMD_EN;
						p->Mas_Par->Oper_Request[MAS_MDB_LED_STATE_RO].Cmd = MDB_MAS_REG_CMD_EN;
						p->Mas_Par->Oper_Request[MAS_MDB_LCD_CODE_DATA_ARRAY_RO].Cmd = MDB_MAS_REG_CMD_EN;
						p->Mas_Par->Oper_Request[MAS_MDB_TSPO_SOFT_VER_WR].Cmd = MDB_MAS_REG_CMD_EN;
						p->Mas_Par->Oper_Request[MAS_MDB_IND_PM_SOFT_VER].Cmd = MDB_MAS_REG_CMD_EN;
						p->Mas_Par->Oper_Request[MAS_MDB_IND_IO_SOFT_VER].Cmd = MDB_MAS_REG_CMD_EN;
						p->Mas_Par->Oper_Request[MAS_MDB_IND_REG_CMD_RW].Cmd = MDB_MAS_REG_CMD_EN;
						p->Mas_Par->Oper_Request[MAS_MDB_WARNING_TSPO_WR].Cmd = MDB_MAS_REG_CMD_EN;

						// Зануляем регистр
						grMAIN.Polling_Fault_Res.Size.bit.Mdb_Mas_Write_Allowed = 1;
						grMAIN.Polling_Fault_Res.Var = 0;
						p->Mas_Par->Oper_Request[MAS_MDB_POLLING_FAULT_RES_RW].Cmd = MDB_MAS_REG_CMD_EN;
						
						// Сбрасываем биты действия статусного регистра
						if (grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.bit.saving)
						{
							grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.bit.zero = 1;
							grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.bit.saving_fault = 1;
							grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.bit.pm_ver = 0;
							grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.bit.io_ver = 0;
							grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].IO_Ver.Var = 0;
							grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].PM_Ver.Var = 0;
							grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].Date_Mdb.Var.all = 0;
							grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].Time_Mdb.Var.all = 0;
							grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].Motor_Power.Var = 0;
							grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].Motor_Curr.Var = 0;
						}
						else if (grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.bit.loading)
						{
							grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.bit.loading = 0;
							grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.bit.loading_fault = 1;
						}
						grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.all &= ~KITx_ACTION_MASK;
						for (uint8_t i = 0; i<EXT_FLASH_SECTOR_BUFF; i++)
						{
							p->Mas_Par->Oper_Request[MAS_MDB_IND_KIT0_PM_VER_WR+i*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
							p->Mas_Par->Oper_Request[MAS_MDB_IND_KIT0_IO_VER_WR+i*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
							p->Mas_Par->Oper_Request[MAS_MDB_IND_KIT0_STATE_WR+i*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
							p->Mas_Par->Oper_Request[MAS_MDB_IND_KIT0_DATE_WR+i*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
							p->Mas_Par->Oper_Request[MAS_MDB_IND_KIT0_TIME_WR+i*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
							p->Mas_Par->Oper_Request[MAS_MDB_IND_KIT0_PM_POWER_WR+i*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
							p->Mas_Par->Oper_Request[MAS_MDB_IND_KIT0_PM_CURR_WR+i*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
						}
					}
				}
			}
      break;
			
      case MAS_MDB_IND_PM_SOFT_VER:
      {
				if (grMAIN.PM_Soft_Ver.Var)
				{
					for (uint8_t i = 0; i<EXT_FLASH_SECTOR_BUFF; i++)
					{
						if (grMAIN.PM_Soft_Ver.Var != grMAIN.Kit[i].PM_Ver.Var)
						{
							if (!grMAIN.Kit[i].State.Var.bit.zero)
							{
								grMAIN.Kit[i].State.Var.bit.pm_ver = 1;
								p->Mas_Par->Oper_Request[MAS_MDB_IND_KIT0_STATE_WR+EXT_FLASH_Params.Oper_Sector*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
							}
						}
					}
					p->Mas_Par->Oper_Request[MAS_MDB_IND_PM_SOFT_VER].Cmd = MDB_MAS_REG_CMD_DIS;
				}
			}
      break;
			
      case MAS_MDB_IND_IO_SOFT_VER:
      {
				if (grMAIN.IO_Soft_Ver.Var)
				{
					for (uint8_t i = 0; i< EXT_FLASH_SECTOR_BUFF; i++)
					{					
						if (grMAIN.IO_Soft_Ver.Var != grMAIN.Kit[i].IO_Ver.Var)
						{
							if (!grMAIN.Kit[i].State.Var.bit.zero)
							{
								grMAIN.Kit[i].State.Var.bit.io_ver = 1;
								Master_Par.Oper_Request[MAS_MDB_IND_KIT0_STATE_WR+EXT_FLASH_Params.Oper_Sector*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
							}
						}
					}
					p->Mas_Par->Oper_Request[MAS_MDB_IND_IO_SOFT_VER].Cmd = MDB_MAS_REG_CMD_DIS;
				}
			}
      break;
			
      case MAS_MDB_IND_KIT0_STATE_WR:
      case MAS_MDB_IND_KIT1_STATE_WR:
      {
				if (grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.bit.saving)
				{
					grMAIN.Reg.Shift_WR.Var = 0;
					p->Mas_Par->Oper_Request[MAS_MDB_IND_REG_SHIFT_WR].Cmd = MDB_MAS_REG_CMD_EN;
					p->Mas_Par->Oper_Request[MAS_MDB_IND_REG_SHIFT_RO].Cmd = MDB_MAS_REG_CMD_DIS;
					p->Mas_Par->Oper_Request[MAS_MDB_IND_REG_BUFF_RO].Cmd = MDB_MAS_REG_CMD_DIS;
				}
			}
      break;
			
      case MAS_MDB_IND_REG_SHIFT_WR:
      {
				if (grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.bit.saving)
				{
					p->Mas_Par->Oper_Request[MAS_MDB_IND_REG_SHIFT_RO].Cmd = MDB_MAS_REG_CMD_EN;
				}
			}
      break;
				
      case MAS_MDB_IND_REG_SHIFT_RO:
      {
				if (grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.bit.saving)
				{
					switch (grMAIN.Reg.Shift_RO.Var)
					{
						case SHIFT_STATE_BUSY: 		break;		// Продолжаем чтение
						case SHIFT_STATE_STARTED:						// Сдвиг не записался
						{
							grMAIN.Reg.Shift_WR.Var = 0;
							p->Mas_Par->Oper_Request[MAS_MDB_IND_REG_SHIFT_WR].Cmd = MDB_MAS_REG_CMD_EN;
						}
						break;
						
						case SHIFT_STATE_FINISHED:					// Все регистры считаны
						{
							p->Mas_Par->Oper_Request[MAS_MDB_IND_REG_SHIFT_RO].Cmd = MDB_MAS_REG_CMD_DIS;
							p->Mas_Par->Oper_Request[MAS_MDB_IND_REG_BUFF_RO].Cmd = MDB_MAS_REG_CMD_DIS;					
							
							grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].Ext_Flash_Addr.Var =\
								EXT_FLASH_Params.Sector_Buff[EXT_FLASH_Params.Oper_Sector].Writing_Fl_Addr;
							
							grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].PM_Ver.Var = grMAIN.PM_Soft_Ver.Var;
							OSL_EEPROM_Add_to_Writing(&grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].PM_Ver.Size);
							p->Mas_Par->Oper_Request[MAS_MDB_IND_KIT0_PM_VER_WR+EXT_FLASH_Params.Oper_Sector*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
	
							grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].IO_Ver.Var = grMAIN.IO_Soft_Ver.Var;
							OSL_EEPROM_Add_to_Writing(&grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].IO_Ver.Size);
							p->Mas_Par->Oper_Request[MAS_MDB_IND_KIT0_IO_VER_WR+EXT_FLASH_Params.Oper_Sector*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
							
							grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].Date_Mdb.Var = grMAIN.IO_Date_Mdb.Var;
							OSL_EEPROM_Add_to_Writing(&grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].Date_Mdb.Size);
							p->Mas_Par->Oper_Request[MAS_MDB_IND_KIT0_DATE_WR+EXT_FLASH_Params.Oper_Sector*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
							
							grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].Time_Mdb.Var = grMAIN.IO_Time_Mdb.Var;
							OSL_EEPROM_Add_to_Writing(&grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].Time_Mdb.Size);
							p->Mas_Par->Oper_Request[MAS_MDB_IND_KIT0_TIME_WR+EXT_FLASH_Params.Oper_Sector*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
							
							grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].Motor_Power.Var = grMAIN.PM_Motor_Power.Var;
							OSL_EEPROM_Add_to_Writing(&grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].Motor_Power.Size);
							p->Mas_Par->Oper_Request[MAS_MDB_IND_KIT0_PM_POWER_WR+EXT_FLASH_Params.Oper_Sector*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;

							grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].Motor_Curr.Var = grMAIN.PM_Motor_Curr.Var;
							OSL_EEPROM_Add_to_Writing(&grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].Motor_Curr.Size);
							p->Mas_Par->Oper_Request[MAS_MDB_IND_KIT0_PM_CURR_WR+EXT_FLASH_Params.Oper_Sector*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
							
							OSL_EEPROM_Add_to_Writing(&grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].Ext_Flash_Addr.Size);
						}
						break;
						
						case SHIFT_STATE_ERR_SHIFT:						// Ошибка сдвига
						case SHIFT_STATE_ERR_UNKNOWN:					// Ошибка логики
						{
							grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.all = 0;
							grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.bit.zero = 1;
							grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.bit.saving_fault = 1;
							Master_Par.Oper_Request[MAS_MDB_IND_KIT0_STATE_WR+EXT_FLASH_Params.Oper_Sector*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
							p->Mas_Par->Oper_Request[MAS_MDB_IND_REG_SHIFT_RO].Cmd = MDB_MAS_REG_CMD_DIS;
						}
						break;	
						
						default:														// Сдвиг считан корректно
						{
							if (grMAIN.Reg.Shift_RO.Var == (grMAIN.Reg.Shift_WR.Var-1))
							{
								grMAIN.Reg.Shift_WR.Var = 0;
								p->Mas_Par->Oper_Request[MAS_MDB_IND_REG_SHIFT_WR].Cmd = MDB_MAS_REG_CMD_EN;
							}
							else if (grMAIN.Reg.Shift_RO.Var == grMAIN.Reg.Shift_WR.Var)		//  Сдвиг верный, читаем данные
							{
								p->Mas_Par->Oper_Request[MAS_MDB_IND_REG_BUFF_RO].Cmd = MDB_MAS_REG_CMD_EN;
							}
							else // Ошибка сдвига
							{
								grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.all = 0;
								grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.bit.zero = 1;
								grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.bit.saving_fault = 1;
								Master_Par.Oper_Request[MAS_MDB_IND_KIT0_STATE_WR+EXT_FLASH_Params.Oper_Sector*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
							}
						}
						break;
					}
				}
				else if (grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.bit.loading)
				{
					if (grMAIN.Reg.Shift_RO.Var == SHIFT_STATE_STARTED)
					{
						grMAIN.Reg.Shift_WR.Var = EXT_FLASH_Params.Oper_Sector;
						p->Mas_Par->Oper_Request[MAS_MDB_IND_REG_SHIFT_RO].Cmd = MDB_MAS_REG_CMD_DIS;
						
						OSL_EXT_FLASH_Cmd_Update(&EXT_FLASH_Params, EXT_FLASH_READ_JR_CMD_STOP);
						OSL_EXT_FLASH_Cmd_Update(&EXT_FLASH_Params, EXT_FLASH_READ_JR_CMD_START);
						EXT_FLASH_Params.Read_Par.Shift = 0;
						
						OSL_EXT_Read_Journ_Str(&EXT_FLASH_Params);
					}
					else if (grMAIN.Reg.Shift_RO.Var == SHIFT_STATE_ERR_SHIFT ||
									 grMAIN.Reg.Shift_RO.Var == SHIFT_STATE_ERR_UNKNOWN)
					{
						grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.bit.loading = 0;
						grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.bit.loading_fault = 1;
						Master_Par.Oper_Request[MAS_MDB_IND_KIT0_STATE_WR+EXT_FLASH_Params.Oper_Sector*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
					}
				}				
			}
      break;	
			
      case MAS_MDB_IND_REG_BUFF_RO:
      {
				if (grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.bit.saving)
				{
					p->Mas_Par->Oper_Request[MAS_MDB_IND_REG_BUFF_RO].Cmd = MDB_MAS_REG_CMD_DIS;
					
					grMAIN.Reg.Shift_WR.Var++;
					p->Mas_Par->Oper_Request[MAS_MDB_IND_REG_SHIFT_WR].Cmd = MDB_MAS_REG_CMD_EN;
					OSL_EXT_FLASH_Add_to_Writing(&EXT_FLASH_Params);
				}
			}
      break;			

      case MAS_MDB_IND_REG_CMD_RW:
      {
				if (!grMAIN.Reg.Cmd.Size.bit.Mdb_Mas_Write_Allowed)
				{
					if (EXT_FLASH_Params.During_oper_mode != EXT_FLASH_DUR_OP_MODE_NEXT)
					{
						// Зануляем команду
						grMAIN.Reg.Cmd.Size.bit.Mdb_Mas_Write_Allowed = 1;
						grMAIN.Reg.Cmd.Var = REG_CMD_FREE;
						return;
					}						
					
					for (uint8_t i = 0; i < EXT_FLASH_SECTOR_BUFF; i++)
					{						
						if (grMAIN.Kit[i].State.Var.all & KIT_BUSY_MASK)
						{
							// Зануляем команду
							grMAIN.Reg.Cmd.Size.bit.Mdb_Mas_Write_Allowed = 1;
							grMAIN.Reg.Cmd.Var = REG_CMD_FREE;
							return;
						}
						
						if (i == (EXT_FLASH_SECTOR_BUFF-1))
						{
							kit_ind = (grMAIN.Reg.Cmd.Var - 1)/3;
							switch (grMAIN.Reg.Cmd.Var)
							{
								case REG_CMD_KIT0_SAVE:
								case REG_CMD_KIT1_SAVE:
								{
									grMAIN.Kit[kit_ind].Ext_Flash_Addr.Var = grMAIN.Kit[kit_ind].Ext_Flash_Addr.Min;
									OSL_EEPROM_Add_to_Writing(&grMAIN.Kit[kit_ind].Ext_Flash_Addr.Size);
									EXT_FLASH_Params.Planned_oper_mode.bit.Set_Sector = 1;
									
									grMAIN.Kit[kit_ind].State.Var.all = 0;
									grMAIN.Kit[kit_ind].State.Var.bit.saving = 1;
									
									p->Mas_Par->Oper_Request[MAS_MDB_IO_DATE_RO].Cmd = MDB_MAS_REG_CMD_EN;
									p->Mas_Par->Oper_Request[MAS_MDB_IO_TIME_RO].Cmd = MDB_MAS_REG_CMD_EN;
									p->Mas_Par->Oper_Request[MAS_MDB_IND_PM_POWER_VER].Cmd = MDB_MAS_REG_CMD_EN;
									p->Mas_Par->Oper_Request[MAS_MDB_IND_IO_CURR_VER].Cmd = MDB_MAS_REG_CMD_EN;
								}
								break;
			
								case REG_CMD_KIT0_DELETE:
								case REG_CMD_KIT1_DELETE:
								{
									grMAIN.Kit[kit_ind].Ext_Flash_Addr.Var = grMAIN.Kit[kit_ind].Ext_Flash_Addr.Min;
									OSL_EEPROM_Add_to_Writing(&grMAIN.Kit[kit_ind].Ext_Flash_Addr.Size);
									
									grMAIN.Kit[kit_ind].State.Var.all = 0;
								}
								break;
								
								case REG_CMD_KIT0_LOAD:
								case REG_CMD_KIT1_LOAD:
								{
									if (!grMAIN.Kit[kit_ind].State.Var.bit.pm_ver &&
											!grMAIN.Kit[kit_ind].State.Var.bit.io_ver &&
											grMAIN.Kit[kit_ind].State.Var.bit.full)
									{
										grMAIN.Kit[kit_ind].State.Var.bit.loading_fault = 0;
										grMAIN.Kit[kit_ind].State.Var.bit.loaded = 0;
										grMAIN.Kit[kit_ind].State.Var.bit.loading = 1;
										p->Mas_Par->Oper_Request[MAS_MDB_IND_KIT0_STATE_WR+kit_ind*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
										p->Mas_Par->Oper_Request[MAS_MDB_IND_REG_SHIFT_RO].Cmd = MDB_MAS_REG_CMD_EN;
										EXT_FLASH_Params.Planned_oper_mode.bit.Set_Sector = 1;
									}
								}
								break;
								
								case REG_CMD_FREE: return;
								default: break;
							}
							// Зануляем команду
							grMAIN.Reg.Cmd.Size.bit.Mdb_Mas_Write_Allowed = 1;
							grMAIN.Reg.Cmd.Var = REG_CMD_FREE;
						}
					}
				}
				else  // Сбрасываем команду
				{
					grMAIN.Reg.Cmd.Size.bit.Mdb_Mas_Write_Allowed = 0;
					Master_Par.Oper_Request[MAS_MDB_IND_REG_CMD_RW].Cmd = MDB_MAS_REG_CMD_EN;
				}
			}
      break;
			
			case MAS_MDB_IND_REG_DATA_WR:
			{
				grMAIN.Reg.Shift_WR.Var++;
				OSL_EXT_Read_Journ_Str(&EXT_FLASH_Params);
			}
      break;
			
			case MAS_MDB_PM_CTRL_SYS_CMD_WR:
			{
				grMAIN.PM_Ctrl_Sys_Cmd.Var = 0;
			}
      break;			
			
			case MAS_MDB_IO_DATE_RO: 				Master_Par.Oper_Request[MAS_MDB_IO_DATE_RO].Cmd = MDB_MAS_REG_CMD_DIS;				break;
			case MAS_MDB_IO_TIME_RO:				Master_Par.Oper_Request[MAS_MDB_IO_TIME_RO].Cmd = MDB_MAS_REG_CMD_DIS;				break;
			case MAS_MDB_IND_PM_POWER_VER:	Master_Par.Oper_Request[MAS_MDB_IND_PM_POWER_VER].Cmd = MDB_MAS_REG_CMD_DIS;	break;
			case MAS_MDB_IND_IO_CURR_VER:		Master_Par.Oper_Request[MAS_MDB_IND_IO_CURR_VER].Cmd = MDB_MAS_REG_CMD_DIS;		break;			
			default: return;
		}
	}
}

void OSL_MDB_MAS_Exception_Callback(Modbus_Params_T *p)
{
	if (p == &Modbus_PULT_RS485_MAS)
	{
    switch (p->Addr)
    {
			case MAS_MDB_IND_REG_DATA_WR:
			{
				grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.bit.loading = 0;
				grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.bit.loading_fault = 1;
				Master_Par.Oper_Request[MAS_MDB_IND_KIT0_STATE_WR+EXT_FLASH_Params.Oper_Sector*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
				Master_Par.Oper_Request[MAS_MDB_IND_REG_DATA_WR].Cmd = MDB_MAS_REG_CMD_DIS;
			}
			break;
			default: break;
		}
	}
}

extern bool LCD_polling_str;

void OSL_MDB_MAS_TimeOut_NoAnswer(Modbus_Params_T *p, uint16_t Addr)
{
	if (p == &Modbus_PULT_RS485_MAS)
	{
		// Если запрос мастера слейву без ответа в течение тайм-аута, повторно не спрашиваем
		Modbus_PULT_RS485_MAS.Manual_Request = false;
		Master_Par.Oper_Request[MAS_MDB_USB_USART_MANUAL].Cmd = MDB_MAS_REG_CMD_DIS;
		
		if (!polling_fault)
		{
			OSL_TIM_Time_Calc(&FAULT_POLLING, &TIME_SOURCE_SLOW, VIEW_RESULT);
			if (FAULT_POLLING.Result_time_us > 5000UL)			// 0.5 sec
			{
				OSL_TIM_Time_Calc_Reset(&FAULT_POLLING);
				polling_fault = 1;

				for (uint8_t i = 0; i< MODBUS_MASTER_PAR_NUM; i++) 				// Отключаем обработку всех регистров
					p->Mas_Par->Oper_Request[i].Cmd = MDB_MAS_REG_CMD_DIS;
				
				grMAIN.Polling_Fault_Res.Size.bit.Mdb_Mas_Write_Allowed = 1;
				grMAIN.Polling_Fault_Res.Var = 1;
				p->Mas_Par->Oper_Request[MAS_MDB_POLLING_FAULT_RES_RW].Cmd = MDB_MAS_REG_CMD_EN;

				if (!LCD_disable_flag &&
						(LCD_PWR_OFF_DELAY.State != TIME_CALC_ACTIVE) && 
						(LCD_PWR_ON_DELAY.State != TIME_CALC_ACTIVE) )
				{
					OSL_LCD_StringUpdate(0, 0, (uint8_t*)&ERROR_POLLING_FAULT_F, 16, OSL_LCD_MODE_STATIC, NO_MATTER);
					OSL_LCD_StringUpdate(1, 0, (uint8_t*)&ERROR_POLLING_FAULT_S, 16, OSL_LCD_MODE_STATIC, NO_MATTER);
				}
				else
				{
					LCD_polling_str = 1;
					OSL_LCD_DisableDisplay();
				}
			}
		}
	}
}

//Колбеки для повторителя по USB

// Проверка ID - по USB принимаем все ID
bool OSL_MDB_Repeater_ID_Check_Callback(Modbus_Params_T *p)
{
	if (p == &USB_Modbus) return 1;
	return 0;
}

uint8_t USART_RS485_data_delayed_repeated[USART_RS485_RX_BUFF];
uint8_t mdb_index;
uint8_t usb_ID;
Modbus_Params_T *prev_mdb = NULL;

// Кадр верный, однако такого адреса нет у платы IO
// Проверка на передачу запроса плате PM по адресу Modbus
bool OSL_MDB_SL_Data_Frame_Callback(Modbus_Params_T *p)
{
	if (p == &USB_Modbus)
	{
		//p->Repeater_flag = 0;
		if (polling_fault) return 0;
		prev_mdb = p;
		usb_ID = p->Rx_Buff[0];
		
		USART_RS485_data_delayed_repeated[0] = Modbus_PULT_RS485_MAS.ID_Device;
		for (mdb_index=1; mdb_index < p->i; mdb_index++)
			USART_RS485_data_delayed_repeated[mdb_index] = p->Rx_Buff[mdb_index];
		
		p->Rx_Params.CRC_Res = CRC16(&USART_RS485_data_delayed_repeated[0], mdb_index);
		USART_RS485_data_delayed_repeated[mdb_index] = (uint8_t)((p->Rx_Params.CRC_Res&0xFF00)>>8);			// Старшая часть
		USART_RS485_data_delayed_repeated[++mdb_index] = (uint8_t)(p->Rx_Params.CRC_Res&0xFF);					// Младшая часть
		mdb_index++;
		
		Modbus_PULT_RS485_MAS.Manual_Request = false;															// На случай, если прежний запрос не успел отработаться
		Master_Par.Oper_Request[MAS_MDB_USB_USART_MANUAL].Cmd = MDB_MAS_REG_CMD_EN;				// Разрешаем передачу запроса, как дойдет очередь
		return 1;		// Отложенный запрос будет
	}
	return 0;// Отложенного запроса не будет
}

// Колбек при сформированной отправке запроса мастера слейву
void OSL_Modbus_Master_Request_Callback(Modbus_Params_T *p, uint16_t Addr)
{
	if (p == &Modbus_PULT_RS485_MAS)
	{
		if (Addr == MAS_MDB_USB_USART_MANUAL)// || (Addr == MAS_MDB_IND_REG_DATA_WR))
		{
			p->Manual_Request = true;				// Этот такт в режиме отправки заранее подготовленного запроса от Slave
			return;
		}
	}
}

// Отложенный запрос мастера, инициированный слейвом
bool OSL_MDB_MAS_Manual_Send_Callback(Modbus_Params_T *p)
{
	if (p == &Modbus_PULT_RS485_MAS)
	{
		if (prev_mdb == NULL)
		{
			p->Manual_Request = false;
			return 0;
		}
		uint8_t k;
		for (k=0; k < mdb_index; k++) p->Tx_Buff[k] = USART_RS485_data_delayed_repeated[k];
		OSL_Modbus_to_Interface_Tx(&Modbus_PULT_RS485_MAS, k);
		
		return 1;
	}
	return 0;
}

//Принятый кадр отложенного приема
bool OSL_MDB_MAS_Manual_Get_Callback(Modbus_Params_T *p)
{
	if (p == &Modbus_PULT_RS485_MAS)
	{
		//Master_Par.Oper_Request[MAS_MDB_USB_USART_MANUAL].Par_Allow = false;
		Master_Par.Oper_Request[MAS_MDB_USB_USART_MANUAL].Cmd = MDB_MAS_REG_CMD_DIS;
		
		if (prev_mdb == NULL) return 0;
		uint8_t mdb_index_5;
		prev_mdb->Tx_Buff[0] = usb_ID;
		for (mdb_index_5=1; mdb_index_5 < p->i; mdb_index_5++)
			prev_mdb->Tx_Buff[mdb_index_5] = p->Rx_Buff[mdb_index_5];
		
		p->Rx_Params.CRC_Res = CRC16(&prev_mdb->Tx_Buff[0], mdb_index_5);
		prev_mdb->Tx_Buff[mdb_index_5] = (uint8_t)((p->Rx_Params.CRC_Res&0xFF00)>>8);		// Старшая часть
		prev_mdb->Tx_Buff[++mdb_index_5] = (uint8_t)(p->Rx_Params.CRC_Res&0xFF);				// Младшая часть

		mdb_index_5++;
		OSL_Modbus_to_Interface_Tx(prev_mdb, mdb_index_5);
		return 1;
	}
	return 0;	
}
				
/****************************************************************************/
//EEPROM
/****************************************************************************/
// Колбек обновления переменной драйвером EEPROM (в том числе в момент инциализации)	
void OSL_EEPROM_VAR_UPDATE_CB(uint32_t Var_Size)
{
	switch (Var_Size - (uint32_t)&grMAIN)
	{
		case GetAdr(Kit[0].Ext_Flash_Addr.Size):	
		case GetAdr(Kit[1].Ext_Flash_Addr.Size):
		{
 			uint8_t Kit_ind_eeprom;
			
			Kit_ind_eeprom = (Var_Size - (uint32_t)&grMAIN - GetAdr(Kit[0].Ext_Flash_Addr.Size))/sizeof(grMAIN.Kit[0]);
			if (grMAIN.Kit[Kit_ind_eeprom].Ext_Flash_Addr.Var == grMAIN.Kit[Kit_ind_eeprom].Ext_Flash_Addr.Min)
			{
				if (!grMAIN.Kit[Kit_ind_eeprom].State.Var.bit.saving)
				{
					grMAIN.Kit[Kit_ind_eeprom].State.Var.bit.full = 0;
					grMAIN.Kit[Kit_ind_eeprom].State.Var.bit.zero = 1;
				}

				grMAIN.Kit[Kit_ind_eeprom].PM_Ver.Var = 0;
				OSL_EEPROM_Add_to_Writing(&grMAIN.Kit[Kit_ind_eeprom].PM_Ver.Size);
				Master_Par.Oper_Request[MAS_MDB_IND_KIT0_PM_VER_WR+Kit_ind_eeprom*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;

				grMAIN.Kit[Kit_ind_eeprom].IO_Ver.Var = 0;
				OSL_EEPROM_Add_to_Writing(&grMAIN.Kit[Kit_ind_eeprom].IO_Ver.Size);
				Master_Par.Oper_Request[MAS_MDB_IND_KIT0_IO_VER_WR+Kit_ind_eeprom*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
				
				grMAIN.Kit[Kit_ind_eeprom].Date_Mdb.Var.all = 0;
				OSL_EEPROM_Add_to_Writing(&grMAIN.Kit[Kit_ind_eeprom].Date_Mdb.Size);
				Master_Par.Oper_Request[MAS_MDB_IND_KIT0_DATE_WR+Kit_ind_eeprom*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;

				grMAIN.Kit[Kit_ind_eeprom].Time_Mdb.Var.all = 0;
				OSL_EEPROM_Add_to_Writing(&grMAIN.Kit[Kit_ind_eeprom].Time_Mdb.Size);
				Master_Par.Oper_Request[MAS_MDB_IND_KIT0_TIME_WR+Kit_ind_eeprom*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
				
				grMAIN.Kit[Kit_ind_eeprom].Motor_Power.Var = 0;
				OSL_EEPROM_Add_to_Writing(&grMAIN.Kit[Kit_ind_eeprom].Motor_Power.Size);
				Master_Par.Oper_Request[MAS_MDB_IND_KIT0_PM_POWER_WR+Kit_ind_eeprom*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
				
				grMAIN.Kit[Kit_ind_eeprom].Motor_Curr.Var = 0;
				OSL_EEPROM_Add_to_Writing(&grMAIN.Kit[Kit_ind_eeprom].Motor_Curr.Size);
				Master_Par.Oper_Request[MAS_MDB_IND_KIT0_PM_CURR_WR+Kit_ind_eeprom*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
			}
			else
			{
				grMAIN.Kit[Kit_ind_eeprom].State.Var.all = 0;
				grMAIN.Kit[Kit_ind_eeprom].State.Var.bit.full = 1;
			}
			Master_Par.Oper_Request[MAS_MDB_IND_KIT0_STATE_WR+Kit_ind_eeprom*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
		}
    break;
		default: return;
	}
}

// Колбек сбоя EEPROM
void OSL_EEPROM_ERROR_CB(void)
{
	grMAIN.Warning_TSPO.Var.bit.TS_EEPROM = 1;
	Master_Par.Oper_Request[MAS_MDB_WARNING_TSPO_WR].Cmd = MDB_MAS_REG_CMD_EN;
}

/****************************************************************************/
//FLASH
/****************************************************************************/
void OSL_EXT_FLASH_ERROR_CB(void)
{
	grMAIN.Warning_TSPO.Var.bit.TS_Flash = 1;
	Master_Par.Oper_Request[MAS_MDB_WARNING_TSPO_WR].Cmd = MDB_MAS_REG_CMD_EN;
}

void OSL_EXT_FLASH_SET_SECTOR_CB(EXT_FLASH_T *p)
{
	p->Oper_Sector = kit_ind;
	p->Sector_Buff[p->Oper_Sector].Writing_Fl_Addr = grMAIN.Kit[p->Oper_Sector].Ext_Flash_Addr.Var;
}

void OSL_EXT_FLASH_DATA_READEN_CB(void)
{
	if (grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.bit.loading)
	{
		switch (grMAIN.Reg.Write_Buff.Bytes.Var)
		{
			case 1: g_Ram_modbus_M[MAS_MDB_IND_REG_DATA_WR].Size = &grMAIN.Reg.Write_Data_8.Size;		break;
			case 2: g_Ram_modbus_M[MAS_MDB_IND_REG_DATA_WR].Size = &grMAIN.Reg.Write_Data_16.Size;	break;
			case 4: g_Ram_modbus_M[MAS_MDB_IND_REG_DATA_WR].Size = &grMAIN.Reg.Write_Data_32.Size;	break;
			default: return;
		}
		EXT_FLASH_Params.Read_Par.Shift++;			// Двигаем сдвиг чтения данных из Flash
		
		g_Ram_modbus_M[MAS_MDB_IND_REG_DATA_WR].Addr = grMAIN.Reg.Write_Buff.Mdb_Addr.Var;
		Master_Par.Oper_Request[MAS_MDB_IND_REG_DATA_WR].Cmd = MDB_MAS_REG_CMD_EN;		// Пишем значение регистра в платы IO\PM
		Master_Par.Oper_Request[MAS_MDB_IND_REG_SHIFT_WR].Cmd = MDB_MAS_REG_CMD_EN;		// Сообщаем плате IO сдвиг
	}
}

void OSL_EXT_FLASH_READ_FINISHED_CB(void)
{
	grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.bit.loading = 0;
	grMAIN.Kit[EXT_FLASH_Params.Oper_Sector].State.Var.bit.loaded = 1;

	grMAIN.PM_Ctrl_Sys_Cmd.Var = 1;
	Master_Par.Oper_Request[MAS_MDB_PM_CTRL_SYS_CMD_WR].Cmd = MDB_MAS_REG_CMD_EN;
	Master_Par.Oper_Request[MAS_MDB_IND_KIT0_STATE_WR+EXT_FLASH_Params.Oper_Sector*KITx_SHIFT].Cmd = MDB_MAS_REG_CMD_EN;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
