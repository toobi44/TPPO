/* Includes ------------------------------------------------------------------*/
#include "osl_version.h"
#include "project_version.h"
#include "user_g_Ram.h"
#include "tim.h"
#include "spi.h"

float var_1;

__weak void OSL_Global_Par_Update_CB(uint32_t Var_Addr, uint32_t Source_Addr){};

grMAIN_T grMAIN = {
//Size																											Var																	Var_Name
	sizeof(BUT_State_T)|GVAR_MAS_WR|GVAR_WR_ALW,							0,																//BUT_State
	sizeof(LED_State_T),																			0,																//LED_State
	
//grLCD_T	
//Size																											Var																	Var_Name
	sizeof(LCD_Code_Symbols_T),																0,																//Code_Symbols
	sizeof(LCD_Row_Column_T),																	0,																//Row_Column
	
	sizeof(LCD_Symbols_T),																		0,																//Buff[0]
	sizeof(LCD_Symbols_T),																		0,																//Buff[1]
	sizeof(LCD_Symbols_T),																		0,																//Buff[2]
	sizeof(LCD_Symbols_T),																		0,																//Buff[3]
	sizeof(LCD_Symbols_T),																		0,																//Buff[4]
	sizeof(LCD_Symbols_T),																		0,																//Buff[5]
	sizeof(LCD_Symbols_T),																		0,																//Buff[6]
	sizeof(LCD_Symbols_T),																		0,																//Buff[7]
	sizeof(bool)|GVAR_MAS_WR,																	0,																//Buff_Flag
	
	GVAR_T_16|GVAR_MAS_WR,																		HG_REVISION,											//Soft_Ver
	sizeof(bool)|GVAR_MAS_WR,																	1,																//Polling_Fault_Res
	sizeof(Device_Warning_T)|GVAR_MAS_WR,											0,																//DeviceWarning
	
//grKit_T[0]
//Size																											Init															Min														Max												Def												Var_Name							Description	
	sizeof(Kit_State_T)|GVAR_MAS_WR,													0,																																																								//State
	sizeof(KIT_CMD_T),																				KIT_CMD_FREE,																																																			//Cmd
	sizeof(TDateVar)|GVAR_M_WD|GVAR_MEM_FIX|GVAR_MAS_WR,			0,																0,														0xFFFF,										0,											//Date
	sizeof(TTimeVar)|GVAR_M_WD|GVAR_MEM_FIX|GVAR_MAS_WR,			0,																0,														0xFFFF,										0,											//Time
	GVAR_T_16|GVAR_M_WD|GVAR_MEM_FIX|GVAR_MAS_WR,							0,																0,														0xFFFF,										0,											//PM_Ver
	GVAR_T_16|GVAR_M_WD|GVAR_MEM_FIX|GVAR_MAS_WR,							0,																0,														0xFFFF,										0,											//IO_Ver
	GVAR_T_16|GVAR_M_WD|GVAR_MEM_FIX|GVAR_MAS_WR,							0,																0,														0xFFFF,										0,											//Motor_Power
	GVAR_T_FL|GVAR_PR|GVAR_M_WD|GVAR_MEM_FIX|GVAR_MAS_WR,			0.0F,															0.0F,													60000.0F,									0,											//Motor_Curr
	GVAR_T_32|GVAR_M_WD|GVAR_MEM_FIX|GVAR_MAS_WR,							NO_MATTER,												0,														0x1FFF,										0,											//Ext_Flash_Addr				Текущий адрес Flash памяти журнала
	
//grKit_T[1]
//Size																											Init															Min														Max												Def												Var_Name							Description	
	sizeof(Kit_State_T)|GVAR_MAS_WR,													0,																																																								//State
	sizeof(KIT_CMD_T),																				KIT_CMD_FREE,																																																			//Cmd
	sizeof(TDateVar)|GVAR_M_WD|GVAR_MEM_FIX|GVAR_MAS_WR,			0,																0,														0xFFFF,										0,											//Date
	sizeof(TTimeVar)|GVAR_M_WD|GVAR_MEM_FIX|GVAR_MAS_WR,			0,																0,														0xFFFF,										0,											//Time
	GVAR_T_16|GVAR_M_WD|GVAR_MEM_FIX|GVAR_MAS_WR,							0,																0,														0xFFFF,										0,											//PM_Ver
	GVAR_T_16|GVAR_M_WD|GVAR_MEM_FIX|GVAR_MAS_WR,							0,																0,														0xFFFF,										0,											//IO_Ver
	GVAR_T_16|GVAR_M_WD|GVAR_MEM_FIX|GVAR_MAS_WR,							0,																0,														0xFFFF,										0,											//Motor_Power
	GVAR_T_FL|GVAR_PR|GVAR_M_WD|GVAR_MEM_FIX|GVAR_MAS_WR,			0.0F,															0.0F,													60000.0F,									0,											//Motor_Curr
	GVAR_T_32|GVAR_M_WD|GVAR_MEM_FIX|GVAR_MAS_WR,							NO_MATTER,												0x2000,												0x3FFF,										0x2000,									//Ext_Flash_Addr				Текущий адрес Flash памяти журнала
	
//grReg_T	
//Size																											Init															Var_Name							Description	
	sizeof(REG_CMD_T),																				REG_CMD_FREE,										//Cmd																					
	GVAR_T_16|GVAR_MAS_WR,																		0,															//Shift_WR
	GVAR_T_16,																								0,															//Shift_RO
//sizeof(REG_STATE_T),																			REG_STATE_FREE,									//State
//Reg_T
//Size																											Init															Var_Name							Description	
	GVAR_T_32,																								0,															//Var_Value
	GVAR_T_16,																								0,															//Mdb_Addr
	GVAR_T_8,																									0,															//Bytes
//Reg_T
	GVAR_T_32,																								0,															//Var_Value
	GVAR_T_16,																								0,															//Mdb_Addr
	GVAR_T_8,																									0,															//Bytes
	
	GVAR_T_8|GVAR_M_P|GVAR_MAS_WR,														(uint8_t*)&grMAIN.Reg.Write_Buff.Var_Value.Var,											//Write_Data_8
	GVAR_T_16|GVAR_M_P|GVAR_MAS_WR,														(uint16_t*)&grMAIN.Reg.Write_Buff.Var_Value.Var,										//Write_Data_16
	GVAR_T_32|GVAR_M_P|GVAR_MAS_WR,														&grMAIN.Reg.Write_Buff.Var_Value.Var,																//Write_Data_32
  GVAR_T_8,                                                 0,
  GVAR_T_16|GVAR_T_S|GVAR_M_WD|GVAR_SL_WR,                  0,                                -10,                          10,                       0,
  GVAR_T_FL|GVAR_P1|GVAR_M_WD|GVAR_SL_WR|GVAR_MEM_FIX,      0,                                -1.0F,                        1.0F,                     0.7F,
  GVAR_T_FL|GVAR_P1|GVAR_M_WPD|GVAR_SL_WR|GVAR_MEM_FIX,     &var_1,                           -1.0F,                        1.0F,                     0.7F,
  
  
  
	sizeof(TDateVar),																					0,															//IO_Date_Mdbs
	sizeof(TTimeVar),																					0,															//IO_Time_Mdb
	GVAR_T_16,																								0,															//PM_Soft_Ver
	GVAR_T_16,																								0,															//IO_Soft_Ver
	GVAR_T_16,																								0,															//Motor_Power
	GVAR_T_FL|GVAR_PR,																				0.0F,														//Motor_Curr
	GVAR_T_16|GVAR_MAS_WR,																		0,															//PM_Ctrl_Sys_Cmd
};
	
Modbus_Var_T g_Ram_modbus[] = {
  
  &grMAIN.Reg.Test_1.Size,						                      3000,    	//0
  &grMAIN.Reg.Test_2.Size,						                      3001,    	//0
  &grMAIN.Reg.Test_3.Size,						                      3002,    	//0
  &grMAIN.Reg.Test_4.Size,						                      3003,    	//0
  
	&grMAIN.LED_State.Size,						                0,     //1
};
/*
Modbus_Var_T g_Ram_modbus_M[MODBUS_MASTER_PAR_NUM] = {
//Pointer														                Addr					Num
	&grMAIN.BUT_State.Size,						                15000,    	//0
	&grMAIN.LED_State.Size,						                15001,    	//1
	&grMAIN.grLCD.Code_Symbols.Size,							 		15002,			//2
	&grMAIN.grLCD.Row_Column.Size,										15003,			//3
	&grMAIN.grLCD.Buff[0].Size,												15004,			//4
	&grMAIN.grLCD.Buff[1].Size,												15005,			//5
	&grMAIN.grLCD.Buff[2].Size,												15006,			//6
	&grMAIN.grLCD.Buff[3].Size,												15007,			//7
	&grMAIN.grLCD.Buff[4].Size,												15008,			//8
	&grMAIN.grLCD.Buff[5].Size,												15009,			//9
	&grMAIN.grLCD.Buff[6].Size,												15010,			//10
	&grMAIN.grLCD.Buff[7].Size,												15011,			//11
	&grMAIN.grLCD.Buff_Flag.Size,											15012,			//12
	&grMAIN.Soft_Ver.Size,														15013,			//13
	&grMAIN.Polling_Fault_Res.Size,										15014,			//14
	NO_MATTER,		            												NO_MATTER,	//15
	&grMAIN.PM_Soft_Ver.Size,  												15015,			//16
	&grMAIN.IO_Soft_Ver.Size,  												12019,			//17
	&grMAIN.PM_Motor_Power.Size,  										3700,				//18
	&grMAIN.PM_Motor_Curr.Size,  											3705,				//19	
	
	&grMAIN.Kit[0].State.Size, 												15050,			//20
	&grMAIN.Kit[0].Cmd.Size, 													15051,			//21
	&grMAIN.Kit[0].Date_Mdb.Size,											15052,			//22
	&grMAIN.Kit[0].Time_Mdb.Size,											15053,			//23
	&grMAIN.Kit[0].PM_Ver.Size,												15054,			//24
	&grMAIN.Kit[0].IO_Ver.Size,												15055,			//25
	&grMAIN.Kit[0].Motor_Power.Size,									15056,			//26
	&grMAIN.Kit[0].Motor_Curr.Size,										15057,			//27

	&grMAIN.Kit[1].State.Size, 												15100,			//28
	&grMAIN.Kit[1].Cmd.Size, 													15101,			//29
	&grMAIN.Kit[1].Date_Mdb.Size, 										15102,			//30
	&grMAIN.Kit[1].Time_Mdb.Size, 										15103,			//31
	&grMAIN.Kit[1].PM_Ver.Size, 											15104,			//32
	&grMAIN.Kit[1].IO_Ver.Size, 											15105,			//33
	&grMAIN.Kit[1].Motor_Power.Size,									15106,			//34
	&grMAIN.Kit[1].Motor_Curr.Size,										15107,			//35
	
	&grMAIN.Reg.Cmd.Size,		  												15016,			//36
	&grMAIN.Reg.Shift_WR.Size, 												15017,			//37
	&grMAIN.Reg.Shift_RO.Size, 												15018,			//38
	
	&grMAIN.Reg.Read_Buff.Var_Value.Size,		 					15020,			//39
	&grMAIN.Reg.Read_Buff.Mdb_Addr.Size,		 					15022,			//40
	&grMAIN.Reg.Read_Buff.Bytes.Size,		 							15023,			//41
	
	NO_MATTER,		            												NO_MATTER,	//42
	&grMAIN.IO_Date_Mdb.Size,		 											12046,			//43
	&grMAIN.IO_Time_Mdb.Size,		 											12047,			//44

	&grMAIN.Warning_TSPO.Size,							 					15110,			//45
	&grMAIN.PM_Ctrl_Sys_Cmd.Size,							 				3900,				//46
};
*/
Memory_Var_T g_Ram_memory[] = {
//Pointer																						Add
	&grMAIN.Kit[0].Date_Mdb.Size,											0,
	&grMAIN.Kit[0].Time_Mdb.Size,											1,
	&grMAIN.Kit[0].PM_Ver.Size,												2,
	&grMAIN.Kit[0].IO_Ver.Size,												3,
	&grMAIN.Kit[0].Motor_Power.Size,									4,
	&grMAIN.Kit[0].Motor_Curr.Size,										5,
	&grMAIN.Kit[0].Ext_Flash_Addr.Size,								6,
	
	&grMAIN.Kit[1].Date_Mdb.Size,											10,
	&grMAIN.Kit[1].Time_Mdb.Size,											11,
	&grMAIN.Kit[1].PM_Ver.Size,												12,	
	&grMAIN.Kit[1].IO_Ver.Size,												13,
	&grMAIN.Kit[1].Motor_Power.Size,									14,
	&grMAIN.Kit[1].Motor_Curr.Size,										15,
	&grMAIN.Kit[1].Ext_Flash_Addr.Size,								16,
  
  
  &grMAIN.Reg.Test_3.Size,                          17,
  &grMAIN.Reg.Test_4.Size,                          18,
  &grMAIN.Reg.Test_2.Size,						              3001,
};

Journal_Var_T g_Ram_journal_RD[EX_FLASH_JOURNAL_PAR_NUM] = {
	&grMAIN.Reg.Write_Buff.Var_Value.Size,
	&grMAIN.Reg.Write_Buff.Mdb_Addr.Size,
	&grMAIN.Reg.Write_Buff.Bytes.Size,
};

Journal_Var_T g_Ram_journal_WR[EX_FLASH_JOURNAL_PAR_NUM] = {
	&grMAIN.Reg.Read_Buff.Var_Value.Size,
	&grMAIN.Reg.Read_Buff.Mdb_Addr.Size,
	&grMAIN.Reg.Read_Buff.Bytes.Size,
};

#if defined (MODBUS_USED)
	#if defined (MODBUS_SLAVE_USED)
		const uint16_t g_Ram_modbus_Size = sizeof(g_Ram_modbus)/sizeof(g_Ram_modbus[0]);
	#endif
	#if defined (MODBUS_MASTER_USED)
		const uint16_t g_Ram_modbus_Size_M = MODBUS_MASTER_PAR_NUM;
	#endif
		
	#if defined (MODBUS_GRAPHIC)		
		const uint16_t g_Ram_modbus_Size_Graph = sizeof(g_Ram_modbus_Graph)/sizeof(g_Ram_modbus_Graph[0]);
	#endif
#endif

#if defined (MEMORY_USED)
	#if defined (MEMORY_PARAMS)
		const uint16_t g_Ram_memory_Size = sizeof(g_Ram_memory)/sizeof(g_Ram_memory[0]);
	#endif
	#if defined (JOURNAL_USED)
		const uint16_t g_Ram_journal_Size = EX_FLASH_JOURNAL_PAR_NUM;
	#endif
#endif

#if defined (MENU_USED)
	const uint16_t g_Ram_menu_Size = sizeof(OSL_Menu_g_Main)/sizeof(OSL_Menu_g_Main[0]);
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
