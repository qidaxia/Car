#ifndef _USART_H
#define _USART_H
#include "sys.h"

///*!
//* @brief:  ���ջ��壨ȫ��ά����
//*/ 
//extern u8 reciveBuff[];
/*!
* @brief:  �������е���Ч���ݸ���
*/ 
//extern u8 reciveLen;

extern void clearReciveBuf(void);

extern u8 getReciveLen(void);

extern void usartInit(u16 baud);

extern void uartSendB(u8 dat);

extern void sendCMD(u8 *buf, u8 len);

//������ӹ����У����������֡��λһЩ��־λ���޸�һЩȫ��ά���ı��������ⲿʹ�� 
extern void handlerFrame(void);



#endif // !_USART_H
