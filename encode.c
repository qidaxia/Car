#include "encode.h"
#include "design.h"

#pragma interrupt_handler encodeISR:2

/*
方向判断：
当A相为上升沿的时候，
B相高电平：反转（从轴侧看向左转CCW）
B相低电平：正转（从轴侧看向右转CW）


//注意硬件:
大梁前进时，编码器正转
小车前进时，编码器反转
*/


static void encodeISR(void)
{
	if (serialParams.direction != Stop)
	{
		if (serialParams.direction == Forward)
		{
			currentPosition++;
		}
		else
		{
			currentPosition--;
		}
	}
	else//惯性或手动控制
	{
		//小车
#if CAR
		if (PIND&BIT(IO_ENCODE_B))//forward
		{
			currentPosition++;
		}
		else
		{
			currentPosition--;
		}

		//大梁
#else
		if (PIND&BIT(IO_ENCODE_B))//back
		{
			currentPosition--;
		}
		else
		{
			currentPosition++;
		}

#endif
	}
	IsNewPosition++;
	GIFR |= BIT(INTF0);
}

//int0,encode_A,上升沿触发
extern void encodeInit(void)
{
	MCUCR = 0x03;
	GICR |= BIT(INT0);
}