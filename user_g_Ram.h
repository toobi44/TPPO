#ifndef __g_Ram_USER_H_dim
#define __g_Ram_USER_H_dim
#ifdef __cplusplus
 extern "C" {
#endif

#include "osl_user_config.h"
#include "BD_g_Ram.h"
#include "gpio.h"
#include "spi.h"

typedef union{
	uint16_t all;
	struct{
		uint16_t IO_Flash:1;  					// 0 Сбой Flash памяти платы IO
		uint16_t IO_Watch:1;  					// 1 Сбой Flash памяти платы IO
		uint16_t TSPO_Polling:1;  			// 2 Сбой связи с платой TSPO
		
		uint16_t TS_Flash:1;  					// 3 Сбой Flash памяти платы IO
		uint16_t TS_EEPROM:1;  					// 4 Сбой Flash памяти платы IO
		uint16_t rsvd:11;  							// 5-15
	} bit;
} Device_Warning_T;

/******************************************/
OSL_VAR_RO_DEFINE(BUT_STATE,		BUT_State_T);
OSL_VAR_RO_DEFINE(LED_STATE,		LED_State_T);

/*************************************************************/
//LCD
typedef struct{
	uint8_t Code;
	uint8_t Symbols;
} Code_Symbols_T;

typedef union{
	uint16_t all;
	Code_Symbols_T Code_Symbols;
} LCD_Code_Symbols_T;

typedef struct{
	uint8_t Row;
	uint8_t Column;
} Row_Column_T;

typedef union{
	uint16_t			all;
	Row_Column_T	Row_Column;
} LCD_Row_Column_T;

typedef union{
	uint16_t all;	
	uint8_t Symbols[2];
} LCD_Symbols_T;

OSL_VAR_RO_DEFINE(CODE_SYMBOLS,			LCD_Code_Symbols_T);
OSL_VAR_RO_DEFINE(ROW_COLUMN,				LCD_Row_Column_T);
OSL_VAR_RO_DEFINE(SYMBOLS,					LCD_Symbols_T);

typedef struct{
	OSL_Var_RO_CODE_SYMBOLS_T					Code_Symbols;						// Код данных, передаваемых дисплею + число символов
	OSL_Var_RO_ROW_COLUMN_T						Row_Column;							// Строка + Столбец
	OSL_Var_RO_SYMBOLS_T							Buff[8];								// Буфер для передачи	символов\команд
	OSL_Var_RO_BOOL_T									Buff_Flag;							// Флаг, подтверждающий чтение буфера
} grLCD_T;
//LCD
/*************************************************************/

/*************************************************************/
//Kit
typedef union{
	uint16_t all;
	struct{
		uint16_t zero:1;									// 0
		uint16_t full:1;									// 1
		uint16_t deleting:1;							// 2		
		uint16_t saving:1;								// 3
		uint16_t loading:1;								// 4
		uint16_t pm_ver:1;								// 5		
		uint16_t io_ver:1;								// 6		
		uint16_t deleting_fault:1;				// 7

		uint16_t saving_fault:1;					// 8
		uint16_t loading_fault:1;					// 9
		uint16_t loaded:1;								// 9		
		uint16_t rsvd2:5;									// 11-15
	} bit;
} Kit_State_T;

#define KITx_ACTION_MASK (BIT_2_MASK|BIT_3_MASK|BIT_4_MASK)

//#define KIT_BUSY_MASK BIT_2_MASK|BIT_3_MASK|BIT_4_MASK
#define KIT_BUSY_MASK 28

typedef enum{
	KIT_CMD_FREE			= 0,
	KIT_CMD_LOAD			= 1,				// ВЫГРУЗИТЬ
	KIT_CMD_SAVE			= 2,				// СОХРАНИТЬ
	KIT_CMD_DELETE		= 3,				// ОЧИСТИТЬ
} KIT_CMD_T;

OSL_VAR_RO_DEFINE(KIT_STATE,				Kit_State_T);
OSL_VAR_RO_DEFINE(KIT_CMD,					KIT_CMD_T);


typedef union{
  uint16_t all;
  struct {
     uint16_t Min:6;        	// 0-5   Минута
     uint16_t Hour:10;      	// 6-15  Час
  } bit;
} TTimeVar;

typedef union{
  uint16_t all;
  struct {
  	uint16_t Day:5;					// 0-4   День месяца
		uint16_t Month:4;				// 5-8   Месяц
  	uint16_t Year:7;					// 9-15  Год
  } bit;
} TDateVar;

OSL_VAR_RW_D_DEFINE(TIME, TTimeVar);
OSL_VAR_RW_D_DEFINE(DATE, TDateVar);

typedef struct{
	OSL_Var_RO_KIT_STATE_T							State;
	OSL_Var_RO_KIT_CMD_T								Cmd;
	OSL_Var_RW_DATE_D_T									Date_Mdb;
	OSL_Var_RW_TIME_D_T									Time_Mdb;
	OSL_Var_RW_UINT16_D_T								PM_Ver;
	OSL_Var_RW_UINT16_D_T								IO_Ver;
	OSL_Var_RW_UINT16_D_T								Motor_Power;
	OSL_Var_RW_FLOAT_D_T								Motor_Curr;
	OSL_Var_RW_UINT32_D_T								Ext_Flash_Addr;
} grKit_T;
//Kit
/*************************************************************/

/*************************************************************/
//Reg
typedef struct{
	OSL_Var_RO_UINT32_T			Var_Value;
	OSL_Var_RO_UINT16_T			Mdb_Addr;
	OSL_Var_RO_UINT8_T			Bytes;
} Reg_T;

typedef enum{
	REG_STATE_FREE								= 0,				// Бездействие
	REG_STATE_READ_DATA_BUSY			= 1,				// Идет чтение данных (читаемый регистр находится в плате PM)
	REG_STATE_READ_DATA_READY			= 2,				// Данные в плате IO готовы для чтения
	REG_STATE_END_DATA						= 3,				// Все регистры считаны
} REG_STATE_T;

typedef enum{
	REG_CMD_FREE					= 0,
	REG_CMD_KIT0_LOAD			= 1,				// ВЫГРУЗИТЬ
	REG_CMD_KIT0_SAVE			= 2,				// СОХРАНИТЬ
	REG_CMD_KIT0_DELETE		= 3,				// ОЧИСТИТЬ

	REG_CMD_KIT1_LOAD			= 4,				// ВЫГРУЗИТЬ
	REG_CMD_KIT1_SAVE			= 5,				// СОХРАНИТЬ
	REG_CMD_KIT1_DELETE		= 6,				// ОЧИСТИТЬ
} REG_CMD_T;

OSL_VAR_RO_DEFINE(REG_CMD,			REG_CMD_T);

typedef struct{
	OSL_Var_RO_REG_CMD_T							Cmd;
	OSL_Var_RO_UINT16_T								Shift_WR;
	OSL_Var_RO_UINT16_T								Shift_RO;
	Reg_T															Read_Buff;
	Reg_T															Write_Buff;
	OSL_Var_RO_UINT8_P_T							Write_Data_8;	
	OSL_Var_RO_UINT16_P_T							Write_Data_16;
	OSL_Var_RO_UINT32_P_T							Write_Data_32;
  OSL_Var_RO_UINT8_T							  Test_1;	
	OSL_Var_RW_INT16_D_T					  	Test_2;
	OSL_Var_RW_FLOAT_D_T							Test_3;
  OSL_Var_RW_FLOAT_PD_T							Test_4;
} grReg_T;

//Reg
/*************************************************************/
//#define KIT_PAT_NUM 2

OSL_VAR_RO_DEFINE(TIME, TTimeVar);
OSL_VAR_RO_DEFINE(DATE, TDateVar);
OSL_VAR_RO_DEFINE(DEVICE_WARNING,	Device_Warning_T);

typedef struct{
	OSL_Var_RO_BUT_STATE_T						BUT_State;
	OSL_Var_RO_LED_STATE_T						LED_State;
	grLCD_T														grLCD;
	OSL_Var_RO_UINT16_T								Soft_Ver;										// Версия ПО платы TSPO
	OSL_Var_RO_BOOL_T									Polling_Fault_Res;					// Попытка сброса аварии связи
	OSL_Var_RO_DEVICE_WARNING_T				Warning_TSPO;    						// Предупреждения устройства TSPO
	grKit_T														Kit[EXT_FLASH_SECTOR_BUFF];
	grReg_T														Reg;
	OSL_Var_RO_DATE_T									IO_Date_Mdb;
	OSL_Var_RO_TIME_T									IO_Time_Mdb;
	OSL_Var_RO_UINT16_T								PM_Soft_Ver;
	OSL_Var_RO_UINT16_T								IO_Soft_Ver;	
	OSL_Var_RO_UINT16_T								PM_Motor_Power;
	OSL_Var_RO_FLOAT_T								PM_Motor_Curr;
	OSL_Var_RO_UINT16_T								PM_Ctrl_Sys_Cmd;
} grMAIN_T;

extern grMAIN_T grMAIN;

#define SIZE_OF_DIV(X) sizeof(X)/sizeof(X[0])

#ifdef __cplusplus
}
#endif
#endif
