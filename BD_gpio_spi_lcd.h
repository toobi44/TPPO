#ifndef __LCD_H_dim
#define __LCD_H_dim
#ifdef __cplusplus
 extern "C" {
#endif

#include "osl_user_config.h"
#include "osl_general.h"
#include "BD_tim.h"
#include "BD_g_Ram.h"
	 
typedef enum {
	OSL_LCD_MODE_STATIC					= 0,		// Запись символов без проверки изменения переменных
	OSL_LCD_MODE_DYNAMIC_DIR		= 1,		// Прямая запись символов с проверки изменения переменных
	OSL_LCD_MODE_DYNAMIC_INV		= 2,		// Инверсная запись символов с проверки изменения переменных
} OSL_LCD_MODE_T;	 

#if defined(WEH1602)
	#include "gpio.h"
	#include "BD_lcd_winstar_1602.h"

	#define	OSL_LCD_BlinkEn()												OSL_WINSTAR_WriteIns(BLINK_EN)
	#define	OSL_LCD_CursorEn()											OSL_WINSTAR_WriteIns(CURSOR_EN)
	#define	OSL_BlinkCursorDis()										OSL_WINSTAR_WriteIns(BLINK_CURSOR_DIS)
	#define	OSL_LCD_WriteIns(a)											OSL_WINSTAR_WriteIns(a)
	#define	OSL_LCD_SetAddr(a,b)										OSL_WINSTAR_SET_ADR(a,b)
	#define	OSL_MEMORY_PAR_WRITE(a)									OSL_EEPROM_Add_to_Writing(a)
	#define	OSL_LCD_WriteData(a)										OSL_WINSTAR_WriteData(a)
	#define	OSL_LCD_UPDATE_STR(a,b,c,d)							OSL_WINSTAR_UPDATE_STR(a,b,c,d)
	#define	OSL_LCD_DisableDisplay()								OSL_WINSTAR_DisableDisplay()

#elif defined(WINSTAR_EXT_CALLBACK)
	#include "BD_lcd_winstar_1602.h"

	#define	OSL_LCD_BlinkEn()												OSL_WINSTAR_WriteIns_CB(BLINK_EN)
	#define	OSL_LCD_CursorEn()											OSL_WINSTAR_WriteIns_CB(CURSOR_EN)
	#define	OSL_BlinkCursorDis()										OSL_WINSTAR_WriteIns_CB(BLINK_CURSOR_DIS)
	#define	OSL_LCD_WriteIns(a)											OSL_WINSTAR_WriteIns_CB(a)
	#define	OSL_LCD_SetAddr(a,b)										OSL_WINSTAR_SET_ADR_CB(a,b)
	#define	OSL_MEMORY_PAR_WRITE(a)									OSL_MENU_WRITE_PAR_TO_MEMORY_CB(a)
	#define	OSL_LCD_WriteData(a)										OSL_WINSTAR_WriteData_CB(a)
	#define	OSL_LCD_UPDATE_STR(a,b,c,d)							OSL_WINSTAR_UPDATE_STR_CB(a,b,c,d)
	#define	OSL_LCD_DisableDisplay()								OSL_WINSTAR_DisableDisplay_CB()

	void OSL_WINSTAR_WriteIns_CB(uint8_t data);
	void OSL_WINSTAR_SET_ADR_CB(uint8_t row, uint8_t column);
	bool OSL_MENU_WRITE_PAR_TO_MEMORY_CB(OSL_G_VAR_FLAGS_T* Size);
	void OSL_WINSTAR_WriteData_CB(uint8_t data);
	void OSL_WINSTAR_UPDATE_STR_CB(uint8_t row, uint8_t column, uint8_t* data, uint8_t sym_num);
	void OSL_WINSTAR_DisableDisplay_CB(void);
#endif

extern TIM_Calc_Params_T DISPLAY_DELAY;
extern const uint8_t Clear_Text[DISPLAY_COLUMNS];

bool OSL_LCD_StringUpdate(uint8_t row, uint8_t column, uint8_t *str, uint8_t sym_num, OSL_LCD_MODE_T MODE, uint8_t par_num);
void OSL_LCD_WriteIns_Func(uint8_t data);

#ifdef __cplusplus
}
#endif
#endif
