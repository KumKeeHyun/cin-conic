#ifndef _WEB_HANDLER_H
#define _WEB_HANDLER_H

#include "web/http.h"
#include "web/request.h"
#include "web/response.h"

typedef struct _Context{
    Request req;
    int sockfd;
}Context;

void HTML(Context *ctx, int code, char *file);
void JPG(Context *ctx, int code, char *file);
void PNG(Context *ctx, int code, char *file);
void sendFile(Context *ctx, char *header, int fd);
void sendMsg(Context *ctx, int code, char *msg);

typedef void(*HandlerFunc)(Context *);

const char *GetStatusMsg(int code);
void cpyHandler(void** des, void* src);
void freeHandler(void* src);

#endif
