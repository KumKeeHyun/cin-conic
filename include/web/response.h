#ifndef _WEB_RESPONSE_H
#define _WEB_RESPONSE_H

typedef struct _Response{
    char *version;
    int status;
    char *connection;
    char *contentType;
    int contentLength;
} Response;

enum ResponseType {
    Version = 0,
    Status,
    Connection,
    ContentType,
    ContentLength,
};

int SetResponse(Response* resp, enum ResponseType type, void* data);
void FreeResponse(Response* resp);

#endif