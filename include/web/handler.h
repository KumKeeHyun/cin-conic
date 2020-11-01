#ifndef _WEB_HANDLER_H
#define _WEB_HANDLER_H

#include "web/http.h"
#include "web/request.h"

typedef struct _Context{
    Request req;
    int sockfd;
    int status;
}Context;

void SendFile(Context *ctx, int code, const char *file);
int checkSuffix(const char *file);
void sendFile(Context *ctx, const char *header, int fd);
void SendMsg(Context *ctx, int code, const char *msg);

typedef void(*HandlerFunc)(Context *);

const char *GetStatusMsg(int code);
void cpyHandler(void** des, void* src);
void freeHandler(void* src);

#endif
