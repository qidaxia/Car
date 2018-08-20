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


//寻找命令头，失败返回0xff,成功返回对应的索引（s所在的索引）
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
* @brief:  数据接收完成执行，
*/
extern void handlerFrame(void)
{
	u8 len = getReciveLen();
	u8 headIndex = 0xff;
	if (len < 9)//数据不完整
	{
		return;
	}
	if (len > 20)//错误数据
	{
		reciveLen = 0;
		return;
	}
	//验证命令尾
	if (reciveBuff[len - 1] == 'd'&&reciveBuff[len - 2] == 'n'&&reciveBuff[len - 3] == 'e')
	{
		//寻找命令头
		headIndex = searchCMDHead(len);
		if (headIndex != 0xff)//找到了
		{
			switch (reciveBuff[headIndex + 5])
			{
			case 0x01://电机控制（一直运行）
				serialParams.direction = (EnumDir)reciveBuff[headIndex + 6];
				cmdFlag.flag_run = TRUE;
				break;
			case 0x11://运动到指定的坐标,6,7,8,9(校验)
				if (len < 13 || ((len > headIndex) && (len - headIndex < 13)))//丢帧
				{
					NAK();
				}
				else if ((u8)(reciveBuff[headIndex + 6] + reciveBuff[headIndex + 7] + reciveBuff[headIndex + 8]) != reciveBuff[9])
				{
					CAK();
				}
				else
				{
					//为避免错误，三字节，最高位加一传输
					serialParams.targetPosition = reciveBuff[headIndex + 6] - 1; serialParams.targetPosition <<= 8;
					serialParams.targetPosition = serialParams.targetPosition + reciveBuff[headIndex + 7]; serialParams.targetPosition <<= 8;
					serialParams.targetPosition = serialParams.targetPosition + reciveBuff[headIndex + 8];
					cmdFlag.flag_runWithTar = TRUE;
				}
				break;
			case 0x02://回归原点
				cmdFlag.flag_toOrign = TRUE;
				break;
			case 0x03://上报状态
				cmdFlag.flag_reportState = TRUE;
				break;
			case 0x04://发送脉冲
				serialParams.pulseStyle = (EnumPulseStyle)reciveBuff[headIndex + 6];
				cmdFlag.flag_sendPul = TRUE;
				break;
			case 0x06://切换速度
				serialParams.speedType = (EnumSpeed)reciveBuff[headIndex + 6];
				cmdFlag.flag_speedChange = TRUE;
				break;
			case 0x07://停止脉冲
				cmdFlag.flag_stopPul = TRUE;
				break;
			case 0x08://(调试，上报目标地址)
				reportTarget();
			default:
				break;
			}

		}
		//数据不完整，或处理完成，则重新开始接收
		clearReciveBuf();
	}
	else//验证命令尾不通过，重新接收
	{
		clearReciveBuf();
	}
	return;
}
