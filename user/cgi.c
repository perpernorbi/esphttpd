/*
Some random cgi routines. Used in the LED example and the page that returns the entire
flash as a binary. Also handles the hit counter on the main page.
*/

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain
 * this notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */


#include <esp8266.h>
#include "cgi.h"
#include "io.h"
#include "json.h"
#include "ledcgi.h"

void ICACHE_FLASH_ATTR processLedMessage(const char* message, int length)
{
    struct ledCommand cmd;
    cmd = ledCgiParseMessage(message, length);
    switch (cmd.cmd) {
    case LEDCOMMAND_OFF:
        ioLed(0); break;
    case LEDCOMMAND_ON:
        ioLed(1); break;
    case LEDCOMMAND_TOGGLE:
        ioLedToggle(); break;
    case LEDCOMMAND_PWM:
        ioPWM(cmd.pwm); break;
    }
}

int ICACHE_FLASH_ATTR cgiLed(HttpdConnData *connData) {
    if (connData->conn==NULL) {
        //Connection aborted. Clean up.
        return HTTPD_CGI_DONE;
    }

    if (connData->requestType == HTTPD_METHOD_POST)
        processLedMessage(connData->post->buff, connData->post->buffLen);

    httpdStartResponse(connData, 200);
    httpdHeader(connData, "Content-Type", "application/json; charset=utf-8");
    httpdEndHeaders(connData);
    char buff[128];
    ledCgiGetStatusJson(buff);
    httpdSend(connData, buff, -1);
    return HTTPD_CGI_DONE;
}



//Template code for the led page.
int ICACHE_FLASH_ATTR tplIndex(HttpdConnData *connData, char *token, void **arg) {
    char buff[128];
    if (token==NULL) return HTTPD_CGI_DONE;

    os_strcpy(buff, "Unknown");

    if (os_strcmp(token, "devicename") == 0) {
        os_strcpy(buff, "Paris Lamp");
    }
    if (os_strcmp(token, "ledstate")==0) {
        if (ioGetLed()) {
            os_strcpy(buff, "on");
        } else {
            os_strcpy(buff, "off");
        }
    }
    httpdSend(connData, buff, -1);
    return HTTPD_CGI_DONE;
}
