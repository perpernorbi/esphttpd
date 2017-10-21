#ifndef LEDCGI_H
#define LEDCGI_H

#include <stdint.h>
#ifdef TEST
#else
#include <esp8266.h>
#endif


#define LEDCOMMAND_OFF 0
#define LEDCOMMAND_ON 1
#define LEDCOMMAND_TOGGLE 2
#define LEDCOMMAND_RGB 3

struct ledCommand {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t cmd;
};

struct ledCommand ICACHE_FLASH_ATTR ledCgiParseMessage(const char *message, int len);
void ICACHE_FLASH_ATTR ledCgiGetStatusJson(char *buff);

#endif // LEDCGI_H
