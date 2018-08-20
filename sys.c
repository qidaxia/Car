#include "sys.h"


#if (mclk==8000000)
//针对不同晶振做不同处理
void delay_ms(u16 ms)
{
	u16 i = 0, j = 0;
	for (i = 0; i < ms; i++)
	{
		for (j = 0; j < 1141; j++)
		{
			;
		}
#if EN_WDR==1
		_WDR();
#endif
	}
	return;
}

//
//void  delay_us(u16 us)
//{
//	while (us--);
//	return;
//}
#elif (mclk==1000000)
void delay_ms(u16 ms)
{
	u16 i = 0, j = 0;
	for (i = 0; i < ms; i++)
	{
		for (j = 0; j < 142; j++)
		{
			;
		}
	}
	return;
}
#endif

