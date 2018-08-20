#include "hook.h"
#include "design.h"



extern u8 IsKey1(void)
{
	if (PINC&BIT(IO_KEY1))
	{
		return FALSE;
	}
	return TRUE;
}

extern u8 IsKey2(void)
{
	if (PINC&BIT(IO_KEY2))
	{
		return FALSE;
	}
	return TRUE;
}

extern u8 IsKey3(void)
{
	if (PINC&BIT(IO_KEY3))
	{
		return FALSE;
	}
	return TRUE;
}

extern u8 IsKey4(void)
{
	if (PINC&BIT(IO_KEY4))
	{
		return FALSE;
	}
	return TRUE;
}


extern u8 IsRun(void)
{
	if (cmdFlag.flag_run)
	{
		cmdFlag.flag_run = FALSE;
		return TRUE;
	}
	return FALSE;
}

extern u8 IsRunWithTarget(void)
{
	if (cmdFlag.flag_runWithTar)
	{
		//cmdFlag.flag_runWithTar = FALSE;//主循环中换需要进入检测
		return TRUE;
	}
	return FALSE;
}

extern u8 IsToOrign(void)
{
	if (cmdFlag.flag_toOrign)
	{
		cmdFlag.flag_toOrign = FALSE;
		return TRUE;
	}
	return FALSE;
}

extern u8 IsReportState(void)
{
	if (cmdFlag.flag_reportState)
	{
		cmdFlag.flag_reportState = FALSE;
		return TRUE;
	}
	return FALSE;
}

extern u8 IsSendPul(void)
{
	if (cmdFlag.flag_sendPul)
	{
		cmdFlag.flag_sendPul = FALSE;
		return TRUE;
	}
	return FALSE;
}

extern u8 IsStopPul(void)
{
	if (cmdFlag.flag_stopPul)
	{
		cmdFlag.flag_stopPul = FALSE;
		return TRUE;
	}
	return FALSE;
}


extern u8 IsSpeedChange(void)
{
	if (cmdFlag.flag_speedChange)
	{
		cmdFlag.flag_speedChange = FALSE;
		return TRUE;
	}
	return FALSE;
}

extern u8 IsLimitForward(void)
{
	return IsKey1();
}
extern u8 IsLimitBack(void)
{
	return IsKey2();
}

extern u8 IsForwardByHuman(void)
{
	return IsKey3();
}

extern u8 IsBackByHuman(void)
{
	return IsKey4();
}
