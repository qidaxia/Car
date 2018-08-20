#include "usart.h"
#include "design.h"

#pragma interrupt_handler uart_rx:12


static u8 reciveLen = 0;
static u8 reciveBuff[256];//256

static void uart_rx(void)
{
	UCSRB &= ~BIT(RXCIE);
	reciveBuff[reciveLen++] = UDR;
	//other handler
	UCSRB |= BIT(RXCIE);
}
extern void clearReciveBuf(void)
{
	reciveLen = 0;
}
extern u8 getReciveLen(void)
{
	return reciveLen;
}
extern void usartInit(u16 baud)
{
	UCSRB = 0x00;
	UCSRA = 0x00;
	UCSRC = (1 << URSEL) | (0 << UPM0) | (3 << UCSZ0);
	baud = mclk / 16 / baud - 1;
	UBRRL = baud;
	UBRRH = baud >> 8;
	UCSRB = (1 << TXEN) | (1 << RXEN) | (1 << RXCIE);
	SREG = BIT(7);
	DDRD |= 0X02;
}

extern void uartSendB(u8 dat)
{
	while (!(UCSRA&(BIT(UDRE))));
	UDR = dat;
	while (!(UCSRA&(BIT(TXC))));
	UCSRA |= BIT(TXC);
}



extern void sendCMD(u8 *buf, u8 len)
{
	u8 i = 0;
	for (i = 0; i < len; i++)
	{
		uartSendB(buf[i]);
	}
}


//Ѱ������ͷ��ʧ�ܷ���0xff,�ɹ����ض�Ӧ��������s���ڵ�������
static u8 searchCMDHead(u8 len)
{
	u8 i = 0;
	u8 headIndex = 0xff;
	for (i = 0; i < len - 4; i++)
	{
		if (reciveBuff[i] == 's'&&reciveBuff[i + 1] == 't'&&reciveBuff[i + 2] == 'a'&&reciveBuff[i + 3] == 'r'&&reciveBuff[i + 4] == 't')
		{
			headIndex = i;
			break;
		}
	}
	return headIndex;
}


/*!
* @brief:  ���ݽ������ִ�У�
*/
extern void handlerFrame(void)
{
	u8 len = getReciveLen();
	u8 headIndex = 0xff;
	if (len < 9)//���ݲ�����
	{
		return;
	}
	if (len > 20)//��������
	{
		reciveLen = 0;
		return;
	}
	//��֤����β
	if (reciveBuff[len - 1] == 'd'&&reciveBuff[len - 2] == 'n'&&reciveBuff[len - 3] == 'e')
	{
		//Ѱ������ͷ
		headIndex = searchCMDHead(len);
		if (headIndex != 0xff)//�ҵ���
		{
			switch (reciveBuff[headIndex + 5])
			{
			case 0x01://������ƣ�һֱ���У�
				serialParams.direction = (EnumDir)reciveBuff[headIndex + 6];
				cmdFlag.flag_run = TRUE;
				break;
			case 0x11://�˶���ָ��������,6,7,8,9(У��)
				if (len < 13 || ((len > headIndex) && (len - headIndex < 13)))//��֡
				{
					NAK();
				}
				else if ((u8)(reciveBuff[headIndex + 6] + reciveBuff[headIndex + 7] + reciveBuff[headIndex + 8]) != reciveBuff[9])
				{
					CAK();
				}
				else
				{
					//Ϊ����������ֽڣ����λ��һ����
					serialParams.targetPosition = reciveBuff[headIndex + 6] - 1; serialParams.targetPosition <<= 8;
					serialParams.targetPosition = serialParams.targetPosition + reciveBuff[headIndex + 7]; serialParams.targetPosition <<= 8;
					serialParams.targetPosition = serialParams.targetPosition + reciveBuff[headIndex + 8];
					cmdFlag.flag_runWithTar = TRUE;
				}
				break;
			case 0x02://�ع�ԭ��
				cmdFlag.flag_toOrign = TRUE;
				break;
			case 0x03://�ϱ�״̬
				cmdFlag.flag_reportState = TRUE;
				break;
			case 0x04://��������
				serialParams.pulseStyle = (EnumPulseStyle)reciveBuff[headIndex + 6];
				cmdFlag.flag_sendPul = TRUE;
				break;
			case 0x06://�л��ٶ�
				serialParams.speedType = (EnumSpeed)reciveBuff[headIndex + 6];
				cmdFlag.flag_speedChange = TRUE;
				break;
			case 0x07://ֹͣ����
				cmdFlag.flag_stopPul = TRUE;
				break;
			case 0x08://(���ԣ��ϱ�Ŀ���ַ)
				reportTarget();
			default:
				break;
			}

		}
		//���ݲ�������������ɣ������¿�ʼ����
		clearReciveBuf();
	}
	else//��֤����β��ͨ�������½���
	{
		clearReciveBuf();
	}
	return;
}
