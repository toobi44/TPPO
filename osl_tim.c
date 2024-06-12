/* Includes ------------------------------------------------------------------*/
#include "osl_tim.h"

#ifdef DAC_SIN
	#include "BD_dac.h"
#endif

void BD_TIM_ReInit(TIMx_Params_T *p)
{	
  LL_TIM_SetPrescaler(p->TIMx, p->TIMx_Reg_Params.Prescaler);
  LL_TIM_SetAutoReload(p->TIMx, p->TIMx_Reg_Params.Period);
  
  
	//p->htim_x->Init.Prescaler = p->TIMx_Reg_Params.Prescaler;
	//p->htim_x->Init.Period = p->TIMx_Reg_Params.Period;
	//HAL_TIM_Base_Init(p->htim_x);
}
/*
void BD_PWM_ReInit(TIMx_PWM_Params_T *p)
{	
	p->htim_x->Init.Prescaler = p->TIMx_Reg_Params.Prescaler;
	p->htim_x->Init.Period = p->TIMx_Reg_Params.Period;
	HAL_TIM_Base_Init(p->htim_x);
}
*/
/********************************************/
/**
  * @brief  Функция применения изменений параметров таймера
  * @param  p: Указатель на структуру типа TIMx_Params_T
  * @retval void
		Вызывайте ее после:
	- Инициализации переменный типа TIMx_Params_T
	- Включения/выключении таймера командой TIMx_en
	- Изменении параметров переменной TIMx_Reg_Params
		Комментарии:
	Если не требуется срочная отработка изменений, фунцию можно закинуть
	в бесконечный while файла main.c
  */

void OSL_TIM_Logical(TIMx_Params_T *p)
{
	if (!p->TIMx_init_was)
	{
		//p->TIMx_Init_func();
		if (!p->Settings.bit.Hand_Calc_Params) BD_TIMx_Params_Calc(&p->TIMx_Reg_Params, p->APBx_Freq);		
		BD_TIM_ReInit(p);
		p->TIMx_Reg_Params_prev = p->TIMx_Reg_Params;		
		//p->htim_x->Instance->CNT = 0UL;
    p->TIMx->CNT = 0UL;
		BD_TIM_Init_Callback(p);
		p->TIMx_init_was = true;
	}
	else if ( memcmp(&p->TIMx_Reg_Params, &p->TIMx_Reg_Params_prev, sizeof(p->TIMx_Reg_Params)) )
	{	
		if (!p->Settings.bit.Hand_Calc_Params) BD_TIMx_Params_Calc(&p->TIMx_Reg_Params, p->APBx_Freq);
		BD_TIM_ReInit(p);
		p->TIMx_Reg_Params_prev = p->TIMx_Reg_Params;
		BD_TIM_ReInit_Callback(p);
	}

	if (p->TIMx_en && !p->TIMx_en_was)
	{    
    LL_TIM_ClearFlag_UPDATE(p->TIMx);
      
    if (p->Settings.bit.Int_Allowed) LL_TIM_EnableIT_UPDATE(p->TIMx);
		LL_TIM_EnableCounter(p->TIMx);
    
		//__HAL_TIM_CLEAR_FLAG(p->htim_x, TIM_FLAG_UPDATE);
    //
		//if (p->Settings.bit.Int_Allowed) HAL_TIM_Base_Start_IT(p->htim_x);
		//else HAL_TIM_Base_Start(p->htim_x);
		if (p->Settings.bit.One_pulse) p->TIMx_en = false;
		else
		{
			p->TIMx_en_was = true;					// Запрещаем включение таймера
			p->TIMx_dis_was = false;				// Разрешаем отключение таймера
		}
	}
	else if (!p->TIMx_en && !p->TIMx_dis_was)
	{
		if (p->Settings.bit.Int_Allowed) LL_TIM_DisableIT_UPDATE(p->TIMx);
		LL_TIM_DisableCounter(p->TIMx);
		p->TIMx->CNT = 0UL;    
   
		//if (p->Settings.bit.Int_Allowed) HAL_TIM_Base_Stop_IT(p->htim_x);
		//else HAL_TIM_Base_Stop(p->htim_x);
		//p->htim_x->Instance->CNT = 0UL;

		p->TIMx_en_was = false;
		p->TIMx_dis_was = true;
	}
}


/******************************************************************/
/*
// PWM
__weak void BD_TIM_PWM_EN (TIMx_PWM_Params_T *p){UNUSED(p);}
__weak void BD_TIM_PWM_DIS(TIMx_PWM_Params_T *p){UNUSED(p);}
__weak void BD_TIMx_PWM_Init_Callback(TIMx_PWM_Params_T *p) {UNUSED(p);}
__weak void BD_TIMx_PWM_ReInit_Callback(TIMx_PWM_Params_T *p) {UNUSED(p);}
	
void BD_TIMx_PWM_Logical(TIMx_PWM_Params_T *p)
{
	if (!p->TIMx_init_was)
	{
		//p->TIMx_Init_func();
		OSL_TIMx_PWM_Params_Calc(p);
		BD_PWM_ReInit(p);
		p->TIMx_Reg_Params_prev = p->TIMx_Reg_Params;
		
		BD_TIMx_PWM_Init_Callback(p);	
		p->TIMx_init_was = true;
	}
	else if (memcmp(&p->TIMx_Reg_Params, &p->TIMx_Reg_Params_prev, sizeof(p->TIMx_Reg_Params)) != 0)
	{
		if (p->TIMx_en_was) BD_TIM_PWM_DIS(p);					// Вырубаем ШИМ, чтобы в момент переделывания ничего не бахнуло		
		if (p->TIMx_Reg_Params.Freq_PWM != p->TIMx_Reg_Params_prev.Freq_PWM)
		{
			OSL_TIMx_PWM_Params_Calc(p);
			BD_PWM_ReInit(p);
		}
		p->TIMx_Reg_Params_prev = p->TIMx_Reg_Params;
		BD_TIMx_PWM_ReInit_Callback(p);		
		if (p->TIMx_en_was) BD_TIM_PWM_EN(p);					// Если до этого ехали, едем дальше
	}
	
	if (p->TIMx_en && !p->TIMx_en_was)
	{	
		if (p->Settings.bit.Int_Allowed) __HAL_TIM_ENABLE_IT(p->htim_x, TIM_IT_UPDATE); // Включить прерывание UPDATE
		else __HAL_TIM_ENABLE(p->htim_x);

		BD_TIM_PWM_EN(p);
		if (p->Settings.bit.One_pulse) p->TIMx_en = false;
		else
		{
			p->TIMx_en_was = true;					// Запрещаем включение таймера
			p->TIMx_dis_was = false;				// Разрешаем отключение таймера
		}		
	}
	else if (!p->TIMx_en && !p->TIMx_dis_was)
	{
		BD_TIM_PWM_DIS(p);

		if (p->Settings.bit.Int_Allowed) __HAL_TIM_DISABLE_IT(p->htim_x, TIM_IT_UPDATE); // Включить прерывание UPDATE
		else __HAL_TIM_DISABLE(p->htim_x);

		p->htim_x->Instance->CNT = 0UL;
		p->TIMx_en_was = false;
		p->TIMx_dis_was = true;
	}
}

uint8_t OSL_PWM_CHx_Logical(PWM_CHx_Params_T *p)
{
	if (!p->TIMx_init_was)
	{
		__HAL_TIM_SET_COMPARE(p->htim_x, p->Channel_x, p->CCR_x);
		p->TIMx_init_was = true;
	}
	
	if ( p->CCR_x != __HAL_TIM_GET_COMPARE(p->htim_x, p->Channel_x) ) __HAL_TIM_SET_COMPARE(p->htim_x, p->Channel_x, p->CCR_x);

	if (p->TIMx_en && !p->TIMx_en_was)
	{
		p->TIMx_en_was = true;
		p->TIMx_dis_was = false;
		if (p->Mode.bit.DIRECT_CH)
		{
			if (p->Mode.bit.Int_Allowed) 
			{
				if (HAL_TIM_PWM_Start_IT(p->htim_x, p->Channel_x)) return 1;
			}
			else HAL_TIM_PWM_Start(p->htim_x, p->Channel_x);
		}
		if (p->Mode.bit.INVERSE_CH)
		{
			if (p->Mode.bit.Int_Allowed)
			{
				if (HAL_TIMEx_OCN_Start_IT(p->htim_x, p->Channel_x))  return 1;
			}
			else HAL_TIMEx_OCN_Start(p->htim_x, p->Channel_x);
		}
	}
	else if (!p->TIMx_en && !p->TIMx_dis_was)
	{
		p->TIMx_en_was = false;
		p->TIMx_dis_was = true;		
		if (p->Mode.bit.DIRECT_CH)
		{
			if (p->Mode.bit.Int_Allowed) HAL_TIM_PWM_Stop_IT(p->htim_x, p->Channel_x); 
			else HAL_TIM_PWM_Stop(p->htim_x, p->Channel_x);
		}
		if (p->Mode.bit.INVERSE_CH)
		{
			if (p->Mode.bit.Int_Allowed) HAL_TIMEx_OCN_Stop_IT(p->htim_x, p->Channel_x);
			else HAL_TIMEx_OCN_Stop(p->htim_x, p->Channel_x);
		}
	}
	return 0;
}

void OSL_TIMx_PWM_Params_Calc(TIMx_PWM_Params_T *p)
{
	uint16_t 	test_Prescaler;
	float			test_Freq_rez, Freq_PWM_during;	
	
	if (!p->Settings.bit.Symmetric_PWM) Freq_PWM_during = p->TIMx_Reg_Params.Freq_PWM;
	else Freq_PWM_during = p->TIMx_Reg_Params.Freq_PWM * 2.0F;
	
	for (uint16_t test_Period = 65535; test_Period > 0; test_Period--)
	{
		test_Prescaler = p->APBx_Freq/((uint32_t)Freq_PWM_during*test_Period);
		test_Freq_rez = (double)p->APBx_Freq/((uint32_t)test_Prescaler*test_Period);
		
		if ( (fabs(test_Freq_rez-Freq_PWM_during)*100.0F)/Freq_PWM_during < 1.0F )
		{
			p->TIMx_Reg_Params.Prescaler = test_Prescaler-1;
			p->TIMx_Reg_Params.Period = test_Period-1;
			return;	
		}
	}
}
*/
/******************************************************************/

__weak void BD_TIM_Init_Callback(TIMx_Params_T *p)
{
	//UNUSED(p);
}

__weak void BD_TIM_ReInit_Callback(TIMx_Params_T *p)
{
	//UNUSED(p);
}

void BD_TIM_Logical_params_init (TIMx_Params_T *p) // Init one time for each TIMx
{
	p->TIMx_init_was					=				false;	
	p->TIMx_en_was						=				false;
	p->TIMx_dis_was						=				true;
	p->TIMx_function_break		=				0;
}
/*
void BD_TIMx_PWM_Logical_params_init (TIMx_PWM_Params_T *p)
{
	p->TIMx_init_was					=				false;	
	p->TIMx_en_was						=				false;
	p->TIMx_dis_was						=				true;
}

void BD_PWM_CHx_Logical_params_init (PWM_CHx_Params_T *p)
{
	p->TIMx_init_was					=				false;	
	p->TIMx_en_was						=				false;
	p->TIMx_dis_was						=				true;
	p->Point_timer						=				0;
}
*/
void BD_TIMx_Params_Calc (TIMx_Reg_Params_T *p, uint32_t APBx_Freq)
{
	float log_var1;
	float log_var2;
	float log_var3;
	float log_var4;
	float log_var5;
	
	log_var3 = log10(Freq2) - log10(Freq1);
	log_var1 = log10(p->Freq) - log10(Freq1);
	log_var2 = log10(Presc2) - log10(Presc1);
	log_var4 = log10(Presc1);
	log_var5 =(log_var1*log_var2)/log_var3+log_var4;
	
	log_var1 = powf(10.0F,log_var5);
	p->Prescaler = (uint16_t)(ceil(log_var1)) - 1;	
	p->Period = (uint16_t)(APBx_Freq/(p->Freq*(float)(p->Prescaler+1))) - 1;
}

void OSL_TIM_Time_Calc (TIM_Calc_Params_T *p, TIMx_Params_T *v, bool Command)
{
	if (Command == VIEW_RESULT && p->State == TIME_CALC_ACTIVE)
	{
		p->tmp_Init_CNT_value = p->Init_CNT_value;
    
    
    if (v->Settings.bit._32_bit_timer) p->Result_time_us = v->TIMx->CNT - p->tmp_Init_CNT_value;
		else p->Result_time_us = (uint16_t)(v->TIMx->CNT - p->tmp_Init_CNT_value);
    
    
		//if (v->Settings.bit._32_bit_timer) p->Result_time_us = v->htim_x->Instance->CNT - p->tmp_Init_CNT_value;
		//else p->Result_time_us = (uint16_t)(v->htim_x->Instance->CNT - p->tmp_Init_CNT_value);
		if (p->Result_time_us > p->Max_time_us) p->Max_time_us = p->Result_time_us;
		if (p->Result_time_us < p->Min_time_us) p->Min_time_us = p->Result_time_us;
	}
	else if (Command == TIME_CALC_ACTIVE)
	{
    p->Init_CNT_value = v->TIMx->CNT;
		//p->Init_CNT_value = v->htim_x->Instance->CNT;
		p->State = TIME_CALC_ACTIVE;
	}
}

void OSL_TIM_Time_Calc_Reset(TIM_Calc_Params_T *p)
{
	p->Result_time_us = 0;
	p->State = WAITING_INIT_TIME;
}

void OSL_TIM_Freq_Calc(Freq_Calc_Params_T *p, TIMx_Params_T *v)
{
	if (p->State == TIME_CALC_ACTIVE)
	{
    p->Result_freq = p->Freq_Const*(v->TIMx->CNT - p->Init_CNT_value);
		//p->Result_freq = p->Freq_Const*(v->htim_x->Instance->CNT - p->Init_CNT_value);
	}
	else if (p->State == WAITING_INIT_TIME) p->State = TIME_CALC_ACTIVE;
	p->Init_CNT_value = v->TIMx->CNT;
  //p->Init_CNT_value = v->htim_x->Instance->CNT;
}

void OSL_TIM_Time_Calc_Init(TIM_Calc_Params_T *p)
{
	p->State = WAITING_INIT_TIME;
	p->Init_CNT_value = 0UL;
	p->Result_time_us = 0UL;
	p->Max_time_us = 0UL;
	p->Min_time_us = 65535UL;
}

void OSL_TIM_Freq_Calc_Init (Freq_Calc_Params_T *p)
{
	p->State = WAITING_INIT_TIME;
	p->Init_CNT_value = 0UL;
	p->Result_freq = 0.0F;
}

__weak uint8_t BD_TIM_Delay_Callback(TIM_Calc_Params_T *p)
{
	return 0;
}

void OSL_TIM_Delay(TIM_Calc_Params_T *p, TIMx_Params_T *m, uint32_t delay_time)
{
	OSL_TIM_Time_Calc (p, m, TIME_CALC_ACTIVE);
	do
	{		
		if ( BD_TIM_Delay_Callback(p) ) return;
		OSL_TIM_Time_Calc (p, m, VIEW_RESULT);
		if (p->State != TIME_CALC_ACTIVE) return;
	}
	while (p->Result_time_us < delay_time);
}

void OSL_TIM_Start_Delay (TIM_Calc_Params_T *p, TIMx_Params_T *m)
{
	OSL_TIM_Time_Calc (p, m, TIME_CALC_ACTIVE);
}

__weak void OSL_TIM_Stop_Delay_Callback(TIM_Calc_Params_T *p){}

void OSL_TIM_Stop_Delay(TIM_Calc_Params_T *p, TIMx_Params_T *m , uint32_t delay_time)
{
	do 
	{	
		OSL_TIM_Stop_Delay_Callback(p);
		OSL_TIM_Time_Calc (p, m, VIEW_RESULT);
	}
	while (p->Result_time_us < delay_time);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
