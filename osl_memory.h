#ifndef __osl_memory_h_
#define __osl_memory_h_
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/	 
#include "osl_user_config.h"
#include "osl_general.h"

#include "BD_g_Ram.h"	 
	 
//TYPES
/* Exported types -------------------------------------------------------------*/ 	 
#if defined(EXT_EEPROM_USED)
	#include "osl_ext_eeprom.h"
	
	#if defined(EXT_FLASH_USED)
		#include "osl_ext_flash.h"
	#endif
	 
#elif defined(FLASH_USED)
	#include "osl_flash.h"
#endif
	 
//FUNCTIONS
/* Exported function prototypes -----------------------------------------------*/
//void OSL_Memory_Update(void);	
void OSL_Memory_Init_Params(void);
	 
#if defined(EXT_EEPROM_USED)
void OSL_Memory_Callback(SPI_HandleTypeDef *hspi);
#endif

#ifdef __cplusplus
}
#endif
#endif
