/* Includes ------------------------------------------------------------------*/
#include "BD_gpio_spi_lcd.h"

#if defined(WINSTAR_EXT_CALLBACK)
__weak void OSL_WINSTAR_WriteIns_CB(uint8_t data){}
__weak void OSL_WINSTAR_SET_ADR_CB(uint8_t row, uint8_t column){}
__weak bool OSL_MENU_WRITE_PAR_TO_MEMORY_CB(OSL_G_VAR_FLAGS_T* Size){return 0;}
__weak void OSL_WINSTAR_WriteData_CB(uint8_t data){};
__weak void OSL_WINSTAR_UPDATE_STR_CB(uint8_t row, uint8_t column, uint8_t* data, uint8_t sym_num){};
__weak void OSL_WINSTAR_DisableDisplay_CB(void);
#endif

const uint8_t Clear_Text[DISPLAY_COLUMNS] = {"                "};

uint8_t prev_sym_num[5] = {0};
uint8_t prev_sym[5][DISPLAY_COLUMNS] = {0};
uint8_t blink_flag = 0;

bool OSL_LCD_StringUpdate(uint8_t row, uint8_t column, uint8_t *str, uint8_t sym_num, OSL_LCD_MODE_T MODE, uint8_t par_num)
{
	uint8_t i;
	
	switch(MODE)         // Если есть знаки после запятой, домножаем
	{
		case OSL_LCD_MODE_STATIC:
		{
			// Обнуляем, чтобы при обновлении динамических данных первый раз точно не совпало
			for (i = 0; i < 5; i ++) prev_sym_num[i] = 0;
			if (blink_flag)
			{
				blink_flag = 0;
				OSL_BlinkCursorDis();
			}
			OSL_LCD_UPDATE_STR(row, column, str, sym_num);
			return 1;
		}
		break;

		case OSL_LCD_MODE_DYNAMIC_DIR: 
		{
			// Если есть новая информация для выведения, только выводим ее на экран
			if (sym_num != prev_sym_num[par_num] || memcmp(str, prev_sym[par_num], sym_num) != 0)
			{
				if (blink_flag)
				{
					blink_flag = 0;
					OSL_BlinkCursorDis();
				}
				if (sym_num < prev_sym_num[par_num])
				{
					OSL_LCD_UPDATE_STR(row, column + sym_num, (uint8_t*)&Clear_Text[0], prev_sym_num[par_num]-sym_num);
				}
				prev_sym_num[par_num] = sym_num;
				OSL_LCD_UPDATE_STR(row, column, str, sym_num);
				for (i = 0; i < sym_num; i++) prev_sym[par_num][i] = str[i];
				return 1;
			}
		}	
		break;

		case OSL_LCD_MODE_DYNAMIC_INV:
		{
			// Если есть новая информация для выведения, только выводим ее на экран
			if (sym_num != prev_sym_num[par_num] || memcmp(str, prev_sym[par_num], sym_num) != 0)
			{
				if (blink_flag)
				{
					blink_flag = 0;
					OSL_BlinkCursorDis();
				}				
				if (sym_num < prev_sym_num[par_num])
				{
					OSL_LCD_UPDATE_STR(row, column - prev_sym_num[par_num], (uint8_t*)&Clear_Text[0], prev_sym_num[par_num]-sym_num);
				}
				prev_sym_num[par_num] = sym_num;
				OSL_LCD_UPDATE_STR(row, column-sym_num, str, sym_num);
				for (i = 0; i < sym_num; i ++) prev_sym[par_num][i] = str[i];
				return 1;
			}
		}
		break;
		default: return 0;
	}
	return 0;
}

void OSL_LCD_WriteIns_Func(uint8_t data)
{
	if (data == CURSOR_EN) 
		blink_flag = 1;
	else if (data == BLINK_CURSOR_DIS) blink_flag = 0;
	OSL_LCD_WriteIns(data);	
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
