#ifndef __osl_ext_eeprom_h_ 
#define __osl_ext_eeprom_h_
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "osl_user_config.h"
#include "BD_g_Ram.h"
//#include "user_g_Ram.h"

#include "spi.h"
	 
#if defined(EXT_EEPROM_AT25640B)
	#include "osl_ext_eeprom_AT25640B.h"
#elif defined(EXT_EEPROM_AT25320B)
	//#include "BD_ext_eeprom_AT25320B.h"
#endif
	 
#define OSL_VAR_MEMORY(NAME, VAR_TYPE)\
typedef union{\
	uint8_t																	Buff[sizeof(VAR_TYPE)];\
	VAR_TYPE																Readen_Value;\
} Var_Memory_COPY_BUFF_##NAME##_T;\
\
typedef struct\
{\
	VAR_TYPE																*Fix_Value;\
	VAR_TYPE																Min;\
	VAR_TYPE																Max;\
	VAR_TYPE																Fact;\
	Var_Memory_COPY_BUFF_##NAME##_T					Copy;\
} Var_Memory_##NAME##_T;

							//NAME		VAR_TYPE
OSL_VAR_MEMORY(8,				int8_t);
OSL_VAR_MEMORY(U8,			uint8_t);
OSL_VAR_MEMORY(16,			int16_t);
OSL_VAR_MEMORY(U16,			uint16_t);
OSL_VAR_MEMORY(32,			int32_t);
OSL_VAR_MEMORY(U32,			uint32_t);
OSL_VAR_MEMORY(FL,			float);
#if defined (IQ_MATH)
OSL_VAR_MEMORY(Q16,			fix16_t);
#endif	 

/* Exported constants --------------------------------------------------------*/
#define MEM_PARAM_FOUND_OK			1
#define MEM_PARAM_NOT_FOUND 		0

typedef enum {
	OSL_EEPROM_MEMORY_CHECK_OK					= 0,		// ������ ������ ��������
	OSL_EEPROM_MEMORY_CHECK_CRC_ERROR		= 1,		// ������ CRC
	OSL_EEPROM_MEMORY_CHECK_LIM_ERROR		= 2,		// ���������� ��������� �� ���������
	OSL_EEPROM_MEMORY_CHECK_PAR_ERROR		= 3,		// ���� ����������� ���� ����������
	OSL_EEPROM_MEMORY_CHECK_DATA_ERROR	= 4,		// �������� ������ (��� ������ �� ��������� ������ ���� ������ ��������)
} OSL_EEPROM_MEMORY_CHECK_T;

#define EEPROM_READY									1
#define EEPROM_BUSY										0

#define EEPROM_BAD_SIZE								2			// �������� �� �������� ��� ������ � EEPROM
#define EEPROM_DATA_LOADED						1			// ������ ��������� � �����
#define EEPROM_BUFFER_OVERLOADED			0			// ����� ����������

#define EEPROM_DEF_UPDATED						1			// ���������� ���������
#define EEPROM_DEF_NOT_UPDATED				0			// ���������� ��� ����� ����������� ��������

/* Exported types ------------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/

typedef enum {
	EEPROM_DUR_OP_MODE_PAR_READING	= 0,			// ������ ���� ����������(������������� ����������)
	EEPROM_DUR_OP_MODE_PAR_WRITING	= 1,			// ������ ���������(��)
	EEPROM_DUR_OP_MODE_DEFAULT			= 2,			// ����� �� ���������
	EEPROM_DUR_OP_MODE_CHIP_ERASE		= 3,			// ������� ���� ������(������� ������ ����������)
	EEPROM_DUR_OP_MODE_ERROR				= 4,			// ������(������ � ����������� �����������)
	EEPROM_DUR_OP_MODE_NEXT					= 5,			// ����� ���������� ������ ������
} EEPROM_DUR_OP_MODE_T;				// ������� ������� �����

typedef union{
	uint8_t all;
	struct {
		uint8_t Par_reading:1;		// 0	������ ���� ����������
		uint8_t Par_writing:1;		// 1	������ ���������(��)
		uint8_t Default:1;				// 2	����� �� ���������
		uint8_t Chip_Erase:1;			// 3	������� ���� ������
		uint8_t rsvd:4;						// 4-7
	} bit;
} EEPROM_PLAN_OP_MODE_T;			// ��������������� ������� �����

typedef enum {
	EEPROM_DUR_ACT_FREE								= 0,
	EEPROM_DUR_ACT_STAT_READING				= 1,		// ������ �������
	EEPROM_DUR_ACT_DATA_READING				= 2,		// ������ ������

	EEPROM_DUR_ACT_FLAG_WRITING				= 3,		// ������ ����� ������
	EEPROM_DUR_ACT_DATA_WRITING				= 4,		// ������ ������
	//EEPROM_DUR_ACT_DEFAULT						= 5,		// ����� � �������� �� ���������
} EEPROM_DUR_ACT_T;				// ������� ��������

typedef union{
	uint16_t	Uns16;
	uint8_t		Array[2];
} EEPROM_CRC_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	OSL_G_VAR_FLAGS_T*					writing_buf[EEPROM_WRITING_BUF_SIZE];		// �������� ����� ������� � ���������� �� ������
	uint16_t										writing_buf_index;											// ������ �������� �������� � ������� �� ������
	uint16_t										writing_elements_num;										// ����� ���������, ��������� ������

	EEPROM_Status_T							Readen_Status;													// ������, ��������� �� ����������			
								
	EEPROM_DUR_OP_MODE_T				During_oper_mode;												// ������� ������� �����
	EEPROM_PLAN_OP_MODE_T				Planned_oper_mode;											// ��������������� ������� �����
	EEPROM_DUR_ACT_T						During_action;													// ������� ��������	
							
	uint16_t										Global_par_write_timer;									// ������ ���������� ����� ������ ������ ���������
	uint16_t										Global_par_read_timer;									// ������ ���������� ����� ������ ������ ���������
	uint16_t										Global_stat_read_timer;									// ������ ���������� ����� ������ ������ �������
	uint16_t										Global_CRC_error_timer;									// ������ ������ CRC
	uint16_t										Global_LIM_error_timer;									// ������ ������ ��������
	uint16_t										Global_DATA_error_timer;								// ������ �������� ������ � ������ (��� ������ �� ���������)
	
	uint16_t										Repeat_par_write_timer;									// ������ ��������� ������ ���������(�������� �� ������������ ������)	
	uint16_t										Repeat_par_read_timer;									// ������ ���������� ������ ���������(� ������ ������������ CRC)
	uint16_t										Repeat_stat_read_timer;									// ������ ���������� ������ �������
							
	uint16_t										Array_Addr;															// ������ �������� ���������
	uint16_t										Array_Addr_Error;												// ������ �������� ���������
	bool												Par_Updated_flag;												// ������������ ��� ���������� ���������(�������� �� �������� ������ CB)	
	bool												Par_Readen_flag;												// ������������ ��� ��������� ���� ����������
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	SPI_DEVICEx_Params_T				SPI_DEVICEx;								// ��������� SPI ����������	
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
	//uint16_t*										Var_pointer;			
	uint16_t										Memory_Addr;														// ����� ���������� � ������
	EEPROM_CRC_T								CRC_Res;
	
	Var_Memory_FL_T							tmp_FL;
	Var_Memory_U32_T						tmp_U32;
	Var_Memory_8_T							tmp_8;
	Var_Memory_U8_T							tmp_U8;
	Var_Memory_16_T							tmp_16;
	Var_Memory_U16_T						tmp_U16;
	Var_Memory_32_T							tmp_32;
#if defined(IQ_MATH)
	Var_Memory_Q16_T						tmp_Q16;	
#endif	
	uint8_t 										i;
	uint8_t											tmb_bit;
} EEPROM_T;

//FUNCTIONS
/* Exported function prototypes -----------------------------------------------*/
SPI_DEVICE_X_RETURN_STATE_T OSL_EEPROM_Memory_Update(void);
void OSL_EEPROM_Callback(void);
void OSL_EEPROM_Init_Params(EEPROM_T *p);
void OSL_EEPROM_ERROR_CB(void);

/* Private function prototypes ------------------------------------------------*/
bool OSL_EEPROM_Var_Search ( EEPROM_T *p );
void OSL_EEPROM_Write_Param_IT ( EEPROM_T *p );
void OSL_EEPROM_Read_Param_IT (EEPROM_T *p);
OSL_EEPROM_MEMORY_CHECK_T OSL_EEPROM_Read_Params_Callback ( EEPROM_T *p );
void OSL_EEPROM_CMD_Write_en_IT ( EEPROM_T *p );
void OSL_EEPROM_Read_Status_IT ( EEPROM_T *p );
bool OSL_EEPROM_Read_Stat_Callback ( EEPROM_T *p );
bool OSL_EEPROM_Default_Params(EEPROM_T *p);
bool OSL_EEPROM_Add_to_Writing(OSL_G_VAR_FLAGS_T* Size);

//VARIABLES
/* Exported variables ---------------------------------------------------------*/
extern EEPROM_T EEPROM_Params;

#ifdef __cplusplus
}
#endif
#endif
