#ifndef IO_H
#define IO_H

void ICACHE_FLASH_ATTR ioLed(int ena);
void ICACHE_FLASH_ATTR ioLedToggle();
uint8_t ICACHE_FLASH_ATTR ioGetLed();
void ioInit(void);

#endif
