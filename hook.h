#ifndef HOOKS_H
#define HOOKS_H
#include "sys.h"




extern u8 IsRun(void);

extern u8 IsRunWithTarget(void);

extern u8 IsToOrign(void);

extern u8 IsReportState(void);

extern u8 IsSendPul(void);

extern u8 IsStopPul(void);

extern u8 IsSpeedChange(void);


//key1
extern u8 IsLimitForward(void);
//key2
extern u8 IsLimitBack(void);
//key3
extern u8 IsForwardByHuman(void);
//key4
extern u8 IsBackByHuman(void);

#endif // !HOOKS_H
