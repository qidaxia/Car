#ifndef _USART_H
#define _USART_H
#include "sys.h"

///*!
//* @brief:  接收缓冲（全局维护）
//*/ 
//extern u8 reciveBuff[];
/*!
* @brief:  缓冲区中的有效数据个数
*/ 
//extern u8 reciveLen;

extern void clearReciveBuf(void);

extern u8 getReciveLen(void);

extern void usartInit(u16 baud);

extern void uartSendB(u8 dat);

extern void sendCMD(u8 *buf, u8 len);

//在这个子过程中，会根据数据帧置位一些标志位，修改一些全局维护的变量，供外部使用 
extern void handlerFrame(void);



#endif // !_USART_H
