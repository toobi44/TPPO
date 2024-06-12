#include "BD_num_string.h"
#include "BD_g_Ram.h"
//#include "osl_menu.h"

// Преобразование переменной float в строку
void OSL_FLOAT_TO_STRING(uint8_t *buffer_array, float var, uint8_t signs, num_string_FLOAT_T *p, FL_STR_MODE_T Mode)
{
  p->symbol_num = 0;
  p->array_num = 0;
	p->tmp_FLOAT = var;
	p->Zero_flag = 0;

	if (p->tmp_FLOAT < 0.0F)                // Проверка на наличие знака
	{
		p->tmp_FLOAT *= -1.0F;
    buffer_array[p->array_num] = '-';
    p->array_num++;                       // Увеличиваем индекс элемента массива
	}

	if (Mode == FL_STR_MODE_SYMBOLS_ONLY)
	{
		if (p->tmp_FLOAT < 1.0F && signs)          // Проверка, есть ли целая часть
		{
			buffer_array[p->array_num] = '0';
			p->array_num++;                    // Увеличиваем индекс элемента массива	
			p->Zero_flag = 1;
		}
	}

	switch(signs)         // Если есть знаки после запятой, домножаем
	{
		case 0: break;
		case 1: p->tmp_FLOAT *= 10.0F;    break;
		case 2: p->tmp_FLOAT *= 100.0F;   break;
		case 3: p->tmp_FLOAT *= 1000.0F;  break;
		case 4: p->tmp_FLOAT *= 10000.0F; break;
		default: return;
	}
  
	p->tmp_32 = (uint32_t)roundf(p->tmp_FLOAT);
	p->num_symbols = OSL_UINT_SYMBOLS_COUNT(p->tmp_32, GVAR_T_32);
	OSL_UINT_TO_STR(p->tmp_array, p->tmp_32, p->num_symbols, 0);

	if (p->Zero_flag)
	{
		if (p->num_symbols > signs) 
			p->array_num--;
	}
	
	if (Mode == FL_STR_MODE_SYMBOLS_ONLY) 
	{
		p->symbols_delta = p->num_symbols - signs;
		for (p->i=0; p->i < p->symbols_delta; p->i++)                           // Кладем в буфер целую часть
		{
			buffer_array[p->array_num] = p->tmp_array[p->symbol_num];             // Перекладываем символы из временного буфера
			p->symbol_num++;                                                         // Плюсуем индекс элемента целого числа(после домножения)
			p->array_num++;                                                // Увеличиваем индекс элемента массива
		}
	}
	else
	{
		p->symbols_delta = p->Lim_Symbols-p->num_symbols;
		p->symbols_delta_2 = p->Lim_Symbols-signs;
		if (signs && !p->symbols_delta_2)
		{
			buffer_array[p->array_num] = '0';
			p->array_num++;                    			// Увеличиваем индекс элемента массива
		}
		
		for (p->i=0; p->i < p->symbols_delta_2; p->i++)                           // Кладем в буфер целую часть
		{
			if (p->i<p->symbols_delta)				// Заполняем нулями пустые ячейки, если они есть
			{
				buffer_array[p->array_num] = '0';
				p->array_num++;                    			// Увеличиваем индекс элемента массива			
			}
			else
			{
				buffer_array[p->array_num] = p->tmp_array[p->symbol_num];             		// Перекладываем символы из временного буфера
				p->symbol_num++;                                                         // Плюсуем индекс элемента целого числа(после домножения)
				p->array_num++;                                                					// Увеличиваем индекс элемента массива
			}
		}
	}
	
	if (signs)                       // Если есть дробная часть, добавляем точку
	{
		buffer_array[p->array_num] = '.';
		p->array_num++;         // Увеличиваем индекс элемента массива
	}
	else return;                            // Иначе выдаем результат
	
	if (Mode == FL_STR_MODE_ZERO) p->symbols_delta = p->num_symbols - signs;

  for (p->i=0; p->i < signs; p->i++)               // Складываем в буфер дробную часть
	{
		if (p->symbols_delta<0)
    {  
      buffer_array[p->array_num] = '0';
      p->symbols_delta++;
    }
    else
    {
      buffer_array[p->array_num] = p->tmp_array[p->symbol_num];
      p->symbol_num++;
    }
    p->array_num++;
	}
}

void OSL_FLOAT_TO_STRING_Lim_INIT(uint8_t signs, float Min, float Max, num_string_FLOAT_T *p)
{
	p->tmp_FLOAT = Min;
	if (p->tmp_FLOAT < 0.0F) p->tmp_FLOAT *= -1.0F;
	switch(signs)         // Если есть знаки после запятой, домножаем
	{
		case 0: break;
		case 1: p->tmp_FLOAT *= 10.0F;    break;
		case 2: p->tmp_FLOAT *= 100.0F;   break;
		case 3: p->tmp_FLOAT *= 1000.0F;  break;
		case 4: p->tmp_FLOAT *= 10000.0F; break;
		default: return;
	}
	p->tmp_32 = (uint32_t)p->tmp_FLOAT;
	p->Min_Symbols = OSL_UINT_SYMBOLS_COUNT(p->tmp_32, GVAR_T_32);
	
	p->tmp_FLOAT = Max;
	if (p->tmp_FLOAT < 0.0F) p->tmp_FLOAT *= -1.0F;
	switch(signs)         // Если есть знаки после запятой, домножаем
	{
		case 0: break;
		case 1: p->tmp_FLOAT *= 10.0F;    break;
		case 2: p->tmp_FLOAT *= 100.0F;   break;
		case 3: p->tmp_FLOAT *= 1000.0F;  break;
		case 4: p->tmp_FLOAT *= 10000.0F; break;
		default: return;
	}

	p->tmp_32 = (uint32_t)p->tmp_FLOAT;
	p->Max_Symbols = OSL_UINT_SYMBOLS_COUNT(p->tmp_32, GVAR_T_32);
  
	//Определяем наибольшее число символов(для минимума и максимумаы)
	if (p->Min_Symbols > p->Max_Symbols) p->Lim_Symbols = p->Min_Symbols;
	else p->Lim_Symbols = p->Max_Symbols;
}

uint8_t OSL_UINT_TO_STRING_Lim_INIT(uint32_t Min, uint32_t Max, uint8_t Size)
{
	uint8_t tmp_Min_num, tmp_Max_num;

	tmp_Min_num = OSL_UINT_SYMBOLS_COUNT(Min, Size);
	tmp_Max_num = OSL_UINT_SYMBOLS_COUNT(Max, Size);
	
	if (tmp_Min_num > tmp_Max_num) return tmp_Min_num;
	else return tmp_Max_num;
}

uint8_t OSL_INT_TO_STRING_Lim_INIT(int32_t Min, int32_t Max, uint8_t Size)
{
	uint8_t tmp_Min_num, tmp_Max_num;
	
	//if (Min < 0) Min*= -1;
	//if (Max < 0) Max*= -1;

	tmp_Min_num = OSL_INT_SYMBOLS_COUNT(Min, Size);				// Обрезаем минус
	tmp_Max_num = OSL_INT_SYMBOLS_COUNT(Max, Size);				// Обрезаем минус

	if (tmp_Min_num > tmp_Max_num) return tmp_Min_num;
	else return tmp_Max_num;
}

uint8_t OSL_INT_SYMBOLS_COUNT(int32_t dig, uint8_t f_n_bit)
{
	uint8_t num = 0;
	
	switch(f_n_bit)
	{
		case GVAR_T_8:
		{
			if( (dig / 100) != 0) num = 3;
			else if( (dig / 10) != 0) num = 2;
			else num = 1;
		}
		break;
		
		case GVAR_T_16:
		{
			if( (dig / 10000) != 0) num = 5;
			else if( (dig / 1000) != 0) num = 4;
			else if( (dig / 100) != 0) num = 3;
			else if( (dig / 10) != 0) num = 2;
			else num = 1;
		}
		break;
		
		case GVAR_T_32:
		{
			if( (dig / 1000000000) != 0) num = 10;
			else if( (dig / 100000000) != 0) num = 9;
			else if( (dig / 10000000) != 0) num = 8;
			else if( (dig / 1000000) != 0) num = 7;
			else if( (dig / 100000) != 0) num = 6;
			else if( (dig / 10000) != 0) num = 5;
			else if( (dig / 1000) != 0) num = 4;
			else if( (dig / 100) != 0) num = 3;
			else if( (dig / 10) != 0) num = 2;
			else num = 1;
		}
		break;
		
		default: return 0;
	}
	return(num);
}

uint8_t OSL_UINT_SYMBOLS_COUNT(uint32_t dig, uint8_t f_n_bit)
{
	uint8_t		num = 0;
	
	switch(f_n_bit)
	{
		case GVAR_T_8:
		{
			if( (dig / 100) != 0) num = 3;
			else if( (dig / 10) != 0) num = 2;
			else num = 1;
		}
		break;
		
		case GVAR_T_16:
		{
			if( (dig / 10000) != 0) num = 5;
			else if( (dig / 1000) != 0) num = 4;
			else if( (dig / 100) != 0) num = 3;
			else if( (dig / 10) != 0) num = 2;
			else num = 1;
		}
		break;
		
		case GVAR_T_32:
		{
			if( (dig / 1000000000) != 0) num = 10;
			else if( (dig / 100000000) != 0) num = 9;
			else if( (dig / 10000000) != 0) num = 8;
			else if( (dig / 1000000) != 0) num = 7;
			else if( (dig / 100000) != 0) num = 6;
			else if( (dig / 10000) != 0) num = 5;
			else if( (dig / 1000) != 0) num = 4;
			else if( (dig / 100) != 0) num = 3;
			else if( (dig / 10) != 0) num = 2;
			else num = 1;
		}
		break;
		
		default: return 0;
	}
	return(num);
}

void OSL_UINT_TO_STR(uint8_t *str, uint32_t dig, uint8_t num, uint8_t delta)
{
	uint8_t sign_shift = 0;
	
	if (delta) for (uint8_t i = 0; i < delta; i++) str[sign_shift++] = '0';	
	
	switch(num)
	{
		case 1:
			str[sign_shift] = (dig % 10) + 0x30;
		break;
		
		case 2:
			str[sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;
		break;
		
		case 3:
			str[sign_shift] = ((dig % 1000) / 100) + 0x30;
			str[++sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;		
		break;
		
		case 4:
			str[sign_shift] = ((dig % 10000) / 1000) + 0x30;
			str[++sign_shift] = ((dig % 1000) / 100) + 0x30;
			str[++sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;				
		break;
		
		case 5:
			str[sign_shift] = ((dig % 100000) / 10000) + 0x30;
			str[++sign_shift] = ((dig % 10000) / 1000) + 0x30;
			str[++sign_shift] = ((dig % 1000) / 100) + 0x30;
			str[++sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;		
		break;
		
		case 6:
			str[sign_shift] = ((dig % 1000000) / 100000) + 0x30;
			str[++sign_shift] = ((dig % 100000) / 10000) + 0x30;
			str[++sign_shift] = ((dig % 10000) / 1000) + 0x30;
			str[++sign_shift] = ((dig % 1000) / 100) + 0x30;
			str[++sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;		
		break;
		
		case 7:
			str[sign_shift] = ((dig % 10000000) / 1000000) + 0x30;
			str[++sign_shift] = ((dig % 1000000) / 100000) + 0x30;
			str[++sign_shift] = ((dig % 100000) / 10000) + 0x30;
			str[++sign_shift] = ((dig % 10000) / 1000) + 0x30;
			str[++sign_shift] = ((dig % 1000) / 100) + 0x30;
			str[++sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;		
		break;
		
		case 8:
			str[sign_shift] = ((dig % 100000000) / 10000000) + 0x30;
			str[++sign_shift] = ((dig % 10000000) / 1000000) + 0x30;
			str[++sign_shift] = ((dig % 1000000) / 100000) + 0x30;
			str[++sign_shift] = ((dig % 100000) / 10000) + 0x30;
			str[++sign_shift] = ((dig % 10000) / 1000) + 0x30;
			str[++sign_shift] = ((dig % 1000) / 100) + 0x30;
			str[++sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;			
		break;
		
		case 9:
			str[sign_shift] = ((dig % 1000000000) / 100000000) + 0x30;
			str[++sign_shift] = ((dig % 100000000) / 10000000) + 0x30;
			str[++sign_shift] = ((dig % 10000000) / 1000000) + 0x30;
			str[++sign_shift] = ((dig % 1000000) / 100000) + 0x30;
			str[++sign_shift] = ((dig % 100000) / 10000) + 0x30;
			str[++sign_shift] = ((dig % 10000) / 1000) + 0x30;
			str[++sign_shift] = ((dig % 1000) / 100) + 0x30;
			str[++sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;		
		break;
		
		case 10:
			str[sign_shift] = ((dig % 10000000000) / 1000000000) + 0x30;
			str[++sign_shift] = ((dig % 1000000000) / 100000000) + 0x30;
			str[++sign_shift] = ((dig % 100000000) / 10000000) + 0x30;
			str[++sign_shift] = ((dig % 10000000) / 1000000) + 0x30;
			str[++sign_shift] = ((dig % 1000000) / 100000) + 0x30;
			str[++sign_shift] = ((dig % 100000) / 10000) + 0x30;
			str[++sign_shift] = ((dig % 10000) / 1000) + 0x30;
			str[++sign_shift] = ((dig % 1000) / 100) + 0x30;
			str[++sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;
		break;
	}
}

void OSL_INT_TO_STR(uint8_t *str, int32_t Var, uint8_t* num, uint8_t delta)
{
	uint32_t dig;
	uint8_t sign_shift = 0;
	bool sign_flag = false;
	uint8_t tmp_num = *num;
	
	if (Var<0) 
	{	
		dig = Var*(-1);
		str[0] = '-';
		sign_shift++;
		sign_flag = true;
	}
	else dig = Var;
	
	if (delta) 
		for (uint8_t i = 0; i < delta; i++) 
		{	
			str[sign_shift++] = '0';
			(*num)++;
		}
		
	switch(tmp_num)
	{
		case 1:
			str[sign_shift] = (dig % 10) + 0x30;
		break;
		
		case 2:
			str[sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;
		break;
		
		case 3:
			str[sign_shift] = ((dig % 1000) / 100) + 0x30;
			str[++sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;		
		break;
		
		case 4:
			str[sign_shift] = ((dig % 10000) / 1000) + 0x30;
			str[++sign_shift] = ((dig % 1000) / 100) + 0x30;
			str[++sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;				
		break;
		
		case 5:
			str[sign_shift] = ((dig % 100000) / 10000) + 0x30;
			str[++sign_shift] = ((dig % 10000) / 1000) + 0x30;
			str[++sign_shift] = ((dig % 1000) / 100) + 0x30;
			str[++sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;		
		break;
		
		case 6:
			str[sign_shift] = ((dig % 1000000) / 100000) + 0x30;
			str[++sign_shift] = ((dig % 100000) / 10000) + 0x30;
			str[++sign_shift] = ((dig % 10000) / 1000) + 0x30;
			str[++sign_shift] = ((dig % 1000) / 100) + 0x30;
			str[++sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;		
		break;
		
		case 7:
			str[sign_shift] = ((dig % 10000000) / 1000000) + 0x30;
			str[++sign_shift] = ((dig % 1000000) / 100000) + 0x30;
			str[++sign_shift] = ((dig % 100000) / 10000) + 0x30;
			str[++sign_shift] = ((dig % 10000) / 1000) + 0x30;
			str[++sign_shift] = ((dig % 1000) / 100) + 0x30;
			str[++sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;		
		break;
		
		case 8:
			str[sign_shift] = ((dig % 100000000) / 10000000) + 0x30;
			str[++sign_shift] = ((dig % 10000000) / 1000000) + 0x30;
			str[++sign_shift] = ((dig % 1000000) / 100000) + 0x30;
			str[++sign_shift] = ((dig % 100000) / 10000) + 0x30;
			str[++sign_shift] = ((dig % 10000) / 1000) + 0x30;
			str[++sign_shift] = ((dig % 1000) / 100) + 0x30;
			str[++sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;			
		break;
		
		case 9:
			str[sign_shift] = ((dig % 1000000000) / 100000000) + 0x30;
			str[++sign_shift] = ((dig % 100000000) / 10000000) + 0x30;
			str[++sign_shift] = ((dig % 10000000) / 1000000) + 0x30;
			str[++sign_shift] = ((dig % 1000000) / 100000) + 0x30;
			str[++sign_shift] = ((dig % 100000) / 10000) + 0x30;
			str[++sign_shift] = ((dig % 10000) / 1000) + 0x30;
			str[++sign_shift] = ((dig % 1000) / 100) + 0x30;
			str[++sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;		
		break;
		
		case 10:
			str[sign_shift] = ((dig % 10000000000) / 1000000000) + 0x30;
			str[++sign_shift] = ((dig % 1000000000) / 100000000) + 0x30;
			str[++sign_shift] = ((dig % 100000000) / 10000000) + 0x30;
			str[++sign_shift] = ((dig % 10000000) / 1000000) + 0x30;
			str[++sign_shift] = ((dig % 1000000) / 100000) + 0x30;
			str[++sign_shift] = ((dig % 100000) / 10000) + 0x30;
			str[++sign_shift] = ((dig % 10000) / 1000) + 0x30;
			str[++sign_shift] = ((dig % 1000) / 100) + 0x30;
			str[++sign_shift] = ((dig % 100) / 10) + 0x30;
			str[++sign_shift] = (dig % 10) + 0x30;
		break;
	}
	if (sign_flag) (*num)++;
}
