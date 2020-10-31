#ifndef _WEB_ROUTER_H
#define _WEB_ROUTER_H

#include "map.h"
#include "handler.h"

#define MAX_LISTEN 10

typedef struct _Router{
    Map *table;
} Router;


Router* NewRouter();
int GET(Router* r, char *pattern, HandlerFunc h);
int POST(Router* r, char *pattern, HandlerFunc h);
int ServeHTTP(Router* r, Context* ctx);
int matchUrlPath(char *pattern, char *url, Map *map);

int StartServer(Router* r, char *addr);
void handleRequest(Router *r, int sd);
int readLine(int fd, char *buf);
int parseHeader(Map *map, int fd);
int parseHeaderMethod(Map *map, char *buf);
int parseHeaderMeta(Map *map, char *buf);
int parseBody(Map *map, int fd);

#endif