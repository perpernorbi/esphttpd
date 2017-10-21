#ifdef TEST
#include <stdio.h>
#include <assert.h>
#include <string.h>
#else
#include <esp8266.h>
#endif

#include "ledcgi.h"
#include "io.h"
#include "jsonparse/jsonparse.h"



/*
 * { "led": "on" }
 *
 * { "led": "off" }
 *
 * { "led": "toggle" }
 *
 * { "led": { "r": 123, "g": 45, "b": 6 } }
 *
 */

static struct jsonparse_state json_state;

int ICACHE_FLASH_ATTR errorOrEndOfObject(int state)
{
    return (state == JSON_TYPE_ERROR) || (state == JSON_TYPE_OBJECT_END);
}


struct ledCommand ICACHE_FLASH_ATTR ledCgiParseMessage(const char *message, int len)
{
    struct ledCommand retval;
    int json_retval;

    jsonparse_setup(&json_state, message, len);
    while (! errorOrEndOfObject(json_retval = jsonparse_next(&json_state))) {
        if (json_retval == JSON_TYPE_PAIR_NAME) {
            if (! jsonparse_strcmp_value(&json_state, "led")) {
                json_retval = jsonparse_next(&json_state);
                if (json_retval == JSON_TYPE_STRING) {
                    if (! jsonparse_strcmp_value(&json_state, "on"))
                        retval.cmd = LEDCOMMAND_ON;
                    else if (! jsonparse_strcmp_value(&json_state, "off"))
                        retval.cmd = LEDCOMMAND_OFF;
                    else if (! jsonparse_strcmp_value(&json_state, "toggle"))
                        retval.cmd = LEDCOMMAND_TOGGLE;
                }
                else if (json_retval == JSON_TYPE_OBJECT) {
                    while (! errorOrEndOfObject(json_retval = jsonparse_next(&json_state))) {
                        if (json_retval == JSON_TYPE_PAIR_NAME) {
                            char color[10];
                            jsonparse_copy_value(&json_state, color, 10);
                            json_retval = jsonparse_next(&json_state);
                            switch (color[0]) {
                            case 'r':
                                retval.r = jsonparse_get_value_as_int(&json_state); break;
                            case 'g':
                                retval.g = jsonparse_get_value_as_int(&json_state); break;
                            case 'b':
                                retval.b = jsonparse_get_value_as_int(&json_state); break;
                            }
                        }
                    }
                    if ((retval.r > 0) || (retval.g > 0) || (retval.b) > 0)
                        retval.cmd = LEDCOMMAND_RGB;
                    else
                        retval.cmd = LEDCOMMAND_OFF;
                }
            }
        }
    }
    return retval;
}

void ICACHE_FLASH_ATTR ledCgiGetStatusJson(char *buff)
{
    uint8_t ledStatus = ioGetLed();
    os_sprintf(buff, "{\"led\":\"%s\"}", (ledStatus) ? "on" : "off" );
}



#ifdef TEST
int main()
{
    struct ledCommand command;
    const char * message;

    message = "{ \"led\": \"on\" }";
    command = ledCgiParseMessage(message, strlen(message));
    assert(command.cmd == LEDCOMMAND_ON);

    message = "{ \"led\": \"off\" }";
    command = ledCgiParseMessage(message, strlen(message));
    assert(command.cmd == LEDCOMMAND_OFF);

    message = "{ \"led\": \"toggle\" }";
    command = ledCgiParseMessage(message, strlen(message));
    assert(command.cmd == LEDCOMMAND_TOGGLE);

    message = "{ \"led\": { \"r\": 123, \"g\": 45, \"b\": 6 } }";
    command = ledCgiParseMessage(message, strlen(message));
    assert(command.r == 123);
    assert(command.g == 45);
    assert(command.b == 6);
    assert(command.cmd == LEDCOMMAND_RGB);

    message = "{ \"led\": { \"r\": 0, \"g\": 0, \"b\": 0 } }";
    command = ledCgiParseMessage(message, strlen(message));
    assert(command.r == 0);
    assert(command.g == 0);
    assert(command.b == 0);
    assert(command.cmd == LEDCOMMAND_OFF);


}
#endif
