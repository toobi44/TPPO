/* Includes ------------------------------------------------------------------*/
#include "BD_gpio_spi_lcd.h"
#include "tim.h"

__weak void OSL_DISPLAY_DISABLE_Callback(void){}

TIM_Calc_Params_T DISPLAY_DELAY = TIME_CALC_INIT_DEF;

void OSL_WINSTAR_ClearDisplay(void)
{
	OSL_WINSTAR_WriteIns(CLEAR_DISPLAY);
	OSL_TIM_Delay(&DISPLAY_DELAY, &TIME_SOURCE, 6500UL);				// 6.5ms
}

static inline void OSL_WINSTAR_DataReadWrite(void)
{
	EN(BIT_SET);
	OSL_TIM_Delay(&DISPLAY_DELAY, &TIME_SOURCE, 1UL);				//1 mks
	EN(BIT_RESET);
}

void OSL_WINSTAR_WriteData(uint8_t data)
{
	RS(BIT_SET);
	RW(BIT_RESET);
	
	data = RusTable[data];

	DB7_adr(data);		
	DB6_adr(data);	
	DB5_adr(data);
	DB4_adr(data);
	#ifdef GPIO_LCD_4BIT_MODE
	OSL_WINSTAR_DataReadWrite();
	
	DB7_adr(data<<4);		
	DB6_adr(data<<4);	
	DB5_adr(data<<4);
	DB4_adr(data<<4);	
	OSL_WINSTAR_DataReadWrite();	
	
	#else
	DB3_adr(data);	
	DB2_adr(data);	
	DB1_adr(data);	
	DB0_adr(data);

	WINSTAR_DataReadWrite();
	#endif
}

void OSL_WINSTAR_WriteIns(uint8_t data)
{
	RS(BIT_RESET);
	RW(BIT_RESET);
	
	DB7_adr(data);
	DB6_adr(data);
	DB5_adr(data);
	DB4_adr(data);
	
	#ifdef GPIO_LCD_4BIT_MODE
	OSL_WINSTAR_DataReadWrite();
	
	DB7_adr(data<<4);
	DB6_adr(data<<4);
	DB5_adr(data<<4);
	DB4_adr(data<<4);
	
	#else
	DB3_adr(data);
	DB2_adr(data);
	DB1_adr(data);
	DB0_adr(data);
	
	#endif
	OSL_WINSTAR_DataReadWrite();
}

void OSL_WINSTAR_InitDisplay(void)
{
#ifdef GPIO_LCD_4BIT_MODE
	OSL_WINSTAR_WriteIns(RETURN_HOME);														// Без этой строчки не работает
	OSL_WINSTAR_Send_Nibble(0x3);
	OSL_WINSTAR_Send_Nibble(0x3);
	OSL_WINSTAR_Send_Nibble(0x3);
	OSL_WINSTAR_Send_Nibble(0x2);
	OSL_TIM_Delay(&DISPLAY_DELAY, &TIME_SOURCE, 5000UL);				//5 ms
	OSL_WINSTAR_WriteIns(FUNCTION_SET_4BIT_MODE);
#else
	OSL_WINSTAR_WriteIns(FUNCTION_SET_8BIT_MODE);
#endif
	OSL_WINSTAR_WriteIns(DISPLAY_OFF);
	OSL_WINSTAR_WriteIns(ENTRY_MODE_SET);
	OSL_WINSTAR_WriteIns(SET_GRAPHIC);
	OSL_WINSTAR_ClearDisplay();
	OSL_WINSTAR_WriteIns(RETURN_HOME);
	OSL_WINSTAR_WriteIns(DISPLAY_ON);
}

void OSL_WINSTAR_DisableDisplay(void)
{
	RS(BIT_RESET);
	RW(BIT_RESET);
	DB7(BIT_RESET);
	DB6(BIT_RESET);
	DB5(BIT_RESET);
	DB4(BIT_RESET);
	OSL_DISPLAY_DISABLE_Callback();
}

#ifdef GPIO_LCD_4BIT_MODE
void OSL_WINSTAR_Send_Nibble(uint8_t data)
{
	RS(BIT_RESET);
	RW(BIT_RESET);

	DB7_adr(data);
	DB6_adr(data);
	DB5_adr(data);
	DB4_adr(data);
	OSL_WINSTAR_DataReadWrite();
}
#endif

void OSL_WINSTAR_SetDDRAMAddress(uint8_t data)
{
	RS(BIT_RESET);
	RW(BIT_RESET);
	
	DB7(BIT_SET);
	DB6_adr(data);
	DB5_adr(data);
	DB4_adr(data);

	#ifdef GPIO_LCD_4BIT_MODE
	OSL_WINSTAR_DataReadWrite();
	
	DB7_adr(data<<4);
	DB6_adr(data<<4);
	DB5_adr(data<<4);
	DB4_adr(data<<4);
	OSL_WINSTAR_DataReadWrite();
	
	#else
	DB3_adr(data);
	DB2_adr(data);
	DB1_adr(data);
	DB0_adr(data);

	OSL_WINSTAR_DataReadWrite();
	#endif
}

void OSL_WINSTAR_SET_ADR (uint8_t row, uint8_t column)
{
	uint8_t str;

	str = column + 0x80;
	if(row == 1) str+= 0x40;
	
	OSL_WINSTAR_SetDDRAMAddress(str);
}

void OSL_WINSTAR_UPDATE_STR(uint8_t row, uint8_t column, uint8_t* data, uint8_t sym_num)
{
	uint8_t i;
	
	OSL_LCD_SetAddr(row,column);
	for (i = 0; i < sym_num; i++) OSL_WINSTAR_WriteData(data[i]);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
