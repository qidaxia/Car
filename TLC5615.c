#include "TLC5615.h"
#include "hook.h"
#include "design.h"

extern void SPI_MasterInit(void)
{
	DDRB |= BIT(SPI_MOSI);
	DDRB |= BIT(SPI_SCK);
	DDRB |= BIT(TLC5615_CS);
	/*!<使能spi主机模式，速率fck/16*/
	SPCR = BIT(SPE) | BIT(MSTR) | BIT(SPR0);
}
static  void SPI_MasterTransmit(u16 iData)
{
	iData <<= 2;
	TLC5615_ENABLE;
	SPDR = (iData >> 8);
	while (!(SPSR&BIT(SPIF)));
	SPDR = iData;
	while (!(SPSR&BIT(SPIF)));
	TLC5615_DISABLE;
}
extern void toHighSpeed(void)
{
	SPI_MasterTransmit(950);
}
extern void toMiddleSpeed(void)
{
	SPI_MasterTransmit(620);
}
extern void toLowSpeed(void)
{
	SPI_MasterTransmit(310);
}
extern void toZeroSpeed(void)
{
	SPI_MasterTransmit(0);
}


