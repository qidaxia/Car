/**
 * @file car.c
 * @date 2018/03/21 11:28
 *
 * @author QiDaXia
 * @Contact: 1176201157@qq.com
 *
 * @brief:
 �޸���־��
 2018/04/16 ���ӣ��յ�ָֹͣ��ͷ���������еı�־λ����ֹ���е�����
 2018/04/20 �޸ģ��ڴ�����С���ϰ�װ�ı�����ת������ͬ��
			����ǰ��ʱ����������ת
			С��ǰ��ʱ����������ת
			����ڱ������ж�ʱ���������������룬��ʶ���ڹ���״̬�»��ֶ�ģʽʱ�ļ�������
 2018/04/22 �����̵����պ�ʱ϶��
			���豸״̬���룬��Ϊָ��״̬��ʵʱ״̬��������ʶ��������ָ��״̬���ϱ�״̬ʱ�ϱ�ʵʱ״̬
			ɾ�� k3�̵������루Ӳ����ֹͣ��Ϊk4�����ⲿ˫ͨ���̵���ʵ�֣�

 key1:ǰ����λ
 key2:������λ
 key3:�ֶ�ǰ��
 key4:�ֶ�����

 *
 * @description:
 *
 * @note:
 ��˿λ��FE D1 FF
*/

#include "design.h"
#include "hook.h"
#include "usart.h"
#include "encode.h"
#include "TLC5615.h"
#include "eeprom.h"


/*
eeprom�洢��ǰλ��(512�ֽ�)��ÿ�ο���ʱ��ȡ
0x0000:У��λ(���������Ч����Ϊ0x00,����Ϊ0xff)
0x00FF:У��λ
0x01FF:У��λ

0x01:��λ
0x02:
0x03:
0x04:��λ
*/

int main(void)
{
	u8 oldLen = 0;
	u32 oldIsNewPosition = 0;
	u16 newPositionCnt = 0;//���ڸ���eeprom��ʱ
	u8 i = 5;//���ڸ�λ����
	ioInit();

	encodeInit();
	SPI_MasterInit();
	usartInit(9600);
	wdtInit();
	delay_ms(2000);
	currentPosition = getPositionInMemory();
	//��ϵͳ��λ��λʱ���Ϳ���������ʾ���ж�
	while (i--)
	{
		onceBeep();
		delay_ms(100);
		onceBeep();
		delay_ms(500);
	}
	while (TRUE)
	{

#if CAR==0
		//�л��ٶ�
		if (IsSpeedChange())
		{
			keyUp();
			changeSpeed();
			keyDown();
		}
#endif
		//�˶�ָ��޾��������
		if (IsRun())
		{
			keyUp();
			runHandler();
			keyDown();
		}
		//�˶�ָ�ָ�����룩
		if (IsRunWithTarget())
		{
			runHandlerWithTarget();
		}
		//��ԭ��
		if (IsToOrign())
		{
			keyUp();
			toOrign();
			keyDown();
		}
		//�ϱ�״̬
		if (IsReportState())
		{
			keyUp();
			ReportState();
			keyDown();
		}
		//��������
		if (IsSendPul())
		{
			keyUp();
			enPulse();
			keyDown();
		}
		//ֹͣ���壨�����ڸߵ�ƽ�����
		if (IsStopPul())
		{
			keyUp();
			disPulse();
			keyDown();
		}
		//��λ�����漰��ȫ����Ҫ�ϸߵ����ȼ���
		if (IsLimitForward())
		{
			if (deviceStatic == Forward || deviceStatic == Stop)
			{
				toStop();
			}
		}
		if (IsLimitBack())
		{
			if (deviceStatic == Back || deviceStatic == Stop)
			{
				currentPosition = 0;
				toStop();
			}
		}
		//�ֶ�ģʽ
		if (IsForwardByHuman())
		{
			keyUp();
			humanToForward();
			keyDown();
		}
		if (IsBackByHuman())
		{
			keyUp();
			humanToBack();
			keyDown();
		}
		//------------------����������ݴ���
		if (getReciveLen() != 0 && oldLen == getReciveLen())
		{
#if EN_WDR==1
			_WDR();
#endif
			handlerFrame();
			oldLen = 0;
			clearReciveBuf();
		}
		else
		{
			oldLen = getReciveLen();
		}

		//--------------�洢��ǰλ�ô���
		if (IsNewPosition != 0 && oldIsNewPosition == IsNewPosition)
		{
#if EN_WDR==1
			_WDR();
#endif
			//��ʱ4.5���ӱ���30*150���˼�����豸�����ٶ��йأ�����̫��
			newPositionCnt++;
			if (newPositionCnt >= 150)
			{
				savePosition();//������̰����ŷ���������;
				IsNewPosition = 0;//�����ʶ�豸λ���Ƿ�ı�ı���
				oldIsNewPosition = 0;
				newPositionCnt = 0;
			}
		}
		else
		{
			oldIsNewPosition = IsNewPosition;
		}
		//-------------------------
		delay_ms(30);//��Ҫɾ������֤���ݿɿ�����

		//----------------------����˸����ʾ��������
		if ((PINA&BIT(IO_LED)) == 0)
		{
			LED_OFF;
		}
		else
		{
			LED_ON;
		}
	}
	return 0;
}
