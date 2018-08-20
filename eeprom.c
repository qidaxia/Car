#include "eeprom.h"
void eepromWrite(u16 address, u8 dat)
{
	/*�ȴ���һ��д��������*/
	while (EECR &BIT(EEWE));
	/*���õ�ַ�����ݼĴ���*/
	EEAR = address;
	EEDR = dat;
	/*��λEEMWE*/
	EECR |= BIT(EEMWE);
	/*��λEEWE������д����*/
	SREG &= ~BIT(7);
	EECR |= BIT(EEWE);
	SREG |= BIT(7);
	delay_ms(1);
	return;
}

u8 eepromRead(u16 address)
{
	while (EECR&BIT(EEWE));
	/*���õ�ַ�Ĵ���*/
	EEAR = address;
	/*����EERE������������*/
	EECR |= BIT(EERE);
	return EEDR;
}

