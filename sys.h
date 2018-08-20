#ifndef _SYS_H__
#define _SYS_H__
#include <iom16v.h>
#include <macros.h>

typedef unsigned char u8;

typedef unsigned int u16;

typedef unsigned long u32;

#define EN_WDR		1

typedef enum
{
	LowSpeed = 0,
	MiddleSpeed,
	HighSpeed
}EnumSpeed;


typedef enum
{
	Forward=1,
	Back,
	Stop
}EnumDir;

typedef enum
{
	HighLevel = 1,
	Pulse,
	Other
}EnumPulseStyle;


typedef struct
{
	u8 flag_run;
	u8 flag_runWithTar;
	u8 flag_toOrign;
	u8 flag_reportState;
	u8 flag_sendPul;
	u8 flag_stopPul;
	u8 flag_connectTest;
	u8 flag_speedChange;
}FLAG;

typedef struct
{
	u32 targetPosition;
	EnumSpeed speedType;
	EnumDir direction;
	EnumPulseStyle pulseStyle;
}PARAMETERS;



//内部1M晶振:熔丝位 E1 D9 FF
//外部8M晶振:熔丝位 FF D9 FF
#define  mclk   8000000

/*
取消JTAGEN
MCUCSR |= 1 << JTD;
MCUCSR |= 1 << JTD;
*/
#define	TRUE		1
#define FALSE		!TRUE



void delay_ms(u16 ms);
void delay_us(u16 us);


#endif