#ifndef DESIGN_H
#define DESIGN_H
#include "sys.h"

/*!
* @brief:

0:bridge
1:car

*/
#define		CAR		0


/*误差消除*/
#if CAR	
#define ADJUST_PAUSE	2
#else
#define ADJUST_PAUSE	6
#endif


#define IO_LED		PA0
#define IO_K1		PA1			/*!<dir  + */
#define IO_K2		PA2			/*!<dir  - */
#define IO_K3		PA3			/*!<en   */
#define IO_K4		PA4			/*!<en   */
#define IO_K5		PA5			/*!<5V pulse  */
#define IO_K6		PA6			/*!<reverse   */
#define IO_BEEP		PA7

#define IO_RXD			PD0
#define IO_TXD			PD1
#define IO_ENCODE_A		PD2
#define IO_ENCODE_B		PD3

#define IO_KEY1		PC0
#define IO_KEY2		PC1
#define IO_KEY3		PC2
#define IO_KEY4		PC3


#if CAR
#define IO_CAR_DIR1	IO_K1
#define IO_CAR_DIR2	IO_K2
#define IO_CAR_EN1	IO_K3
#define IO_CAR_EN2	IO_K4
#define IO_CAR_5V	IO_K5
#else
#define IO_BRIDGE_DIR1	IO_K1
#define IO_BRIDGE_DIR2	IO_K2

#endif


#define LED_ON		PORTA&=~BIT(IO_LED)
#define LED_OFF		PORTA|=BIT(IO_LED)
#define BEEP_ON		PORTA|=BIT(IO_BEEP)
#define BEEP_OFF	PORTA&=~BIT(IO_BEEP)


#define K1_CLOSE	PORTA&=~BIT(IO_K1)
#define K2_CLOSE	PORTA&=~BIT(IO_K2)
#define K3_CLOSE	PORTA&=~BIT(IO_K3)
#define K4_CLOSE	PORTA&=~BIT(IO_K4)
#define K5_CLOSE	PORTA&=~BIT(IO_K5)
#define K6_CLOSE	PORTA&=~BIT(IO_K6)
#define K1_FREE		PORTA|=BIT(IO_K1)
#define K2_FREE		PORTA|=BIT(IO_K2)
#define K3_FREE		PORTA|=BIT(IO_K3)
#define K4_FREE		PORTA|=BIT(IO_K4)
#define K5_FREE		PORTA|=BIT(IO_K5)
#define K6_FREE		PORTA|=BIT(IO_K6)

////version 1.1
//#define SET_PUL		K5_CLOSE;
//#define CLR_PUL		K5_FREE;
//version 2.1
#define SET_PUL		K4_CLOSE;
#define CLR_PUL		K4_FREE;



/*!
* @brief:  (全局维护)
*/
extern u32	IsNewPosition;
extern u32 currentPosition;
extern FLAG cmdFlag;
extern PARAMETERS serialParams;
extern EnumDir deviceStatic;


extern void NAK(void);
extern void CAK(void);
void toFwd(void);
void toBack(void);
extern void toStop(void);
//将当前坐标保存至eeprom中
extern void savePosition(void);
//从eeprom中提取坐标
extern u32 getPositionInMemory(void);
extern void toOrign(void);
extern void changeSpeed(void);
extern void ioInit(void);
extern void onceBeep(void);
extern void ReportState(void);
extern void runHandler(void);
extern void runHandlerWithTarget(void);
extern void enPulse(void);
extern void disPulse(void);
extern void humanToForward(void);
extern void humanToBack(void);
extern void reportTarget(void);


extern void wdtInit(void);

extern void keyUp(void);
extern void keyDown(void);
#endif // !DESIGN_H
