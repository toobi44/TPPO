#ifndef __osl_spi_h_
#define __osl_spi_h_
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "osl_general.h"

/* Exported types ------------------------------------------------------------*/	 
typedef enum 
{
	SPI_MODE_RX					= 0,
	SPI_MODE_TX					= 1,
	SPI_MODE_RX_TX			= 2,
} SPI_MODE_T;

typedef enum 
{
	SPI_DEVICE_X_RETURN_STATE_FREE = 0,
	SPI_DEVICE_X_RETURN_STATE_BUSY = 1,
} SPI_DEVICE_X_RETURN_STATE_T;

#define SPI_X_STATE_FREE		(bool)0
#define SPI_X_STATE_BUSY		(bool)1
	 
typedef struct{
	uint32_t				DataSize;
	uint32_t				BaudRatePrescaler;
	//uint32_t				CRCCalculation;
	uint32_t 				CLKPolarity;
	uint32_t 				CLKPhase;	
} SPIx_Reg_Params_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	SPI_MODE_T											data_mode;
	GPIO_TypeDef										*CS_GPIO_Port;
	uint16_t												CS_GPIO_Pin;
	GPIO_PinState										GPIO_Active_Level;
__IO 	uint8_t											*data_send;
__IO 	uint8_t											*data_get;
	SPI_DEVICE_X_RETURN_STATE_T			(*Device_X_func_IT)(void);										// Указатель на функцию IT
	void														(*Device_X_func_CB)(void);										// Указатель на функцию CallBack
	SPI_HandleTypeDef								*hspi_x;
	SPIx_Reg_Params_T								Reg_Params;
// Changing params (must be inited, may be reinited by developer):		
	bool														Enable_Flag;																	// Флаг разрешения опроса данного SPI устройства
	uint16_t												Num_packets;
	// Logic inited params (mustn`t be inited, reinited):
} SPI_DEVICEx_Params_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	uint8_t													Elements_num;																	// Число элементов
	SPI_DEVICEx_Params_T						**SPI_DEVICEx_Params_p;												// Указатель на начальный элемент массива выполения
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
} SPI_TASKx_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	uint8_t													Task_timer;																		// Номер текущего таска
	uint8_t													Busy_Flag;																		// Флаг SPI приемо-передачей длч текущего устройства
	// Ones setting params (mustn`t be changed by developer aftet first initing):
	uint8_t													Tasks_num;																		// Число тасков
	SPI_HandleTypeDef								*hspi_x;
	SPI_TASKx_T											*Zero_task;																		// Указатель на нулевой таск
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
	SPI_TASKx_T											*Operating_Task;
	SPI_DEVICEx_Params_T						*Operating_SPI_Device;												// Указатель SPI устройства, допущенного до обработки
	SPI_DEVICEx_Params_T						**tmp_device;
	uint8_t													index;
} SPIx_Params_T;

//FUNCTIONS
/* Exported function prototypes -----------------------------------------------*/
void OSL_SPI_DEVICEx_Logical(SPI_DEVICEx_Params_T *p);
void OSL_SPI_Data(SPI_DEVICEx_Params_T *p);
void OSL_SPIx_Params_INIT(SPIx_Params_T *p);
void OSL_SPI_X_Callback(SPIx_Params_T *p);
void OSL_SPI_X_IT(SPIx_Params_T *p);

#ifdef __cplusplus
}
#endif
#endif
