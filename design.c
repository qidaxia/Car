#include "design.h"
#include "hook.h"
#include "usart.h"
#include "TLC5615.h"
#include "eeprom.h"


/*!
* @brief:  ��λ�øı�ʱ����ֵ���ۼӣ����ڱ�ʶλ�õĸı�
��0:δ���������������ˣ�����������
�ڴ洢�굱ǰ����󣬽������㡣
*/
u32	IsNewPosition = 0;


/*!
* @brief:  ��ǰ����λ��
*/
u32 currentPosition = 0;



/*!
* @brief:  ͨ�Ų�����ȫ��ά����
* waring:serialParams.direction�ֶ�ֻ���� stopʱ���Żᱻ������ΪStop�����������ֻ����ָ��
*/
PARAMETERS serialParams = {
	0,			/*!< targetPosition  */
	MiddleSpeed,	/*!< speedType  */
	Stop,		/*!< direction  */
	HighLevel	/*!< pulseStyle  */
};

//�豸��ʵʱ״̬
EnumDir deviceStatic = Stop;




/*!
* @brief:  ͨ��״̬λ��ȫ��ά����
*/
FLAG cmdFlag = {
	FALSE,	/*!< flag_run  */
	FALSE,	/*!< flag_runWithTar  */
	FALSE,	/*!< flag_toOrign  */
	FALSE,	/*!< flag_reportState  */
	FALSE,	/*!< flag_sendPul  */
	FALSE,	/*!< flag_stopPul  */
	FALSE,	/*!< flag_connectTest  */
	FALSE	/*!< flag_speedChange  */
};

static u8 ackBuff[] = { 'A','C','K' };
static u8 nakBuff[] = { 'N','A','K' };
static u8 cakBuff[] = { 'C','A','K' };
static u8 stateBuff[] = { 's','t','a','r','t', 0x00,0x00,0x00,0x00,0x00,'e','n','d' };



static void ACK(void)
{
#if EN_WDR==1
	_WDR();
#endif
	sendCMD(ackBuff, 3);
}

extern void NAK(void)
{
#if EN_WDR==1
	_WDR();
#endif
	sendCMD(nakBuff, 3);
}

extern void CAK(void)
{
#if EN_WDR==1
	_WDR();
#endif
	sendCMD(cakBuff, 3);
}



static void refreshStateBuff(void)
{
	stateBuff[5] = (u8)deviceStatic;
	stateBuff[6] = (currentPosition >> 16) + 1;
	stateBuff[7] = currentPosition >> 8;
	stateBuff[8] = currentPosition;
	stateBuff[9] = (u8)(stateBuff[6] + stateBuff[7] + stateBuff[8]);
	return;
}


static void clearAllFlag(void)
{
	cmdFlag.flag_connectTest = FALSE;
	cmdFlag.flag_reportState = FALSE;
	cmdFlag.flag_run = FALSE;
	cmdFlag.flag_runWithTar = FALSE;
	cmdFlag.flag_sendPul = FALSE;
	cmdFlag.flag_speedChange = FALSE;
	cmdFlag.flag_stopPul = FALSE;
	cmdFlag.flag_toOrign = FALSE;
}

/*
С����
	K1,K2:����
	K3,K4����Դ
	K5��5V����
������
	K1,��ת
	K2,��ת
*/
void toFwd(void)
{
#if EN_WDR==1
	_WDR();
#endif
	if (IsLimitForward())
	{
		return;
	}
	if (deviceStatic != Stop)
	{
		toStop();
	}
#if CAR

	K2_FREE;
	delay_ms(600);
	K1_CLOSE;
	//-----------
	K3_CLOSE;
#else
	changeSpeed();
	K2_FREE;
	delay_ms(100);
	K1_CLOSE;
	delay_ms(100);

#endif
	deviceStatic = Forward;
	while (IsLimitBack())//�ȴ���λ����
	{
#if EN_WDR==1
		_WDR();
#endif
	}
}



void toBack(void)
{
#if EN_WDR==1
	_WDR();
#endif
	if (IsLimitBack())
	{
		return;
	}
	if (deviceStatic != Stop)
	{
		toStop();
	}
#if CAR

	K1_FREE;
	delay_ms(600);
	K2_CLOSE;
	//-----------
	K3_CLOSE;
#else
	changeSpeed();
	K1_FREE;
	delay_ms(100);
	K2_CLOSE;
	delay_ms(100);
#endif
	deviceStatic = Back;

	while (IsLimitForward())//�ȴ���λ����
	{
#if EN_WDR==1
		_WDR();
#endif
	}
	return;
}



extern void toStop(void)
{
#if EN_WDR==1
	_WDR();
#endif
	if (deviceStatic == Stop)
	{
		return;
	}
	deviceStatic = Stop;
	//waring:The following code cannot be annotated
	//the encoder counts by this flag;
	serialParams.direction = Stop;
	clearAllFlag();//clear all flag.stop all task

#if CAR
	K1_FREE;
	K2_FREE;
	delay_ms(600);
	K3_FREE;
#else
	toZeroSpeed();
	K1_FREE;
	delay_ms(1);
	K2_FREE;
	delay_ms(1);
#endif

	// waring:
	delay_ms(500);//��ֹ����Ƶ��ͨ��
	return;
}
extern void toOrign(void)
{
	ACK();
	toBack();
	//���������е���λ�����������
	return;
}


extern void savePosition(void)
{
	u32 tempPosition = 0;

	tempPosition = currentPosition;

#if EN_WDR==1
	_WDR();
#endif
	eepromWrite(0x04, tempPosition); tempPosition >>= 8;
	eepromWrite(0x03, tempPosition); tempPosition >>= 8;
	eepromWrite(0x02, tempPosition); tempPosition >>= 8;
	eepromWrite(0x01, tempPosition);
	eepromWrite(0, 0x00);
	eepromWrite(256, 0x00);
	eepromWrite(511, 0x00);//waring:511��not 512
	onceBeep();
	return;
}

extern u32 getPositionInMemory(void)
{
	u32 tempPosition = 0;

	if (eepromRead(0) != 0x00)
	{
		goto END;
	}
	if (eepromRead(256) != 0x00)
	{
		goto END;
	}
	if (eepromRead(511) != 0x00)//ע�⣺��511����512
	{
		goto END;
	}
	tempPosition = eepromRead(0x01);
	tempPosition <<= 8;
	tempPosition += eepromRead(0x02);
	tempPosition <<= 8;
	tempPosition += eepromRead(0x03);
	tempPosition <<= 8;
	tempPosition += eepromRead(0x04);
END:
	return tempPosition;
}

extern void changeSpeed(void)
{
	ACK();

	if (serialParams.speedType == HighSpeed)
	{
		toHighSpeed();

	}
	else if (serialParams.speedType == MiddleSpeed)
	{
		toMiddleSpeed();
	}
	else
	{
		toLowSpeed();
	}
	return;
}


extern void ioInit(void)
{
	DDRA = 0xff;
	PORTA = 0xff;
	BEEP_OFF;

	DDRC = 0x00;
	PORTC = 0xff;

	DDRD &= ~BIT(IO_ENCODE_A);
	DDRD &= ~BIT(IO_ENCODE_B);

	PORTD &= ~BIT(IO_ENCODE_A);
	PORTD &= ~BIT(IO_ENCODE_B);
	return;
}

extern void onceBeep(void)
{
	BEEP_ON;
	delay_ms(50);
	BEEP_OFF;
}

extern void ReportState(void)
{
	refreshStateBuff();
	sendCMD(stateBuff, 13);
	return;
}



extern void runHandler(void)
{
	ACK();
	if (serialParams.direction == Stop)
	{
		toStop();
	}
	else if (serialParams.direction == Forward)
	{
		toFwd();
	}
	else
	{
		toBack();
	}
	return;
}



extern void runHandlerWithTarget(void)
{
	if (deviceStatic == Stop)
	{
		ACK();
		if (currentPosition < serialParams.targetPosition)
		{
			toFwd();
		}
		else if (currentPosition > serialParams.targetPosition)
		{
			toBack();
		}
		else
		{
			toStop();
		}
	}
	else
	{
		if (deviceStatic == Forward && currentPosition >= serialParams.targetPosition-ADJUST_PAUSE)
		{
			toStop();
			return;
		}
		if (deviceStatic == Back &&currentPosition <= serialParams.targetPosition+ADJUST_PAUSE)
		{
			toStop();
			return;
		}
	}

	return;
}



extern void enPulse(void)
{
	ACK();
	switch (serialParams.pulseStyle)
	{
	case HighLevel:
		SET_PUL;
		break;
	case Pulse:
		SET_PUL;
		delay_ms(200);
		CLR_PUL;
		break;
	default:
		break;
	}

}

extern void disPulse(void)
{
	ACK();
	switch (serialParams.pulseStyle)
	{
	case HighLevel:
		CLR_PUL;
		break;
	case Pulse:
		break;
	default:
		break;
	}
}

extern void humanToForward(void)
{
	if (IsLimitForward())
	{
		return;
	}
	delay_ms(100);
	if (!IsForwardByHuman())
	{
		return;
	}
	toFwd();
	while (IsForwardByHuman())
	{
		if (IsLimitForward())
		{
			goto END;
		}
#if EN_WDR==1
		_WDR();
#endif
	}
END:
	toStop();
	delay_ms(1000);
	return;
}

extern void humanToBack(void)
{
	if (IsLimitBack())
	{
		return;
	}
	delay_ms(100);
	if (!IsBackByHuman())
	{
		return;
	}
	toBack();
	while (IsBackByHuman())
	{
		if (IsLimitBack())
		{
			goto END;
		}
#if EN_WDR==1
		_WDR();
#endif
	}
END:
	toStop();
	delay_ms(1000);
	return;
}

extern void reportTarget(void)
{
	u32 temVal = serialParams.targetPosition;
	u8 tarBuf[6] = { ':',0,0,0,0,':' };

#if EN_WDR==1
	_WDR();
#endif

	tarBuf[1] = (temVal >> 16) + 1;
	tarBuf[2] = temVal >> 8;
	tarBuf[3] = temVal;
	tarBuf[4] = (u8)(tarBuf[1] + tarBuf[2] + tarBuf[3]);

	sendCMD(tarBuf, 6);
}

extern void wdtInit(void)
{
	_WDR();
	WDTCR = 0x1f;
	WDTCR = 0x0f;
}
extern void keyUp(void)
{
#if EN_WDR==1
	_WDR();
#endif
	LED_ON;
	onceBeep();
}
extern void keyDown(void)
{
#if EN_WDR==1
	_WDR();
#endif
	LED_OFF;
}


