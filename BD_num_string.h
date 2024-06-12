#ifndef __BD_num_string_H_
#define __BD_num_string_H_
#ifdef __cplusplus
 extern "C" {
#endif

#include "osl_user_config.h"
#include "osl_general.h"

uint8_t OSL_INT_SYMBOLS_COUNT(int32_t dig, uint8_t f_n_bit);
uint8_t OSL_UINT_SYMBOLS_COUNT(uint32_t dig, uint8_t f_n_bit);
	 
void OSL_INT_TO_STR(uint8_t *str, int32_t Var, uint8_t* num, uint8_t delta);
void OSL_UINT_TO_STR(uint8_t *str, uint32_t dig, uint8_t num, uint8_t delta);

typedef struct {
	uint32_t		tmp_32;
	float				tmp_FLOAT;
	uint8_t			symbol_num;
	uint8_t			array_num;
	uint8_t			i;
	uint8_t			tmp_array[16];
	int8_t			symbols_delta;
	int8_t			symbols_delta_2;
	uint8_t     num_symbols;
	uint8_t			Min_Symbols;							// Число значащих символов минимума
	uint8_t			Max_Symbols;							// Число значащих символов максимума
	uint8_t			Lim_Symbols;							// Предельное число значащих символов
	uint8_t			Zero_flag;	
} num_string_FLOAT_T;

typedef enum {
	FL_STR_MODE_SYMBOLS_ONLY		= 0,						// Отображать только значащие символы
	FL_STR_MODE_ZERO						= 1,						// Заполнять нудями пустые ячейки(при корректировке)
} FL_STR_MODE_T;

void OSL_FLOAT_TO_STRING(uint8_t *buffer_array, float var, uint8_t signs, num_string_FLOAT_T *p, FL_STR_MODE_T Mode);
void OSL_FLOAT_TO_STRING_Lim_INIT(uint8_t signs, float Min, float Max, num_string_FLOAT_T *p);
uint8_t OSL_UINT_TO_STRING_Lim_INIT(uint32_t Min, uint32_t Max, uint8_t Size);
uint8_t OSL_INT_TO_STRING_Lim_INIT(int32_t Min, int32_t Max, uint8_t Size);

#ifdef __cplusplus
}
#endif
#endif

