/**
 * @file car.c
 * @date 2018/03/21 11:28
 *
 * @author QiDaXia
 * @Contact: 1176201157@qq.com
 *
 * @brief:
 修改日志：
 2018/04/16 增加，收到停止指令头，清零所有的标志位，终止所有的任务
 2018/04/20 修改，在大梁和小车上安装的编码器转动方向不同，
			大梁前进时，编码器正转
			小车前进时，编码器反转
			因此在编码器中断时，加入了条件编译，以识别在惯性状态下或手动模式时的计数错误
 2018/04/22 调整继电器闭合时隙，
			将设备状态分离，分为指令状态，实时状态，编码器识别方向依托指令状态，上报状态时上报实时状态
			删除 k3继电器代码（硬件将停止改为k4触发外部双通道继电器实现）

 key1:前进限位
 key2:后退限位
 key3:手动前进
 key4:手动后退

 *
 * @description:
 *
 * @note:
 熔丝位：FE D1 FF
*/

#include "design.h"
#include "hook.h"
#include "usart.h"
#include "encode.h"
#include "TLC5615.h"
#include "eeprom.h"


/*
eeprom存储当前位置(512字节)，每次开机时读取
0x0000:校验位(如果数据有效，则为0x00,否则为0xff)
0x00FF:校验位
0x01FF:校验位

0x01:高位
0x02:
0x03:
0x04:低位
*/

int main(void)
{
	u8 oldLen = 0;
	u32 oldIsNewPosition = 0;
	u16 newPositionCnt = 0;//用于更新eeprom计时
	u8 i = 5;//用于复位鸣笛
	ioInit();

	encodeInit();
	SPI_MasterInit();
	usartInit(9600);
	wdtInit();
	delay_ms(2000);
	currentPosition = getPositionInMemory();
	//当系统复位复位时，就可以利用提示音判断
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
		//切换速度
		if (IsSpeedChange())
		{
			keyUp();
			changeSpeed();
			keyDown();
		}
#endif
		//运动指令（无距离参数）
		if (IsRun())
		{
			keyUp();
			runHandler();
			keyDown();
		}
		//运动指令（指定距离）
		if (IsRunWithTarget())
		{
			runHandlerWithTarget();
		}
		//归原点
		if (IsToOrign())
		{
			keyUp();
			toOrign();
			keyDown();
		}
		//上报状态
		if (IsReportState())
		{
			keyUp();
			ReportState();
			keyDown();
		}
		//发送脉冲
		if (IsSendPul())
		{
			keyUp();
			enPulse();
			keyDown();
		}
		//停止脉冲（仅用于高电平情况）
		if (IsStopPul())
		{
			keyUp();
			disPulse();
			keyDown();
		}
		//限位，（涉及安全，需要较高的优先级）
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
		//手动模式
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
		//------------------接收完成数据处理
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

		//--------------存储当前位置处理
		if (IsNewPosition != 0 && oldIsNewPosition == IsNewPosition)
		{
#if EN_WDR==1
			_WDR();
#endif
			//计时4.5秒钟保存30*150：此间隔与设备运行速度有关，不可太短
			newPositionCnt++;
			if (newPositionCnt >= 150)
			{
				savePosition();//保存过程伴随着蜂鸣器提醒;
				IsNewPosition = 0;//清零标识设备位置是否改变的变量
				oldIsNewPosition = 0;
				newPositionCnt = 0;
			}
		}
		else
		{
			oldIsNewPosition = IsNewPosition;
		}
		//-------------------------
		delay_ms(30);//不要删掉，保证数据可靠传输

		//----------------------灯闪烁，提示程序运行
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
