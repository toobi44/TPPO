/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __tim_H_dim
#define __tim_H_dim
#ifdef __cplusplus
 extern "C" {
#endif

#include "osl_user_config.h"
#include "osl_general.h"
	 
// TIM_Calc_Params_T Mode
#define	LIMIT_TIME_CALC										0
#define	FREQ_CALC													1

// Reinit_allowed Commands
#define TIME_CALC_ACTIVE									(bool)0
#define VIEW_RESULT												(bool)1

#define WAITING_INIT_TIME									(bool)1

#define GO_UP															0
#define GO_DOWN														1

typedef enum 
{
	UNUSED_SET														= 0,
	FREQ_SET															= 1,		// Расчет частоты примерный по логарифмам
	//DAC_SINUS															= 2,		// Расчет точек для ЦАП
} TIMx_Params_Calc_Mode_T;

typedef union{
	uint8_t all;
	struct {
		uint8_t Int_Allowed:1;						// 0			Прерывание активно
		uint8_t Hand_Calc_Params:1;				// 1			Ручной расчет параметров
		uint8_t One_pulse:1;							// 2			Режим ручного взведения таймера
		uint8_t _32_bit_timer:1;					// 3			32 битный таймер
		uint8_t rsvd:4;										// 4-7		
	} bit;
} TIMx_Settings_T;

//Mode TIM_Calc Modes
#define TIM_Calc_Mode_16BIT	0
#define TIM_Calc_Mode_32BIT	1

typedef union{
	uint8_t all;
	struct {
		uint8_t One_pulse:1;							// 0			Режим ручного взведения таймера
		uint8_t Int_Allowed:1;						// 1			Прерывание активно
		uint8_t Symmetric_PWM:1;					// 2			Симметричный ШИМ
		uint8_t rsvd:5;										// 3-7		
	} bit;
} PWMx_Settings_T;

#define	DIRECT_PWM_MODE										1
#define	INVERSE_PWM_MODE									2
#define	BOTH_PWM_MODE											3

typedef union{
	uint8_t all;
	struct {
		uint8_t DIRECT_CH:1;							// 0			Прямой
		uint8_t INVERSE_CH:1;							// 1			Инверсный
		uint8_t Int_Allowed:1;						// 2			Прерывание активно
		uint8_t rsvd:5;										// 3-7		
	} bit;
} PWM_CHx_MODE_T;

// Params for TIMx function
typedef struct 
{
	float									Freq;
	uint32_t							Prescaler;
	uint32_t							Period;
//	uint16_t							Num_Points;
} TIMx_Reg_Params_T;

typedef struct 
{
	float									Freq_PWM;
	uint32_t							Prescaler;
	uint32_t							Period;
} PWMx_Reg_Params_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	bool											TIMx_init_was;	
	bool											TIMx_en_was;
	bool											TIMx_dis_was;
	uint32_t									TIMx_function_break;
	
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	TIM_HandleTypeDef*				htim_x;																// Structer of TIMx	
	//void											(*TIMx_Init_func)(void);							// TIMx init function	
	TIMx_Params_Calc_Mode_T		Calc_Mode;
	TIMx_Settings_T						Settings;
	uint32_t									APBx_Freq;
	
	// Changing params (must be inited, may be reinited by developer):
	bool											TIMx_en;															//Allowes TIMx to be enabled	
	TIMx_Reg_Params_T					TIMx_Reg_Params;

	// Logic inited params (mustn`t be inited, reinited):
	TIMx_Reg_Params_T					TIMx_Reg_Params_prev;
} TIMx_Params_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	bool									TIMx_init_was;	
	bool									TIMx_en_was;
	bool									TIMx_dis_was;
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	TIM_HandleTypeDef*		htim_x;																// Structer of TIMx	
	//void									(*TIMx_Init_func)(void);							// TIMx init function
	uint32_t							APBx_Freq;
	PWMx_Settings_T				Settings;	
	// Changing params (must be inited, may be reinited by developer):
	bool									TIMx_en;															//Allowes TIMx to be enabled
	PWMx_Reg_Params_T			TIMx_Reg_Params;
	// Logic inited params (mustn`t be inited, reinited):
	PWMx_Reg_Params_T			TIMx_Reg_Params_prev;
} TIMx_PWM_Params_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	bool									TIMx_init_was;	
	bool									TIMx_en_was;
	bool									TIMx_dis_was;	
	uint16_t							Point_timer;
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
	TIM_HandleTypeDef*		htim_x;																// Structer of TIMx	
	uint32_t							Channel_x;
	PWM_CHx_MODE_T				Mode;
	// Changing params (must be inited, may be reinited by developer):
	bool									TIMx_en;															//Allowes TIMx to be enabled
  uint32_t              CCR_x;
	uint16_t							Shift_deg;	
	// Logic inited params (mustn`t be inited, reinited):
	uint16_t							Shift_Point;	
} PWM_CHx_Params_T;

typedef struct
{
	// Function inited (must be inited by special function ones):
	uint32_t							Init_CNT_value;
	uint32_t							tmp_Init_CNT_value;	
	uint32_t							Result_time_us;				// Result measure	[Microsec]
	uint32_t							Max_time_us;					// Microseconds		[Microsec]
	uint32_t							Min_time_us;					// Microseconds		[Microsec]		
	bool									State;
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
} TIM_Calc_Params_T;

#define	TIME_CALC_INIT_DEF {\
/*Init_CNT_value = */0UL,\
/*tmp_Init_CNT_value = */0UL,\
/*Result_time_us = */0UL,\
/*Max_time_us = */0UL,\
/*Min_time_us = */65535UL,\
/*State = */WAITING_INIT_TIME,\
}

typedef struct
{
	// Function inited (must be inited by special function ones):
	bool									State;
	uint32_t							Init_CNT_value;
	float									Result_freq;
	float									Freq_Const;	
	// Ones setting params (mustn`t be changed by developer aftet first initing):	
} Freq_Calc_Params_T;

void BD_TIM_ReInit(TIMx_Params_T *p);
void OSL_TIM_Logical(TIMx_Params_T *p);
void BD_TIM_Logical_params_init (TIMx_Params_T *p);
void BD_TIMx_Params_Calc (TIMx_Reg_Params_T *p, uint32_t APBx_Freq);
void BD_TIM_Init_Callback(TIMx_Params_T *p);
void BD_TIM_ReInit_Callback(TIMx_Params_T *p);

// Freq Calc
void OSL_TIM_Freq_Calc_Init (Freq_Calc_Params_T *p);
void OSL_TIM_Freq_Calc (Freq_Calc_Params_T *p, TIMx_Params_T *v);

// Time Calc
void OSL_TIM_Time_Calc_Init (TIM_Calc_Params_T *p);
void OSL_TIM_Time_Calc (TIM_Calc_Params_T *p, TIMx_Params_T *v, bool Command);
void OSL_TIM_Time_Calc_Reset(TIM_Calc_Params_T *p);
void OSL_TIM_Delay (TIM_Calc_Params_T *p, TIMx_Params_T *m , uint32_t delay_time);
void OSL_TIM_Start_Delay (TIM_Calc_Params_T *p, TIMx_Params_T *m);
void OSL_TIM_Stop_Delay (TIM_Calc_Params_T *p, TIMx_Params_T *m , uint32_t delay_time);

//PWM
void OSL_TIMx_PWM_Params_Calc(TIMx_PWM_Params_T *p);

void BD_TIMx_PWM_Logical(TIMx_PWM_Params_T *p);
uint8_t OSL_PWM_CHx_Logical(PWM_CHx_Params_T *p);

void BD_TIMx_PWM_Logical_params_init (TIMx_PWM_Params_T *p);
void BD_PWM_CHx_Logical_params_init (PWM_CHx_Params_T *p);

//Callbacks:
uint8_t BD_TIM_Delay_Callback(TIM_Calc_Params_T *p);
void BD_TIMx_PWM_Init_Callback(TIMx_PWM_Params_T *p);
void BD_TIMx_PWM_ReInit_Callback(TIMx_PWM_Params_T *p);
void OSL_TIM_Stop_Delay_Callback(TIM_Calc_Params_T *p);

#ifdef __cplusplus
}
#endif
#endif
