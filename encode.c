#include "encode.h"
#include "design.h"

#pragma interrupt_handler encodeISR:2

/*
�����жϣ�
��A��Ϊ�����ص�ʱ��
B��ߵ�ƽ����ת������࿴����תCCW��
B��͵�ƽ����ת������࿴����תCW��


//ע��Ӳ��:
����ǰ��ʱ����������ת
С��ǰ��ʱ����������ת
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
	else//���Ի��ֶ�����
	{
		//С��
#if CAR
		if (PIND&BIT(IO_ENCODE_B))//forward
		{
			currentPosition++;
		}
		else
		{
			currentPosition--;
		}

		//����
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

//int0,encode_A,�����ش���
extern void encodeInit(void)
{
	MCUCR = 0x03;
	GICR |= BIT(INT0);
}