#ifndef _EEPROM__H
#define _EEPROM__H
#include "sys.h"

void eepromWrite(u16 address, u8 dat);
u8   eepromRead(u16 address);

#endif // !_EEPROM__H
