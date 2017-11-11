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
 * { "led": [ 123, 45, 6, 78 ] }
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
                else if (json_retval == JSON_TYPE_ARRAY) {
                    int channel;
                    retval.cmd = LEDCOMMAND_OFF;
                    for (channel = 0; channel < 4; ++channel) {
                        json_retval = jsonparse_next(&json_state);
                        retval.pwm[channel] = jsonparse_get_value_as_int(&json_state);
                        if (retval.pwm[channel] > 0) retval.cmd = LEDCOMMAND_PWM;
                        json_retval = jsonparse_next(&json_state);
                    }
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
uint8_t ioGetLed()
{
    return 0;
}

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

    message = "{ \"led\": [ 123, 45, 6, 78 ] }";
    command = ledCgiParseMessage(message, strlen(message));
    assert(command.pwm[0] == 123);
    assert(command.pwm[1] == 45);
    assert(command.pwm[2] == 6);
    assert(command.pwm[3] == 78);
    assert(command.cmd == LEDCOMMAND_PWM);

    message = "{ \"led\": [ 0, 0, 0, 0 ] }";
    command = ledCgiParseMessage(message, strlen(message));
    assert(command.pwm[0] == 0);
    assert(command.pwm[1] == 0);
    assert(command.pwm[2] == 0);
    assert(command.pwm[3] == 0);
    assert(command.cmd == LEDCOMMAND_OFF);


}
#endif
