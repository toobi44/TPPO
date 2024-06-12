#ifndef __dyn_array_H_dim
#define __dyn_array_H_dim
#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "stm32f1xx_it.h"
#include "dim_lib_spi.h"

#include <stdbool.h>
#include "rt_heap.h"

typedef struct 
{
	// Changing params (must be inited, may be reinited by developer):
	uint8_t								Num_packets;

	// Function inited (must be inited by special function ones):
//	bool								array_was_inited;
	bool								first_array_size_calc;

	uint8_t								Num_packets_prev;
	SPI_DataSize_T						SPI_DataSize_prev;
	
	// Logic inited params (mustn`t be inited, reinited):
	uint8_t								*dyn_get_array;
	uint8_t								*dyn_send_array;

	uint8_t								sizeof_var;
	
} SPI_Dyn_Array_Params_T;

void SPI_Dyn_Array_Init_Reinit (SPI_Dyn_Array_Params_T *p, SPI_DataSize_T SPIx_DataSize);
void SPI_Dyn_Arrays_Init_Params (SPI_Dyn_Array_Params_T *p);

void *malloc (unsigned int size);
void free (void *p);

#ifdef __cplusplus
}
#endif
#endif
