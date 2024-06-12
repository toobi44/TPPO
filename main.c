/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Display.h"
//#include "ModBus.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t Tx[1];
uint8_t Rx[5];
uint16_t reg1031 = 1521;
uint16_t REGS[5000];
extern uint8_t pack_base_lenght;
uint8_t data[][16] = {
{"                "},
{"Влево           "},
{"Вверх           "},
{"Вправо          "},
{"Вниз            "},
{"Пуск            "},
{"Ввод            "},
{"МПУ             "},
{"Реверс          "},
{"Стоп            "},
};
    uint32_t data1 = 100;
    uint8_t num = 3;
    //uint8_t state1;
    //uint8_t state2;
    //uint8_t state3;
    
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void COUNT(uint32_t count);
void CNT_START(uint16_t *CNT_prev);
void CNT_STOP(uint16_t *CNT_prev, uint16_t *result);
void LED_INIT();

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_SPI1_Init();
  MX_USB_DEVICE_Init();
  MX_RTC_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  //HAL_TIM_Base_Start_IT(&htim2);
  LCD_PWR_on();
  //HAL_TIM_Base_Start_IT(&htim1);
  USART_PULT_RS485_Init(&USART_PULT_RS485);
  USER_TIME_SOURCE_FAST_Logical_Init(&TIME_SOURCE);
  OSL_TIM_Start_Delay(&DISPLAY_POWER_DELAY, &TIME_SOURCE);
  USER_TIME_SOURCE_SLOW_Logical_Init(&TIME_SOURCE_SLOW);
  USER_MDB_INT_TIMER_Logical_Init(&MDB_INT_TIM);
  Modbus_PULT_RS485_Slave_Init(&RS485_Modbus, &USART_PULT_RS485);
  USER_SPI_MASTER_Init(&SPI_Params);
  
  
  DIG_INP_Init();
	
  OSL_TIM_Stop_Delay(&DISPLAY_POWER_DELAY, &TIME_SOURCE, 50000UL);      //50ms
	OSL_WINSTAR_InitDisplay();
  
  
  //LCD_INIT();
  //светодиойды
  //LED_INIT();
  //REGS[1031] = reg1031;
  
  //HAL_UART_Receive_DMA(&huart2, Rx_buffer, 200);
  pack_base_lenght = 200;
  
  
  OSL_MDB_Frame_end_Timeout(&RS485_Modbus, &TIME_SOURCE);
  
  
  //COUNT(data1);
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    OSL_TIM_Time_Calc (&TIME_CALC_TEST, &TIME_SOURCE, TIME_CALC_ACTIVE);
    OSL_TIM_Logical(&MDB_INT_TIM);
    
    OSL_TIM_Time_Calc (&TIME_CALC_TEST, &TIME_SOURCE, VIEW_RESULT);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_2)
  {
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_LSI_Enable();

   /* Wait till LSI is ready */
  while(LL_RCC_LSI_IsReady() != 1)
  {

  }
  LL_PWR_EnableBkUpAccess();
  if(LL_RCC_GetRTCClockSource() != LL_RCC_RTC_CLKSOURCE_LSI)
  {
    LL_RCC_ForceBackupDomainReset();
    LL_RCC_ReleaseBackupDomainReset();
    LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSI);
  }
  LL_RCC_EnableRTC();
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_6);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_SetSystemCoreClock(72000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
  LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_PLL_DIV_1_5);
}

/* USER CODE BEGIN 4 */
void COUNT(uint32_t count)
{
  for (uint32_t i = 0; i < count; i++)
  {
    OSL_UINT_TO_STR((uint8_t*)data[0], i, 3);
    OSL_WINSTAR_UPDATE_STR(0, 0, (uint8_t*)data[0], 3);
  }
}
void CNT_START(uint16_t *CNT_prev)
{
  *CNT_prev = (uint16_t)TIM3->CNT;
}
void CNT_STOP(uint16_t *CNT_prev, uint16_t *result)
{
  *result = TIM3->CNT - *CNT_prev;
}
void LED_INIT()
{
  Tx[0] = 2;
  HAL_GPIO_WritePin(CS_LED_GPIO_Port, CS_LED_Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive_DMA(&hspi1, Tx, Rx, 1);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
