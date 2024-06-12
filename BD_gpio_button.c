#include "BD_gpio_button.h"

BUT_STATE_T OSL_but_state_monitor(GPIO_PinState pin_state, Button_struct_T *p)
{
	if (p->dur_state == BUT_NON_PUSHED)
	{
		if (!pin_state)
		{
			p->change_state_timer++;
			if (*p->T_samp * (float)p->change_state_timer >= *p->push_period)
			{
				p->change_state_timer = 0;
				p->dur_state = BUT_PUSHED;
				return BUT_STATE_PUSHED;
			}
		}
		else if (p->change_state_timer) p->change_state_timer--;
		return BUT_STATE_FREE;
	}
	else if (p->dur_state == BUT_PUSHED)
	{
		if (pin_state)
		{
			p->change_state_timer++;
			if (*p->T_samp * (float)p->change_state_timer >= *p->non_push_period)
			{
				p->change_state_timer = 0;
				p->dur_state = BUT_NON_PUSHED;				
				return BUT_STATE_NON_PUSHED;
			}
		}
		else if (p->change_state_timer) p->change_state_timer--;
		return BUT_STATE_FREE;
	}
	else if (p->dur_state == BUT_INITING)
	{
		if (!pin_state)
		{
			if (p->push_init_flag)
			{
				p->push_init_flag = false;
				p->change_state_timer = 0;
			}

			p->non_push_init_flag = true;		
			p->change_state_timer++;
			if (*p->T_samp * (float)p->change_state_timer >= *p->push_period)
			{
				p->change_state_timer = 0;
				p->dur_state = BUT_PUSHED;
				return BUT_STATE_PUSHED;
			}
			return BUT_STATE_FREE;
		}
		else// if (pin_state == BUT_NON_PUSHED)
		{
			if (p->non_push_init_flag)
			{
				p->non_push_init_flag = false;
				p->change_state_timer = 0;
			}			
			
			p->push_init_flag = true;
			p->change_state_timer++;
			if (*p->T_samp * (float)p->change_state_timer >= *p->non_push_period)
			{
				p->change_state_timer = 0;
				p->dur_state = BUT_NON_PUSHED;
				return BUT_STATE_NON_PUSHED;
			}
			return BUT_STATE_FREE;			
		}
	}
	return BUT_STATE_FREE;	
}

void OSL_but_state_monitor_reinit(Button_struct_T *p)
{
	p->change_state_timer = 0;
	p->dur_state = BUT_INITING;
	p->push_init_flag = true;
	p->non_push_init_flag = true;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
