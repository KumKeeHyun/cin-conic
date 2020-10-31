#include "web/request.h"
#include <stdlib.h>
#include <string.h>

void MapToRequest(Request* req, Map *map) {
    char *value;

    memset(req, sizeof(Request), 0);
    if (GetData(map, "Method", (void**)&value, cpyString) != -1 ) {
        req->method = strMemCpy(value);
        free(value);
    } 
    if (GetData(map, "Url", (void**)&value, cpyString) != -1 ) {
        req->url = strMemCpy(value);
        free(value);
    }
    if (GetData(map, "Version", (void**)&value, cpyString) != -1 ) {
        req->version = strMemCpy(value);
        free(value);
    }
    if (GetData(map, "Host", (void**)&value, cpyString) != -1 ) {
        req->host = strMemCpy(value);
        free(value);
    }
    if (GetData(map, "Connection", (void**)&value, cpyString) != -1 ) {
        req->connection = strMemCpy(value);
        free(value);
    }
    if (GetData(map, "Content-Type", (void**)&value, cpyString) != -1 ) {
        req->contentType = strMemCpy(value);
        free(value);
    }
    if (GetData(map, "Content-Length", (void**)&value, cpyString) != -1 ) {
        req->contentLength = atoi(value);
        free(value);
    }
    if (GetData(map, "Body", (void**)&value, cpyString) != -1 ) {
        req->body = strMemCpy(value);
        free(value);
    } 
    req->map = NewMap(10);
}

void FreeRequest(Request *req) {
    if (req->method != NULL) free(req->method);
    if (req->url != NULL) free(req->url);
    if (req->version != NULL) free(req->version);
    if (req->host != NULL) free(req->host);
    if (req->connection != NULL) free(req->connection);
    if (req->contentType != NULL) free(req->contentType);
    if (req->body != NULL) free(req->body);
    FreeMap(req->map, freeString);
}