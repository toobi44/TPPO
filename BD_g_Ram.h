#ifndef __BD_g_Ram
#define __BD_g_Ram
#ifdef __cplusplus
 extern "C" {
#endif

#include "osl_user_config.h"
#include "osl_general.h"

typedef uint8_t		U8;
typedef uint16_t	U16;

typedef union{
	uint16_t all;
	struct {
	//Integer
		uint16_t Type_8:1;									// 0
		uint16_t Type_16:1;									// 1
		uint16_t Type_32:1;									// 2
		uint16_t Sign:1;										// 3
	//Point
		uint16_t Type_FL_nQ16:1;						// 4
		uint16_t Points:2;									// 5-6
		uint16_t Float_Real:1;							// 7		
	//Mode
		uint16_t Mode_Write:1;							// 8		
		uint16_t Mode_Pointer:1;						// 9
		uint16_t Mode_Default:1;						// 10	
	//Memory
		uint16_t Mem_Fix:1;									// 11	
	//Menu
		uint16_t Menu_Visible:1;						// 12
	//Modbus
		uint16_t Mdb_Mas_Write_Allowed:1;		// 13				Modbus Master Write allowed
		uint16_t Mdb_Sl_Write_Allowed:1;		// 14				Modbus Slave Write allowed
		uint16_t Mdb_Write_Always:1;				// 15				Modbus Slave or Master Write always
	} bit;
} OSL_G_VAR_FLAGS_T;

//MASKS
//Integer
#define GVAR_T_8										(uint16_t)BIT_0_MASK
#define GVAR_T_16										(uint16_t)BIT_1_MASK
#define GVAR_T_32										(uint16_t)BIT_2_MASK
#define GVAR_T_S										(uint16_t)BIT_3_MASK													// Sign
#define GVAR_T_B										sizeof(bool)

//#define GVAR_ONLY_INT								BIT_0_MASK|BIT_1_MASK|BIT_2_MASK
#define GVAR_ONLY_INT								(uint16_t)0x7						//BIT_0_MASK|BIT_1_MASK|BIT_2_MASK

//Point				
#define GVAR_T_FL										(uint16_t)BIT_4_MASK
#define GVAR_T_Q16									(uint16_t)0x0

#define GVAR_P0											(uint16_t)0x0
#define GVAR_P1											(uint16_t)0x20
#define GVAR_P2											(uint16_t)0x40
#define GVAR_P3											(uint16_t)0x60
#define GVAR_PR											(uint16_t)0x80
//#define GVAR_P4											(uint16_t)0xA0

#define GVAR_T_FL_PR								GVAR_T_FL|GVAR_PR

//Modes
#define GVAR_M_W										(uint16_t)BIT_8_MASK						// Write mode
#define GVAR_M_P										(uint16_t)BIT_9_MASK						// Pointer mode
#define GVAR_M_WP										(uint16_t)0x300									// Write + Pointer mode 								(BIT_8_MASK|BIT_9_MASK)
#define GVAR_M_WD										(uint16_t)0x500 								// Write + Default mode									(BIT_8_MASK|BIT_10_MASK)
#define GVAR_M_WPD									(uint16_t)0x700 								// Write + Pointer + Default mode				(BIT_8_MASK|BIT_9_MASK|BIT_10_MASK)

#define GVAR_MEM_FIX								(uint16_t)BIT_11_MASK													// Memory Fix

//SWITCH MASKS
#define GVAR_ONLY_TYPES							(uint16_t)0x17			// BIT_0-BIT_2, BIT_4
#define GVAR_ONLY_TYPES_AND_SIGN		(uint16_t)0x1F			// BIT_0-BIT_4
#define GVAR_ONLY_POINTS						(uint16_t)0xE0			// BIT_5-BIT_7
#define GVAR_ONLY_WPD								(uint16_t)0x700			// BIT_8-BIT_10	Only Write + Pointer + Default mode

// Modbus modes
#define GVAR_MAS_WR									(uint16_t)BIT_13_MASK
#define GVAR_SL_WR									(uint16_t)BIT_14_MASK
#define GVAR_WR_ALW									(uint16_t)BIT_15_MASK

//COMBO
#define GVAR_W_EDIT									GVAR_M_W|GVAR_SL_WR
#define GVAR_WP_MEM_EDIT						GVAR_M_WP|GVAR_SL_WR
#define GVAR_WD_MEM_EDIT						GVAR_M_WD|GVAR_MEM_FIX|GVAR_SL_WR
#define GVAR_WPD_MEM_EDIT						GVAR_M_WPD|GVAR_MEM_FIX|GVAR_SL_WR

// Modbus modes
//#define RO_M				BIT_0_MASK			// read only
//#define WO_M				BIT_1_MASK			// write only (only unsigned variables)
//#define RW_M				BIT_2_MASK			// read/write
//#define MEM_FIX			BIT_3_MASK			// write with flash/eeprom fixing
//#define GR_B_M			BIT_4_MASK			// graphic buffer mode
//#define WR_ALW			BIT_5_MASK			// write always

#define FIRST_LINE	0
#define SECOND_LINE	1

typedef union{
	uint8_t all;
	struct {
	//Integer
		uint8_t Folder:1;							// 0											// Контейнер(можно класть другие объекты)
		uint8_t Params:3;							// 1-3										// Параметры различного формата
				
		uint8_t Invisible:1;					// 4											// Невидимый объект

		uint8_t Time:1;								// 5											// Время
		uint8_t Date:1;								// 6											// Дата
		
		uint8_t rsvd:1;								// 7
	} bit;
} OSL_G_Menu_Par_FLAGS_T;

#define GVAR_M_FLD								(uint8_t)BIT_0_MASK								// Папка
#define GVAR_M_DEC								(uint8_t)BIT_1_MASK
#define GVAR_M_STR								(uint8_t)BIT_2_MASK
#define GVAR_M_COMB								(uint8_t)0x6							// BIT_1_MASK|BIT_2_MASK
#define GVAR_M_UNION							(uint8_t)BIT_3_MASK
#define GVAR_M_BIN								(uint8_t)0xA							// BIT_1_MASK|BIT_3_MASK

#define GVAR_OBJ_FORM							(uint8_t)0xF							// Формат объекта (биты 0-3)
#define GVAR_MENU_PARAMS					(uint8_t)0xE							// BIT_1_MASK|BIT_2_MASK|BIT_3_MASK

#define GVAR_INVIS								(uint8_t)BIT_4_MASK

/**************************************************************/
#define OSL_SET_TYPES_DEFINE(STRUCT_NAME)\
OSL_VAR_##STRUCT_NAME##_DEFINE(INT8			,		int8_t	);\
OSL_VAR_##STRUCT_NAME##_DEFINE(INT16		,		int16_t	);\
OSL_VAR_##STRUCT_NAME##_DEFINE(INT32		,		int32_t	);\
OSL_VAR_##STRUCT_NAME##_DEFINE(UINT8		,		uint8_t	);\
OSL_VAR_##STRUCT_NAME##_DEFINE(UINT16		,		uint16_t);\
OSL_VAR_##STRUCT_NAME##_DEFINE(UINT32		,		uint32_t);\
OSL_VAR_##STRUCT_NAME##_DEFINE(FLOAT		,		float		);\
OSL_VAR_##STRUCT_NAME##_DEFINE(BOOL			,		bool		);

/**************************************************************/
//READ ONLY{
#define OSL_VAR_RO_DEFINE(NAME, VAR_TYPE)\
typedef struct{\
	OSL_G_VAR_FLAGS_T						Size;\
	VAR_TYPE										Var;\
} OSL_Var_RO_##NAME##_T;

OSL_SET_TYPES_DEFINE(RO);
//READ ONLY}
/**************************************************************/

/**************************************************************/
//READ ONLY + POINTER{
#define OSL_VAR_RO_P_DEFINE(NAME, VAR_TYPE)\
typedef struct{\
	OSL_G_VAR_FLAGS_T						Size;\
	VAR_TYPE*										Var;\
} OSL_Var_RO_##NAME##_P_T;

OSL_SET_TYPES_DEFINE(RO_P);
//READ ONLY + POINTER}
/**************************************************************/

/**************************************************************/
//READ/WRITE{
#define OSL_VAR_RW_DEFINE(NAME, VAR_TYPE)\
typedef struct{\
	OSL_G_VAR_FLAGS_T			Size;\
	VAR_TYPE							Var;\
	VAR_TYPE							Min;\
	VAR_TYPE							Max;\
} OSL_Var_RW_##NAME##_T;

OSL_SET_TYPES_DEFINE(RW);
//READ/WRITE}
/**************************************************************/

/**************************************************************/
//READ/WRITE POINTER{
#define OSL_VAR_RW_P_DEFINE(NAME, VAR_TYPE)\
typedef struct{\
	OSL_G_VAR_FLAGS_T			Size;\
	VAR_TYPE*							Var;\
	VAR_TYPE							Min;\
	VAR_TYPE							Max;\
} OSL_Var_RW_##NAME##_P_T;

OSL_SET_TYPES_DEFINE(RW_P);
//READ/WRITE POINTER}
/**************************************************************/

/**************************************************************/
//READ/WRITE DEFAULT{
#define OSL_VAR_RW_D_DEFINE(NAME, VAR_TYPE)\
typedef struct{\
	OSL_G_VAR_FLAGS_T			Size;\
	VAR_TYPE							Var;\
	VAR_TYPE							Min;\
	VAR_TYPE							Max;\
	VAR_TYPE							Default;\
} OSL_Var_RW_##NAME##_D_T;

OSL_SET_TYPES_DEFINE(RW_D);
//READ/WRITE DEFAULT}
/**************************************************************/

/**************************************************************/
//READ/WRITE POINTER DEFAULT{
#define OSL_VAR_RW_PD_DEFINE(NAME, VAR_TYPE)\
typedef struct{\
	OSL_G_VAR_FLAGS_T			Size;\
	VAR_TYPE*							Var;\
	VAR_TYPE							Min;\
	VAR_TYPE							Max;\
	VAR_TYPE							Default;\
} OSL_Var_RW_##NAME##_PD_T;

OSL_SET_TYPES_DEFINE(RW_PD);
//READ/WRITE POINTER DEFAULT}
/**************************************************************/

typedef struct
{
	const uint8_t*							Value;
	const uint16_t							Size;
} OSL_Var_T;

/*********************************************************/
// VAR
typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	const uint8_t*							Min;
	const uint8_t*							Max;
	const uint8_t*							Value;
	const uint16_t							Size;
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):	
} Var_T;

/*********************************************************/
// FLASH
typedef struct 
{
	const OSL_G_VAR_FLAGS_T*		Size;
	const uint16_t							Addr;
} Memory_Var_T;

/*********************************************************/
// JOURNAL
typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	const OSL_G_VAR_FLAGS_T*		Size;
//	const uint16_t							Modbus_Addr;
//	const uint16_t							Flash_Addr;
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
} Journal_Var_T;

/*********************************************************/
// MODBUS
typedef struct
{
	OSL_G_VAR_FLAGS_T*					Size;
	uint16_t										Addr;
} Modbus_Var_T;

/*********************************************************/
// MENU
typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	const uint8_t*							Pointer;
	const uint8_t								Type;
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
} Menu_Object_T;

/**************************************/
// DEC{
typedef struct
{
	const uint8_t								Str[4];
} Units_T;

typedef struct 
{
	OSL_G_Menu_Par_FLAGS_T			Menu_Flags;							// Флаги распознания объекта меню
	OSL_G_VAR_FLAGS_T*					Var_Flags;							// Флаги типа распознания переменной
	const uint8_t								Main_Str[16];
	const Units_T*							Units_Str;
	const uint8_t								Units_Pos;	
} OSL_Menu_PAR_DEC_T;

// DEC}

/**************************************/
// STRING{
typedef struct
{
	uint8_t											Str[16];
} STR_T;

typedef struct
{
	OSL_G_Menu_Par_FLAGS_T			Menu_Flags;							// Флаги распознания объекта меню
	const OSL_G_VAR_FLAGS_T*		Var_Flags;							// Флаги типа распознания переменной
	const uint8_t								Main_Str[16];
	const STR_T*								Var_Str;
} OSL_Menu_PAR_STR_T;
// STRING}

/**************************************/

//COMB{
typedef struct {
	OSL_G_VAR_FLAGS_T*					Var_Flags;
	uint8_t											Pos_x;	
} OSL_COMB_T;

typedef struct {
	OSL_G_Menu_Par_FLAGS_T			Menu_Flags;							// Флаги распознания объекта меню	
	const OSL_COMB_T*						Var_array;
	const uint8_t								Main_Str[16];
	const uint8_t								Second_Str[16];	
	const uint8_t								Var_Number;
	const uint8_t								Sumbols_Number;
} OSL_Menu_PAR_COMB_T;
//COMB}

/**************************************/

//UNION{
typedef struct 
{
	OSL_G_Menu_Par_FLAGS_T			Menu_Flags;							// Флаги распознания объекта меню
	const OSL_G_VAR_FLAGS_T*		Var_Flags;							// Флаги типа распознания переменной
	const uint8_t								Main_Str[16];
	const uint8_t								Bits_Num;
	const STR_T*								Var_Str;
} OSL_Menu_PAR_UNION_T;
//UNION}

/**************************************/
// BIN{
typedef struct
{
	OSL_G_Menu_Par_FLAGS_T			Menu_Flags;							// Флаги распознания объекта меню
	const OSL_G_VAR_FLAGS_T*		Var_Flags;							// Флаги типа распознания переменной
	const uint8_t								Main_Str[16];
	const uint8_t								Bits_Num;								// Число бит для отображения
	const uint8_t								Main_Pos;								// Положение числа
	const STR_T*								Bit_Str;								// Указатель на массив строк - расшифровка битов	
} OSL_Menu_PAR_BIN_T;

// BIN}

/**************************************/

/************************************************************************************/

// Folders
typedef struct
{
	OSL_G_Menu_Par_FLAGS_T					Menu_Flags;							// Флаги распознания объекта меню
	OSL_G_Menu_Par_FLAGS_T**				Content;								// Указатель на содержимое группы
	const uint8_t										First_Str[16];					// Первая строка
	const uint8_t										Second_Str[16];					// Вторая строка
	const uint8_t										Objects_Num;						// Число объектов в группе
} OSL_Menu_FOLDER_T;
/************************************************************************************/

#if defined (MODBUS_USED)
	#if defined (MODBUS_SLAVE_USED)
		extern Modbus_Var_T						g_Ram_modbus[];
		extern const uint16_t					g_Ram_modbus_Size;
	#endif
	#if defined (MODBUS_MASTER_USED)	
		extern Modbus_Var_T						g_Ram_modbus_M[];
		extern const uint16_t					g_Ram_modbus_Size_M;	
	#endif
	#if defined (MODBUS_GRAPHIC)	
		extern Modbus_Var_T						g_Ram_modbus_Graph[];
		extern const uint16_t					g_Ram_modbus_Size_Graph;	
	#endif
#endif

#if defined (MEMORY_USED)
	#if defined (MEMORY_PARAMS)
		extern Memory_Var_T						g_Ram_memory[];
		extern const uint16_t					g_Ram_memory_Size;
	#endif
	#if defined (JOURNAL_USED)
		extern Journal_Var_T					g_Ram_journal_RD[];
		extern Journal_Var_T					g_Ram_journal_WR[];
		extern const uint16_t					g_Ram_journal_Size;
	#endif
#endif

#if defined (MENU_USED)
	extern const OSL_G_Menu_Par_FLAGS_T*	OSL_Menu_g_Main[];
	extern const uint16_t									g_Ram_menu_Size;
#endif
	
void OSL_Global_Par_Update_CB(uint32_t Var_Addr, uint32_t Source_Addr);
#define GetAdr(Elem)				((uint32_t)&(((grMAIN_T*)0)->Elem))

#ifdef __cplusplus
}
#endif
#endif
