#ifndef __modbus_H_dim
#define __modbus_H_dim
#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "osl_user_config.h"
#include "osl_general.h"
	 
#if defined (OSL_LL_TIM)
	 #include "osl_tim.h"
#else
   #include "BD_tim.h" 
#endif
   
#include "osl_crc.h"
#include "BD_g_Ram.h"	 
	 
#if defined MEMORY_USED
	#include "osl_memory.h"	 
#endif
	 
#if defined (BT_MODBUS) || defined (RS485_MODBUS)
	#if defined (OSL_USART_LL)
		#include "osl_usart.h"
	#else
	 #include "BD_usart.h"
	#endif
#endif

#if defined(OSL_USER_MDB_ETH_EN)
	#include "tcp.h"
	#include "osl_eth_connect.h"
#endif	 
	 
#if defined(KERNEL_USED)
	#include "and2kernel.h"
	#include "and2direct.h"
#endif	 

#define FC_READ_HR								0x03			// Read Holding Registers
#define FC_WRITE_MR								0x10			// Write Multiple registers
		
#define PARAM_FOUND_OK									0
#define PARAM_NOT_FOUND_OK							1
	 
#define MDB_PROT_IDENT									(uint16_t)0        //Идентификатор протокола Modbus
//#define QUANTITY_MIN                  	(uint8_t)0x0001
//#define QUANTITY_READ_MAX             	(uint8_t)0x007D
//#define QUANTITY_WRITE_MAX            	(uint8_t)0x007B
//#define PORT_ETH_TCP                  	(uint8_t)0x01F6
//#define PORT_ETH_RTU                  	(uint8_t)0x01F4

//Physical level
typedef enum {
	MDB_INT_RS485		= 0,
	MDB_INT_BT			= 1,
	MDB_INT_USB			= 2,
	MDB_INT_ETH			= 3,
} MDB_INT_T;

//Protocol level
typedef enum {
	MDB_PROTOCOL_RTU		= 0,
	MDB_PROTOCOL_TCP		= 1,
} MDB_PROTOCOL_T;

typedef enum {
	MDB_ROLE_SLAVE	= 0,
	MDB_ROLE_MASTER	= 1,
} MDB_ROLE_T;

typedef enum {
	MDB_REC_STATE_FREE								= 0,			// Не было приема
	MDB_REC_STATE_SUCCESSFUL					= 1,			// Требует штатного ответа (S), либо штатный ответ получен(M)
	MDB_REC_STATE_EXCEPTION						= 2,			// Исключение - требует ответа об ошибке	
	MDB_REC_STATE_WAITING_ANSWER			= 3,			// Ожидание ответа (для Master)
	MDB_REC_STATE_USART_FAULT					= 4,			// Ошибка USART - сгенерировано HAL библиотекой	
	MDB_REC_STATE_ERROR								= 5,			// Неверное ID, Команда, возвращаемое значение колбека
	MDB_REC_STATE_REPEATER_MODE				= 6,			// Режим повторителя
	MDB_REC_STATE_MANUAL_MODE					= 7,			// Режим ручного запроса (Master)
	MDB_REC_STATE_DELAYED_ANSWER			= 8,			// Отложенный запрос (S), на данный	кадр не будет ответа прямо сейчас
} MDB_REC_STATE_T;

typedef union{
	uint16_t all;
	struct {
		uint16_t		ID_Mistake:1;										// 0				Несовпадение ID
		uint16_t		Unknown_Command:1;							// 1				Неизвестная команда
		uint16_t		CRC_Mistake:1;									// 2				Неверная контрольная сумма
		uint16_t		Rx_Buffer_overload:1;						// 3				Запрос на считывание превышает буфер
		uint16_t		Tx_Buffer_overload:1;						// 4				Запрос на запись превышает буфер
		uint16_t		No_access:1;										// 5				Адрес данных, указанный в запросе, недоступен (при чтении и записи)
		uint16_t		Over_Lim:1;											// 6				Записываемое значение превышает дотсупный диапазон
		uint16_t		Zero_Num_Regs:1;								// 7				Число регистров = 0

		uint16_t		Zero_Num_Bytes:1;								// 8				Число байт == 0
		uint16_t		Short_Packet:1;									// 9				Короткий пакет принят
		uint16_t		Unknown_Exception:1;						// 10				Неизвестное исключение
		uint16_t		Unexpected_Rx:1;								// 11				Ответ не соответствует логике устройства
    uint16_t		Rx_Buf_Pointer:1;							  // 12				Указатель на буфер приема не инициализирован
    uint16_t		Protocol_Mdb:1;									// 13				Не протокол MODBUS
		//uint16_t		Quantity_Registers:1;						// 14				Количество регистров
		uint16_t		rzvd:2;													// 15
	} bit;
} Mdb_Error_T;

typedef enum {
	MDB_EXC_CODE_FREE				= 0,									// Исключение отсутсвует
 	MDB_EXC_CODE_CMD			  = 1,               		// Неправильная команда
	MDB_EXC_CODE_NO_ACCESS	= 2,									// Адрес данных, указанный в запросе, недоступен (при чтении и записи)
	MDB_EXC_CODE_OVER_LIM		= 3,									// Значение, содержащееся в поле данных запроса, является недопустимой величиной
} MDB_EXC_CODE_T;

typedef struct {	
//MBAP Header (Modbus Application Header)
  uint16_t                    Trans_Ident;                // Transaction Identifier (Identification of a MODBUS Request)
  uint16_t                    Prot_Ident;                 // Protocol Identifier (0 = MODBUS protocol)
  uint16_t                    Length;                     // Number of following bytes
	uint8_t											ID;													// Unit Identifier \ Slave ID

	uint8_t											Func_Code;									// Команда (код функции)
	uint16_t										Data_addr;									// Адрес данных
	uint16_t										Num_reg;										// Количество регистров
	uint16_t										CRC_Res;										// Контрольная сумма
	uint8_t											Bytes;											// Количество байтw
} Mdb_Rx_Tx_T;

#define TYPEDEF_INT_VAR_MODBUS_X(VAR_TYPE)\
typedef struct\
{\
	VAR_TYPE											Min;\
	VAR_TYPE											Max;\
	VAR_TYPE											*Value;\
	VAR_TYPE											Data;\
} Var_Modbus_##VAR_TYPE##_T;

#define TYPEDEF_POINT_VAR_MODBUS_X(VAR_TYPE)\
typedef struct\
{\
	VAR_TYPE											Min;\
	VAR_TYPE											Max;\
	VAR_TYPE											*Value;\
	VAR_TYPE											Data;\
	uint16_t											Sign_Symbols;\
} Var_Modbus_##VAR_TYPE##_T;

TYPEDEF_INT_VAR_MODBUS_X(int8_t);
TYPEDEF_INT_VAR_MODBUS_X(uint8_t);
TYPEDEF_INT_VAR_MODBUS_X(int16_t);
TYPEDEF_INT_VAR_MODBUS_X(uint16_t);
TYPEDEF_INT_VAR_MODBUS_X(int32_t);
TYPEDEF_INT_VAR_MODBUS_X(uint32_t);

TYPEDEF_POINT_VAR_MODBUS_X(float);
#if defined (IQ_MATH)
TYPEDEF_POINT_VAR_MODBUS_X(fix16_t);
#endif	

typedef struct
{
// Ones setting params (mustn`t be changed by developer aftet first initing):
	uint16_t*								Out_Data;
	Modbus_Var_T*						g_Ram_modbus_x;										// Указатель на массив с адресами и уровнем доступа
	uint16_t								g_Ram_modbus_Size_x;							// Размер массива
	bool										Conv_only_flag;	
// Logic inited params (mustn`t be inited, reinited):
	Var_Modbus_int8_t_T			tmp_str_8;
	Var_Modbus_uint8_t_T		tmp_str_U8;	
	Var_Modbus_int16_t_T		tmp_str_16;
	Var_Modbus_uint16_t_T		tmp_str_U16;
	Var_Modbus_int32_t_T		tmp_str_32;
	Var_Modbus_uint32_t_T		tmp_str_U32;
	Var_Modbus_float_T			tmp_str_FL;
#if defined (IQ_MATH)
	Var_Modbus_fix16_t_T		tmp_str_Q16;
#endif	
	uint8_t									Num_reg;
	uint16_t								Addr;
	bool										write_flag;
	uint16_t 								Var_Timer;
	uint16_t 								Tmp_Sum;
} Modbus_Conv_Data_T;	

#if defined(MODBUS_SLAVE_USED)
	#if defined (MODBUS_GRAPHIC)

#define   TRIG_MODE_AND_BUFF_FULL           5
#define   TRIG_MODE_AND_TRIG_WAIT           9
#define   ROUND_MODE_AND_BUFFER_NOT_FULL    2
#define   ROUND_MODE_AND_BUFFER_FULL        6

	typedef union{
		uint8_t all;
		struct {
			uint8_t		Trig_Mode:1;								// 0 - Режим триггера(по событию)
 			uint8_t		Round_Rewrite:1;						// 1 - Круговая перезапись(режим без триггера)     
			uint8_t		Buffer_Full:1;  						// 2 - Буфер заполнен(можно выгружать данные)
			uint8_t		Trigger_Waiting:1;					// 3 - Ожидание триггера
			uint8_t		rsvd:4;											// 4-7
		} bit;
	} Trigger_State_T;

	typedef struct
	{
		// Function inited (must be inited by special function ones):
		uint16_t							Gr_Buffer[2][MAX_GRAPH_BUFFER];
		bool									Monitor_flag;														// Флаг показываем, что настройки графиков были залиты в ОЗУ
		uint16_t							Point_offset;
		uint16_t							Buffer_addr;														// Текущий адрес буфера, в который сейчас идет запись
		uint16_t							Buffer_addr_tmp;
		uint16_t							During_Addr;
		uint16_t							Graphic_num;	
		//Trigger params
		bool									Allow_flag;															// Флаг для пользователя, разрешающий выгрузку данного значения в буфер
		bool									trigger_mode;
		bool									trigger_RefreshBuffer_CMD;
		bool 									trigger_in_process;
		bool 									trigger_buffer_rewritten;								// Флаг перезаписаности буфера после активации
		
		uint16_t							during_buffer_limit;
		uint16_t							buffer_shift;
		uint16_t							leftover_buffer_timer;

		bool									Trigger_Blocked;												// 
		bool 									round_buffer_rewrite_flag;
		Trigger_State_T				Trigger_State;													// Состояние триггера
		bool 									trigger_multiple_16_flag;								// Флаг кратности таймера буфера 16
		uint16_t							size_convert_rez;												// Результат преобразования данных для буфера
	// Ones setting params (mustn`t be changed by developer aftet first initing):
		uint16_t							RefreshBuffer_CMD;
		uint16_t							SetScreen_CMD;
		uint16_t							SetPeriod_CMD;
		uint8_t							  Trigger_Activate_CMD;										// Команда на активацию триггера
		uint16_t							Addr_CH1;
		uint16_t							Addr_CH2;
		uint16_t							Buffer_size;
		Modbus_Conv_Data_T		Conv_Data;
	// Changing params (must be inited, may be reinited by developer):
		float									trigger_part;
		uint8_t								Data_Size;															// Число байт, которое необходимо за один заход загрузчить в буфер
		uint8_t								tmp_Ind;	
	// Logic inited params (mustn`t be inited, reinited):	
	} Graphic_Buffer_T;
	#endif

	#if defined(KERNEL_USED)
	typedef struct
	{
	// Ones setting params (mustn`t be changed by developer aftet first initing):
		float									*Int_Freq;															// Указатель на частоту, отвечающего за отработку тайм-аутов
	// Logic inited params (mustn`t be inited, reinited):
		uint16_t							TA15_mks;																// [мк сек] AT интервал 1.5 символа после завершения передачи и переходом в прием	
		uint16_t							TA25_mks;																// [мк сек] AT интервал 2.5 символа (т.к. проверяется после приёма байта)
		uint16_t							TA35_mks;																// [мк сек] AT интервал 3.5 символа
		And2_PDB_t						pdb_F_OBJ_Modbus;												// PDB отложенного прерывания
		And2_PDB_t						pdb_PF_OBJ_Modbus;											// PDB приоритетной функции
	} Modbus_Kernel_T;	                                            
	#endif
#endif
	
#if defined(MODBUS_MASTER_USED) && defined(MODBUS_MASTER_PAR_NUM)
typedef struct {
	MDB_REC_STATE_T							Receive_State;
	Mdb_Error_T									Error;
	MDB_EXC_CODE_T							Exception;
} Modbus_Master_Par_State_T;

typedef enum {
	MDB_MAS_REG_CMD_FREE	= 0,
	MDB_MAS_REG_CMD_EN		= 1,
	MDB_MAS_REG_CMD_DIS		= 2,
} MDB_MAS_REG_CMD_T;	

typedef struct {
	uint16_t										Var_Num;
	MDB_MAS_REG_CMD_T						Cmd;
	bool												Par_Allow;
} Oper_Request_T;	

typedef struct
{
// Function inited (must be inited by special function ones):
	//uint8_t											ID_timer;																										// ID Текущего Slave
	uint16_t										Par_timer;																									// Номер текущего парметра
	//Modbus_Master_Par_State_T		Par_State[MODBUS_MASTER_ID_NUM][MODBUS_MASTER_PAR_NUM];			// Статус каждого параметра	
	Modbus_Master_Par_State_T		Par_State[MODBUS_MASTER_PAR_NUM];														// Статус каждого параметра
	//bool												first_request_flag;																					// Первый запрос(ответа еще не должно быть)
	TIM_Calc_Params_T						TIM_Answer_Time;																						// Структура для расчета времени окончания ожидания ответа
	Modbus_Conv_Data_T					Mas_Conv_Data;
	uint16_t										Low_priority_Addr;																					// Текущий низкоприоритетный параметр
	uint16_t										Low_priority_Flag;																					// Флаг низкоприоритетного параметра
	bool												Par_Allow_During;																						// Опрос текущего адреса разрешен
// Ones setting params (mustn`t be changed by developer aftet first initing):
	//uint8_t											Slave_ID;																										// ID Подчиненных устройсв
	uint32_t										Answer_Lim_Time;																						// [мкс] Время окончания ожидания ответа
// Changing params (must be inited, may be reinited by developer):
	Oper_Request_T							Oper_Request[MODBUS_MASTER_PAR_NUM];
// Logic inited params (mustn`t be inited, reinited):
	uint16_t										tmp_Par;
	bool												Mas_Request_flag;
} Modbus_Master_T;
#endif

typedef struct
{
	// Function inited (must be inited by special function ones):
	IND_PROT								Inductrial_Protocol;							// Переменная для выбора протокола
#if defined (OSL_LL_DMA)
	uint32_t					      Byte_timer;												// Регистр, считающий принятые байты
#else
	__IO uint32_t*					Byte_timer;												// Регистр, считающий принятые байты
#endif

#if defined (BT_MODBUS) || defined (RS485_MODBUS)
	USARTx_Params_T*				USART;														// Указатель на USART структуру
#endif
	uint32_t								Bytes_Delta;											// Число байт, ушедших в обработку
	uint32_t								Byte_timer_prev;									// Предыдущее количество принятых байт	
	TIM_Calc_Params_T				TIM_Reset_Time;										// Структура для расчета времени сброса приема
	uint16_t								Addr;															// Адрес в массиве g_Ram_modbus[]
	uint32_t								Mdb_Timer_Break_Receive;					// Число прерванных приемов
	uint32_t								Mdb_Timer_No_Answer;							// Число запросов не требующих ответа(S), запросов без ответа(M)
	uint32_t								Mdb_Timer_Success_Answer;					// Число штатных ответов(S,M)
	uint32_t								Mdb_Timer_Exception;							// Число отвеченных(S), принятых(M) исключенипй
	uint32_t								usb_Timer;		
	uint8_t									ID_Device;												// ID устройства
	
	MDB_EXC_CODE_T					Exception;												// Код исключения
	Mdb_Error_T							Error;														// Аварии
	MDB_REC_STATE_T					Receive_State;										// Статус приема
	
	uint16_t 								Mdb_Rx_Data[MAX_TX_DATA_SIZE];		// Считанные данные
	uint16_t 								Mdb_Tx_Data[MAX_TX_DATA_SIZE];		// Отправленные данные	
	
	Mdb_Rx_Tx_T							Rx_Params;												// Принятый пакет
	Mdb_Rx_Tx_T							Tx_Params;												// Оправляемый пакет
		
	bool										journal_flag;											// Флаг для отправки ответа на запрос журнала
	bool										Repeater_flag;										// Флаг ретранслятора
	bool										Manual_Request;										// Запрос, формируемый вручную пользователем
// Ones setting params (mustn`t be changed by developer aftet first initing):	

	uint8_t*								Rx_Buff;
	uint8_t*								Tx_Buff;
		
	MDB_INT_T								Interface;												// Интерфейс(USB, RS-485, Bluetooth, Eth)
	MDB_PROTOCOL_T					Mdb_Protocol;											// Протокол RTU\TCP
	MDB_ROLE_T							Role;															// Роль устройства (Master\Slave)
#if defined (RS485_MODBUS)
	GPIO_TypeDef*						RS_485_Port;											// Порт ножки переключения направления приемо-передачи
	uint16_t								RS_485_Pin;												// Номер ножки переключения направления приемо-передачи
#endif

#if defined(MODBUS_SLAVE_USED)	
	#if defined (MODBUS_GRAPHIC)
		Graphic_Buffer_T*			Graphic;
	#endif

	#if defined (EXT_EEPROM_USED)
		EEPROM_PLAN_OP_MODE_T*	Planned_oper_mode;
	#elif defined(FLASH_USED)
		bool*									Par_Rewrite_flag;
	#endif

	#if defined(KERNEL_USED)
		Modbus_Kernel_T*			Kernel;
	#endif
#endif
		
#if defined MODBUS_MASTER_USED && defined MODBUS_MASTER_PAR_NUM
	Modbus_Master_T*				Mas_Par;
#endif
	
	Modbus_Conv_Data_T			Mdb_Conv_Data;
	
// Changing params (must be inited, may be reinited by developer):
	uint32_t								Reset_Timeout;
// Logic inited params (mustn`t be inited, reinited):
	uint8_t									i;
	uint8_t									k;
#if defined(OSL_USER_MDB_ETH_EN)
	eth_struct 							*es_tcp;
  struct pbuf 						*ptr;
	struct tcp_pcb 					*client;
	struct tcp_pcb					*server;	
#endif
} Modbus_Params_T;
/*********************************************************/

void OSL_Mdb_RTU_TCP_Slave_Successful_Tx(Modbus_Params_T *p);
void OSL_Mdb_RTU_TCP_Slave_Exception_Tx(Modbus_Params_T *p);

#if defined (BT_MODBUS) || defined (RS485_MODBUS)
	void OSL_Modbus_INIT(Modbus_Params_T *p, USARTx_Params_T *USART);
	void OSL_Modbus_Restore_Receiving(Modbus_Params_T *p);
#elif defined(USB_MODBUS)
	void OSL_Modbus_INIT(Modbus_Params_T *p);
#endif

void OSL_MDB_Frame_end_Timeout(Modbus_Params_T *p, TIMx_Params_T *v);
bool OSL_Modbus_Var_Search(Modbus_Params_T *p);
bool BD_Modbus_Check_Limits (Modbus_Params_T *p);
void OSL_Modbus_Graphics_Send (Modbus_Params_T *p);
void OSL_Modbus_Graphic_Update(Modbus_Params_T *p);

#define TRIGGER_WORK_BANNED		0
#define TRIGGER_WORK_ALLOWED	1
bool OSL_MODBUS_TriggerSet(Modbus_Params_T *p);
void OSL_MODBUS_TriggerActivate(Modbus_Params_T *p);

// Колбеки
void OSL_MODBUS_BufferUpdateCallback(Modbus_Params_T *p, uint8_t graph_num);
void OSL_MODBUS_GraphicTriggerSearchCallback(Modbus_Params_T *p, uint8_t graph_num);
void OSL_MODBUS_AddrRequestCallback(Modbus_Params_T *p, uint16_t Addr);
void OSL_MODBUS_AddrWriteCallback(Modbus_Params_T *p, uint32_t Var_Size);
void OSL_Mdb_Slave_Packet_Check(Modbus_Params_T *p);
void OSL_Modbus_Master_Packet_Check(Modbus_Params_T *p);
void OSL_Packet_Check_Modbus_Master(Modbus_Params_T *p);
void OSL_Modbus_Size_Convert(Modbus_Params_T *p, Modbus_Conv_Data_T *c);
void OSL_MODBUS_ClearGettedData(Modbus_Params_T *p, uint16_t Num_packets_get_prev);

#if defined (MODBUS_GRAPHIC)
void OSL_Modbus_Graphic_INIT(Graphic_Buffer_T *p);
#endif
void OSL_Modbus_Master_Request(Modbus_Params_T *p);
void OSL_Modbus_Master_INIT(Modbus_Params_T *p);
//bool OSL_Modbus_Master_Change_ID_Par(Modbus_Params_T *p);
void OSL_Modbus_Master_Timeout_Request(Modbus_Params_T *p, TIMx_Params_T *v);

#if defined(KERNEL_USED)
void func_F_OBJ_Modbus(void);
void func_PF_OBJ_Modbus(void);
#endif

#if defined(MODBUS_MASTER_USED)

#endif

#ifdef __cplusplus
}
#endif
#endif
