#ifndef __lcd_winstar_1602_H_dim
#define __lcd_winstar_1602_H_dim
#ifdef __cplusplus
 extern "C" {
#endif

#include "osl_user_config.h"	 
#include "osl_general.h"

#define DISPLAY_STRINGS		2

#define SPACE				32
	 
#define	FUNCTION_SET_8BIT_MODE	0x3A			// Set Russian symbols, 8 bit
#define	FUNCTION_SET_4BIT_MODE	0x2A			// Set Russian symbols, 4 bit
#define	DISPLAY_OFF							0x8				// Display off
#define	ENTRY_MODE_SET					0x6				// Entry mode set increment cursor by 1 not shifting display
#define	SET_GRAPHIC							0x17			// SetGraphic
#define	CLEAR_DISPLAY						0x01			// ClearDisplay
#define	RETURN_HOME							0x02			// ReturnHome
#define	DISPLAY_ON							0x0C			// DisplayOn
#define	NIBBLE_3								0x03
#define	NIBBLE_2								0x02
#define	BLINK_EN								0xD       // Blinking enable
#define	BLINK_CURSOR_DIS				0xC       // Blinking and cursor disable
#define	BLINK_CURSOR_EN					0xF       // Blinking and cursor enable
#define	CURSOR_EN								0xE       // Cursor enable

#if defined(GPIO_LCD_MODE)

	#include "gpio.h"

	#define RS(a) HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, a)
	#ifdef RW_EXISTS
		#define RW(a) HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin, a)
	#else
		#define RW(a) while(0)
	#endif
	#define EN(a) HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin, a)

	#define DB7(a) HAL_GPIO_WritePin(LCD_DB7_GPIO_Port, LCD_DB7_Pin, a)
	#define DB6(a) HAL_GPIO_WritePin(LCD_DB6_GPIO_Port, LCD_DB6_Pin, a)
	#define DB5(a) HAL_GPIO_WritePin(LCD_DB5_GPIO_Port, LCD_DB5_Pin, a)
	#define DB4(a) HAL_GPIO_WritePin(LCD_DB4_GPIO_Port, LCD_DB4_Pin, a)

	#ifndef GPIO_LCD_4BIT_MODE
		#define DB3(a) HAL_GPIO_WritePin(LCD_DB3_GPIO_Port,	LCD_DB3_Pin, a)
		#define DB2(a) HAL_GPIO_WritePin(LCD_DB2_GPIO_Port, LCD_DB2_Pin, a)
		#define DB1(a) HAL_GPIO_WritePin(LCD_DB1_GPIO_Port, LCD_DB1_Pin, a)
		#define DB0(a) HAL_GPIO_WritePin(LCD_DB0_GPIO_Port, LCD_DB0_Pin, a)
	#endif

	#define DB7_adr(a) HAL_GPIO_WritePin(LCD_DB7_GPIO_Port, LCD_DB7_Pin, (GPIO_PinState)((char)BIT_7_MASK & (a)))
	#define DB6_adr(a) HAL_GPIO_WritePin(LCD_DB6_GPIO_Port, LCD_DB6_Pin, (GPIO_PinState)((char)BIT_6_MASK & (a)))
	#define DB5_adr(a) HAL_GPIO_WritePin(LCD_DB5_GPIO_Port, LCD_DB5_Pin, (GPIO_PinState)((char)BIT_5_MASK & (a)))
	#define DB4_adr(a) HAL_GPIO_WritePin(LCD_DB4_GPIO_Port, LCD_DB4_Pin, (GPIO_PinState)((char)BIT_4_MASK & (a)))

	#ifndef GPIO_LCD_4BIT_MODE
		#define DB3_adr(a) HAL_GPIO_WritePin(LCD_DB3_GPIO_Port, LCD_DB3_Pin, (GPIO_PinState)((char)BIT_3_MASK & (a)))
		#define DB2_adr(a) HAL_GPIO_WritePin(LCD_DB2_GPIO_Port, LCD_DB2_Pin, (GPIO_PinState)((char)BIT_2_MASK & (a)))
		#define DB1_adr(a) HAL_GPIO_WritePin(LCD_DB1_GPIO_Port, LCD_DB1_Pin, (GPIO_PinState)((char)BIT_1_MASK & (a)))
		#define DB0_adr(a) HAL_GPIO_WritePin(LCD_DB0_GPIO_Port, LCD_DB0_Pin, (GPIO_PinState)((char)BIT_0_MASK & (a)))
	#endif

	#define BIT_SET		GPIO_PIN_SET
	#define BIT_RESET	GPIO_PIN_RESET

#elif defined(SPI_LCD_MODE)

	#define BIT_SET				(bool)1
	#define BIT_RESET			(bool)0		
		
	typedef union _CTRL_BITS_T{
		uint8_t all;
		struct {
			bool rsvd:6;
			bool RW_BIT:1;
			bool RS_BIT:1;
		} bit;
	} CTRL_BITS_T;

	typedef union _DB_BITS_T{
		uint8_t all;
		struct {
			bool DB0_BIT:1;
			bool DB1_BIT:1;
			bool DB2_BIT:1;
			bool DB3_BIT:1;
			bool DB4_BIT:1;
			bool DB5_BIT:1;
			bool DB6_BIT:1;
			bool DB7_BIT:1;
		} bit;
	} DB_BITS_T;	
		
	CTRL_BITS_T	CTRL_BITS;
	DB_BITS_T		DB_BITS;

	#define RS(a) (CTRL_BITS.bit.RS_BIT = a)
	#define RW(a) (CTRL_BITS.bit.RW_BIT = a)

	#define DB7(a) (DB_BITS.bit.DB7_BIT = a)
	#define DB6(a) (DB_BITS.bit.DB6_BIT = a)
	#define DB5(a) (DB_BITS.bit.DB5_BIT = a)
	#define DB4(a) (DB_BITS.bit.DB4_BIT = a)
	#define DB3(a) (DB_BITS.bit.DB3_BIT = a)
	#define DB2(a) (DB_BITS.bit.DB2_BIT = a)
	#define DB1(a) (DB_BITS.bit.DB1_BIT = a)
	#define DB0(a) (DB_BITS.bit.DB0_BIT = a)

	#define DB7_adr(a) (DB_BITS.bit.DB7_BIT = (bool)((DB7_MASK & (a)) >>7))
	#define DB6_adr(a) (DB_BITS.bit.DB6_BIT = (bool)((DB6_MASK & (a)) >>6))
	#define DB5_adr(a) (DB_BITS.bit.DB5_BIT = (bool)((DB5_MASK & (a)) >>5))
	#define DB4_adr(a) (DB_BITS.bit.DB4_BIT = (bool)((DB4_MASK & (a)) >>4))	
	#define DB3_adr(a) (DB_BITS.bit.DB3_BIT = (bool)((DB3_MASK & (a)) >>3))
	#define DB2_adr(a) (DB_BITS.bit.DB2_BIT = (bool)((DB2_MASK & (a)) >>2))
	#define DB1_adr(a) (DB_BITS.bit.DB1_BIT = (bool)((DB1_MASK & (a)) >>1))
	#define DB0_adr(a) (DB_BITS.bit.DB0_BIT = (bool)(DB0_MASK & (a) ))

#endif
	
// Russian simbols
static const char RusTable[256] = 
{
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
	0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
	0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,
	0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
	0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
	0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
	0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA2,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
	0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB5,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
	0x41,0xA0,0x42,0xA1,0xE0,0x45,0xA3,0xA4,0xA5,0xA6,0x4B,0xA7,0x4D,0x48,0x4F,0xA8,
	0x50,0x43,0x54,0xA9,0xAA,0x58,0xE1,0xAB,0xAC,0xE2,0xAD,0xAE,0x62,0xAF,0xB0,0xB1,
	0x61,0xB2,0xB3,0xB4,0xE3,0x65,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0x6F,0xBE,
	0x70,0x63,0xBF,0x79,0xE4,0x78,0xE5,0xC0,0xC1,0xE6,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7
};	

void OSL_WINSTAR_ClearDisplay(void);
void OSL_WINSTAR_WriteData(uint8_t data);
void OSL_WINSTAR_WriteIns(uint8_t data);
void OSL_WINSTAR_InitDisplay(void);
void OSL_WINSTAR_Send_Nibble(uint8_t data);
static inline void OSL_WINSTAR_DataReadWrite(void);
void OSL_WINSTAR_SetDDRAMAddress(uint8_t data);
void OSL_WINSTAR_SET_ADR (uint8_t row, uint8_t column);
void OSL_WINSTAR_StringUpdate(uint8_t row, uint8_t column, uint8_t *str, uint8_t sym_num, OSL_LCD_MODE_T MODE, uint8_t par_num);
void OSL_WINSTAR_DisableDisplay(void);
void OSL_WINSTAR_UPDATE_STR(uint8_t row, uint8_t column, uint8_t* data, uint8_t sym_num);

#ifdef __cplusplus
}
#endif
#endif
