#ifndef CGI_H
#define CGI_H

#include "httpd.h"

int cgiLed(HttpdConnData *connData);
int tplIndex(HttpdConnData *connData, char *token, void **arg);

#endif
