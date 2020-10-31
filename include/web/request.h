#ifndef _WEB_REQUEST_H
#define _WEB_REQUEST_H

#include "map.h"

typedef struct _Request{
    char *method;
    char *url;
    char *version;
    char *host;
    char *connection;
    char *contentType;
    int contentLength;
    char *body;
    Map *map;
} Request;

void MapToRequest(Request* req, Map *map);
void FreeRequest(Request *req);

#endif