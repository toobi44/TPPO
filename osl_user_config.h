#ifndef __g_main_USER_H_dim
#define __g_main_USER_H_dim
#ifdef __cplusplus
 extern "C" {
#endif

/******************/
//STM32 FAMILY{
	 
#define F1XX
//#define F2XX
//#define F3XX
//#define F4XX
//#define F7XX
	 
//STM32 FAMILY}		 
/******************/

/******************/
//RTOS{	 
//#define KERNEL_USED
//RTOS}
/******************/
	 
/******************/
//TIM
//#define DAC_SIN

// Base params:
#define		Freq1				2.0F
#define		Points1			840.0F	 
#define		Presc1			2000.0F

#define		Freq2				2000.0F
#define		Points2			210.0F	
#define		Presc2			2.0F
/******************/
	 
/******************/
//DISPLAY{
/****/
//TYPE{
#define WEH1602
//#define	SEGMENT_BUFFER_SPI
//TYPE}
 
#define DISPLAY_COLUMNS	16
/****/

/****/
//CONFIG{
#if defined(WEH1602)
	//GPIO/SPI:
	#define GPIO_LCD_MODE
	//#define SPI_LCD_MODE
	
	#if defined (GPIO_LCD_MODE)
		//BIT MODE:
		#define GPIO_LCD_4BIT_MODE
	#elif defined(SPI_LCD_MODE)
		//RW MODE:
		//#define RW_EXISTS
	#endif	
#elif defined(SEGMENT_BUFFER_SPI)

#endif
//CONFIG}
/****/

//DISPLAY}
/******************/

/******************/
//ADC{
#define FLOAT_ADC
//#define Q16_ADC

//#define IQ_MATH

//#define ADC_CHANNELS_NUM		4				//2 Тока, U_dc, Температура MCU
//ADC}
/******************/

/******************/
//ONE_WIRE{
//#define	MAX_DEVICES					0x01
//ONE_WIRE}
/******************/

/******************/
//g_RAM{

//MODBUS{
#define	MODBUS_USED
#if defined (MODBUS_USED)

	#define	MODBUS_SLAVE_USED			// Активация Ведомого режима
	//#define	MODBUS_MASTER_USED		// Активация Ведущего режима

	#define MAX_RX_DATA_SIZE 80		// Размера данных на чтение (2 байта)
	#define MAX_TX_DATA_SIZE 80		// Размера данных на запись (2 байта)	 
	 
	#if defined (MODBUS_MASTER_USED)	 
		#define MODBUS_MASTER_PAR_NUM			47		// Число считываемых регистров
	#endif

	//Modbus Interface{
	//#define USB_MODBUS						// У USB всегда буфер задается отдельно
	//#define BT_MODBUS
	#if defined (BT_MODBUS)
		#define USART_BT_RX_BUFF 200
		#define USART_BT_TX_BUFF 200
		#if MAX_RX_DATA_SIZE*2 > (USART_BT_RX_BUFF - 10)			// Проверяем, есть ли в буфере UART место под MAX_RX_DATA_SIZE при записи; 10 - число байт в посылке без данных на запись
			#error "osl_user_config.h" Превышен размер данных на\
			запись для данного массива USART
		#endif
		
		#if MAX_TX_DATA_SIZE*2 > (USART_BT_TX_BUFF - 10)			// Проверяем, есть ли в буфере UART место под MAX_RX_DATA_SIZE при чтении; 10 - число байт в посылке без данных на чтение
			#error "osl_user_config.h" Превышен размер данных на\
			чтение для данного массива USART
		#endif
	#endif	

	#define RS485_MODBUS
	#if defined (RS485_MODBUS)
		#define USART_RS485_RX_BUFF 200
		#define USART_RS485_TX_BUFF 200

		#if MAX_RX_DATA_SIZE*2 > (USART_RS485_RX_BUFF - 10)			// Проверяем, есть ли в буфере UART место под MAX_RX_DATA_SIZE при записи; 10 - число байт в посылке без данных на запись
			#error "osl_user_config.h" Превышен размер данных на\
			запись для данного массива USART
		#endif
		
		#if MAX_TX_DATA_SIZE*2 > (USART_RS485_TX_BUFF - 10)			// Проверяем, есть ли в буфере UART место под MAX_RX_DATA_SIZE при чтении; 10 - число байт в посылке без данных на чтение
			#error "osl_user_config.h" Превышен размер данных на\
			чтение для данного массива USART
		#endif
	#endif
	//Modbus Interface}

	#if defined (MODBUS_MASTER_USED)
		#define USART_RS485_MASTER_RX_BUFF 200
		#define USART_RS485_MASTER_TX_BUFF 200

		#if MAX_RX_DATA_SIZE*2 > (USART_RS485_MASTER_RX_BUFF - 10)			// Проверяем, есть ли в буфере UART место под MAX_RX_DATA_SIZE при записи; 10 - число байт в посылке без данных на запись
			#error "osl_user_config.h" Превышен размер данных на\
			запись для данного массива USART
		#endif
		
		#if MAX_TX_DATA_SIZE*2 > (USART_RS485_MASTER_TX_BUFF - 10)			// Проверяем, есть ли в буфере UART место под MAX_RX_DATA_SIZE при чтении; 10 - число байт в посылке без данных на чтение
			#error "osl_user_config.h" Превышен размер данных на\
			чтение для данного массива USART
		#endif
	#endif
	//Modbus Interface}			

	//Modbus Mode{
	//#define MODBUS_GRAPHIC
	
	#if defined (MODBUS_GRAPHIC)
		#define MAX_GRAPH_BUFFER 1024
	#endif
	
	//Modbus Mode}
#endif
//MODBUS}

//MENU{
//#define	MENU_USED
#if defined (MENU_USED)
	#define MAX_LEVEL		3			// Степень вложенности меню
	//#define MENU_RESET
#endif
//MENU}

//SPEED{
//#define CAP_DATA_BUFFER 20		// Число точек при считывании CAP		
//#define CAP_DATA_32_BITS	
//SPEED}

//MEMORY{
#define	MEMORY_USED

#if defined (MEMORY_USED)

//PARAMS SET{

	//#define	FLASH_USED

	#if !defined (FLASH_USED)
		#define	EXT_EEPROM_USED
		#define	MEMORY_PARAMS									// Активизация параметров
	#endif

	//FLASH{
	#if defined (FLASH_USED)
		//PARAMS SET{
		#define ADDR_FLASH_PAGE_0								((uint32_t)0x08000000)
		#if defined (F1XX)
			#define PARAMS_START_PAGE							0x37				// 55
			#define PARAMS_START_ADDRESS					((uint32_t)(ADDR_FLASH_PAGE_0 + (FLASH_PAGE_SIZE*PARAMS_START_PAGE)))
			#define PARAMS_END_ADDRESS						((uint32_t)(PARAMS_START_ADDRESS + FLASH_PAGE_SIZE-4)))
		#elif defined(F4XX)
			#define PARAMS_SECTOR_NUM							FLASH_SECTOR_11
			#define FLASH_SECTOR_SIZE							0x20000			//128 KBytes
			#define PARAMS_START_ADDRESS					((uint32_t)0x080E0000)
			#define PARAMS_END_ADDRESS						((uint32_t)(PARAMS_START_ADDRESS + FLASH_SECTOR_SIZE-4)))
		#endif
		//PARAMS SET}
	//FLASH}	
	#elif defined(EXT_EEPROM_USED)
		//EXT_EEPROM{
		#define	EXT_EEPROM_AT25640B
		#define	EEPROM_WRITING_BUF_SIZE						30							
		#define	EEPROM_STATUS_READ_LIMIT	        20			// Предел запросов статуса перед выставлением флага сбой EEPROM
		#define	EEPROM_PAR_READ_LIMIT      	      5
		#define	EEPROM_PAR_WRITE_LIMIT   	        5
		//EXT_EEPROM}
	#endif
	
//PARAMS SET}
	
//JOURNAL SET{

	#if !defined (FLASH_USED)
		#define	EXT_FLASH_USED

		#define	EX_FLASH_STATUS_READEN_TIMER_LIMIT	20							// Предел запросов статуса перед выставлением флага сбой EX_FLASH
		#define	EX_FLASH_JOURNAL_BUFF_SIZE					10							// Размер буфера для записи
		#define	EX_FLASH_JOURNAL_PAR_NUM						3								// Число параметров в журнале
		#define	EXT_FLASH_SECTOR_BUFF								2								// Количество рабочих секторов
				
				
		//#define	EX_FLASH_START_SECTOR_ADDR					0x0							// Начальный адрес журнала
		//#define	EX_FLASH_STOP_SECTOR_ADDR						0x00FFFF				// Конечный адрес журнала
				
	#endif	

	#if defined (FLASH_USED)
		#define	JOURNAL_LOCAL
		
		#if defined (JOURNAL_LOCAL)
			#define	JOURNAL_USED
			
			#define JOURNAL_PAGES									2

			#if defined (F1XX)
				
				#define JOURNAL_PAGE_SIZE							FLASH_PAGE_SIZE
				#define JOURNAL_START_PAGE						0x38	// 56
				#define JOURNAL_START_ADDRESS					((uint32_t)(ADDR_FLASH_PAGE_0 + (JOURNAL_PAGE_SIZE*JOURNAL_START_PAGE)))
				#define JOURNAL_END_ADDRESS						((uint32_t)(JOURNAL_START_ADDRESS + (JOURNAL_PAGE_SIZE*JOURNAL_PAGES)-4))
			#elif defined(F4XX)
				#define JOURNAL_PAGE_SIZE							0x20000										// 128 KBytes				
				#define JOURNAL_START_ADDRESS					((uint32_t)0x080C0000)		// FLASH_SECTOR_10
				#define JOURNAL_END_ADDRESS						((uint32_t)(JOURNAL_START_ADDRESS + (JOURNAL_PAGE_SIZE*JOURNAL_PAGES)-4))
			#endif			
		#endif
	#elif defined(EXT_FLASH_USED)
		#define	JOURNAL_USED

		#define	EXT_FLASH_AT25DF081A
		
		#define JOURNAL_SECTORS										2
		
		#define JOURNAL_PROTECTION_SECTORS				16
		#define JOURNAL_MAX_PROTECTION_SECTORS		16
		
		#define JOURNAL_MAX_PARAMS								12

		#define JOURNAL_MANUFACTURER_ID						AT25DF081A_MANUFACTURER_ID
		#define JOURNAL_DEVICE_ID_PART_1					AT25DF081A_DEVICE_ID_PART_1
		#define JOURNAL_DEVICE_ID_PART_2					AT25DF081A_DEVICE_ID_PART_2
		
		#define JOURNAL_PROGR_PAGE_SIZE						0x100											// 256 Bytes
		#define JOURNAL_ERASE_SECTOR_SIZE					0x1000										// 4 KBytes	
		#define JOURNAL_START_ADDRESS							((uint32_t)0x00)					// FLASH_SECTOR_0
		#define JOURNAL_END_ADDRESS								((uint32_t)(JOURNAL_START_ADDRESS + (JOURNAL_ERASE_SECTOR_SIZE*JOURNAL_SECTORS)-4))
				
		#define EXT_FLASH_ERASE_NUM 											30
		#define EXT_FLASH_STATUS_READEN_TIMER_LIMIT				150
	#endif
//JOURNAL SET}
	
#endif
//MEMORY}

//g_RAM}
/******************/

#ifdef __cplusplus
}
#endif
#endif
