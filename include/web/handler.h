#ifndef _WEB_HANDLER_H
#define _WEB_HANDLER_H

#include "web/request.h"
#include "web/response.h"

typedef struct _Context{
    Request req;
    int sockfd;
}Context;

typedef void(*HandlerFunc)(Context *);

void cpyHandler(void** des, void* src);
void freeHandler(void* src);

#endif
