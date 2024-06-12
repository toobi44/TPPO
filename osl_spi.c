/* Includes ------------------------------------------------------------------*/
#include "osl_spi.h"

/* Exported function prototypes ----------------------------------------------*/
void OSL_SPI_DEVICEx_Logical(SPI_DEVICEx_Params_T *p)
{
	bool flag = false;

	if (p->hspi_x->Init.DataSize != p->Reg_Params.DataSize)
	{
		flag = 1;
		p->hspi_x->Init.DataSize = p->Reg_Params.DataSize;
		//MODIFY_REG(p->hspi_x->Instance->CR1, SPI_CR1_DFF, p->hspi_x->Init.DataSize);
	}
	if (p->hspi_x->Init.BaudRatePrescaler != p->Reg_Params.BaudRatePrescaler)
	{
		flag = 1;	
		p->hspi_x->Init.BaudRatePrescaler = p->Reg_Params.BaudRatePrescaler;
		//MODIFY_REG(p->hspi_x->Instance->CR1, SPI_CR1_BR, p->hspi_x->Init.BaudRatePrescaler);
	}
	if (p->hspi_x->Init.CLKPolarity != p->Reg_Params.CLKPolarity)
	{
		flag = 1;
		p->hspi_x->Init.CLKPolarity = p->Reg_Params.CLKPolarity;
		//MODIFY_REG(p->hspi_x->Instance->CR1, SPI_CR1_CPOL, p->hspi_x->Init.CLKPolarity);
	}
	if (p->hspi_x->Init.CLKPhase != p->Reg_Params.CLKPhase)
	{
		flag = 1;
		p->hspi_x->Init.CLKPhase = p->Reg_Params.CLKPhase;
		//MODIFY_REG(p->hspi_x->Instance->CR1, SPI_CR1_CPHA, p->hspi_x->Init.CLKPhase);
	}
	
	if (flag)
	{
		HAL_SPI_Init(p->hspi_x);
		__HAL_SPI_ENABLE(p->hspi_x);
	}
}

void OSL_SPI_Data(SPI_DEVICEx_Params_T *p)
{
	if (p->hspi_x->Init.Mode == SPI_MODE_MASTER) HAL_GPIO_WritePin(p->CS_GPIO_Port, p->CS_GPIO_Pin, p->GPIO_Active_Level);
	
	switch (p->data_mode)
	{
		case (SPI_MODE_TX):			HAL_SPI_Transmit_DMA(p->hspi_x, (uint8_t*)p->data_send, p->Num_packets);																break;
		case (SPI_MODE_RX):			HAL_SPI_Receive_DMA(p->hspi_x, (uint8_t*)p->data_get, p->Num_packets);																	break;
		case (SPI_MODE_RX_TX):	HAL_SPI_TransmitReceive_DMA(p->hspi_x, (uint8_t*)p->data_send, (uint8_t*)p->data_get, p->Num_packets);	break;	
	}
}

void OSL_SPIx_Params_INIT(SPIx_Params_T *p)
{
	p->Task_timer = 0;
	p->Busy_Flag = false;
}

void OSL_SPI_X_IT(SPIx_Params_T *p)
{
	if (!p->Busy_Flag)
	{
		if (p->Task_timer >= p->Tasks_num) p->Task_timer = 0;
		p->Operating_Task = p->Zero_task + p->Task_timer;						// Адрес текущего таска
		p->tmp_device = p->Operating_Task->SPI_DEVICEx_Params_p;

		for (p->index = 0; p->index < p->Operating_Task->Elements_num; p->index++)
		{
			// Шагаем по массиву устройств из данного таска
			if (p->tmp_device[p->index]->Enable_Flag)
			{
				// Вызываем функцию данного SPI устройства		
				p->Busy_Flag = true;
				p->Operating_SPI_Device = p->tmp_device[p->index];
				
				if (p->tmp_device[p->index]->Device_X_func_IT() == SPI_DEVICE_X_RETURN_STATE_BUSY) break;
				else p->Busy_Flag = false;
			}
			else p->Busy_Flag = false;
		}
		p->Task_timer++;
	}
}

void OSL_SPI_X_Callback(SPIx_Params_T *p)
{
	HAL_GPIO_WritePin(p->Operating_SPI_Device->CS_GPIO_Port, p->Operating_SPI_Device->CS_GPIO_Pin, (GPIO_PinState)!p->Operating_SPI_Device->GPIO_Active_Level);
	if (p->Operating_SPI_Device->Device_X_func_CB != NULL) p->Operating_SPI_Device->Device_X_func_CB();
	p->Busy_Flag = false;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
