/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __gpio_button_H_dim
#define __gpio_button_H_dim
#ifdef __cplusplus
 extern "C" {
#endif

#include "osl_user_config.h"	 
#include "osl_general.h"
#include "tim.h"

// Defines:
#define BUT_PUSHED								0
#define BUT_NON_PUSHED						1
#define BUT_INITING								2
	 
typedef enum
{
	BUT_STATE_FREE									= 0,
	BUT_STATE_PUSHED								= 1,
	BUT_STATE_NON_PUSHED						= 2,
	BUT_STATE_PUSH_FRONT						= 3,	
	BUT_STATE_NON_PUSH_FRONT				= 4,
} BUT_STATE_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	uint32_t								change_state_timer;
//uint32_t								change_front_timer;	
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	uint8_t									dur_state;	
	bool										push_init_flag;
	bool										non_push_init_flag;
//bool										front_search_flag;
	float*									T_samp;
	float*									push_period;
	float*									non_push_period;
//	float										front_period;
	// Changing params (must be inited, may be reinited by developer):
	// Logic inited params (mustn`t be inited, reinited):
} 
Button_struct_T;

#define	BUT_STR_INIT_DEF {\
.change_state_timer = 0,\
.dur_state = BUT_INITING,\
.push_init_flag = true,\
.non_push_init_flag = true,\
}

BUT_STATE_T OSL_but_state_monitor (GPIO_PinState pin_state, Button_struct_T *p);
void OSL_but_state_monitor_reinit(Button_struct_T *p);

#ifdef __cplusplus
}
#endif
#endif
