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


//Cgi that turns the LED on or off according to the 'led' param in the POST data
int ICACHE_FLASH_ATTR cgiLed(HttpdConnData *connData) {
	int len;
	char buff[1024];
	
	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}

    if (connData->requestType == HTTPD_METHOD_POST) {
        len=httpdFindArg(connData->post->buff, "led", buff, sizeof(buff));
        if (len!=0) {
            switch (buff[0]) {
            case 't':
                ioLedToggle();
                break;
            case '0':
                ioLed(0);
            case '1':
                ioLed(1);
            }
        }
    }
    if (ioGetLed())
        httpdSend(connData, "on", 2);
    else
        httpdSend(connData, "off", 3);
	return HTTPD_CGI_DONE;
}



//Template code for the led page.
int ICACHE_FLASH_ATTR tplIndex(HttpdConnData *connData, char *token, void **arg) {
	char buff[128];
	if (token==NULL) return HTTPD_CGI_DONE;

    os_strcpy(buff, "Unknown");

    if (os_strcmp(token, "devicename") == 0) {
        os_strcpy(buff, "Chengdu Lamp");
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
