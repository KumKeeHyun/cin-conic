#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "web/handler.h"
#include "web/router.h"
#include "map.h"

Router* NewRouter() {
    Router *r = (Router*)malloc(sizeof(Router));
    r->table = NewMap(4);
    return r;
}

int GET(Router* r, char *pattern, HandlerFunc h) {
    Map *getTable;
    if (lookup(r->table, "GET") == NULL) {
         getTable = NewMap(10);
        SetData(r->table, "GET", getTable, cpyMap, freeMap);
    }
    GetData(r->table, "GET", (void **)&getTable, cpyMap);
    SetData(getTable, pattern, h, cpyHandler, freeHandler);
    return 0;
}

int POST(Router* r, char *pattern, HandlerFunc h) {
    Map *postTable;
    if (lookup(r->table, "POST") == NULL) {
        postTable = NewMap(10);
        SetData(r->table, "POST", postTable, cpyMap, freeMap);
    }
    GetData(r->table, "POST", (void **)&postTable, cpyMap);
    SetData(postTable, pattern, h, cpyHandler, freeHandler);
    return 0;
}

void printTemp(void *src) {
    printf("%p", src);
}

int ServeHTTP(Router* r, Context* ctx) {
    Map *table;
    node *iter;
    char *errorMsg = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Lenght: 10\r\n\r\n404 Error";

    printf("log: %s\t%s\t%s\n", ctx->req.method, ctx->req.url, ctx->req.version);

    if (GetData(r->table, ctx->req.method, (void**)&table, cpyMap) == -1) {
        goto error;
    }

    for (int i = 0; i < table->size; i++) {
        iter = table->nodes[i];
        for (iter = table->nodes[i]; iter != NULL; iter = iter->next) {
            if (matchUrlPath(iter->key, ctx->req.url, ctx->req.map) == 0) {
                HandlerFunc h = (HandlerFunc)iter->value;
                h(ctx);
                return 0;
            }
        }
    }
error:
    printf("not found\n");
    write(ctx->sockfd, errorMsg, strlen(errorMsg));
    return -1;
}

int matchUrlPath(char *pattern, char *url, Map *map) {
    int patternSliceLen, urlSliceLen, num;
    char *_pattern, *_url;
    char **patternSlice, **urlSlice, *temp;
    patternSliceLen = urlSliceLen = 0;
    
    if (strcmp(pattern, url) == 0) {
        return 0;
    }

    for (char *ch = pattern; *ch != '\0'; ch++)
        if (*ch == '/')
            patternSliceLen++;
    for (char *ch = url; *ch != '\0'; ch++)
        if (*ch == '/')
            urlSliceLen++;
    if (patternSliceLen != urlSliceLen)
        return -1;

    _pattern = strMemCpy(pattern);
    patternSlice = (char **)malloc(sizeof(char**)*(patternSliceLen+1));
    _url = strMemCpy(url);
    urlSlice = (char **)malloc(sizeof(char**)*(patternSliceLen+1));
   
    
    num = 0; temp = strtok(_pattern, "/");
    while (temp != NULL) {
        patternSlice[num++] = temp;
        temp = strtok(NULL, "/");
    }
    num = 0; temp = strtok(_url, "/");
    while (temp != NULL) {
        urlSlice[num++] = temp;
        temp = strtok(NULL, "/");
    }
    
    for (num = 0; num <= patternSliceLen; num++) {
        if (strcmp(patternSlice[num], urlSlice[num]) == 0){
            continue;
        } else if (strlen(patternSlice[num]) > 0 && patternSlice[num][0] == ':') {
            SetData(map, patternSlice[num]+1, urlSlice[num], cpyString, freeString);
        } else {
            free(_pattern);
            free(patternSlice);
            free(_url);
            free(urlSlice);
            return -1;
        }
    }

    free(_pattern);
    free(patternSlice);
    free(_url);
    free(urlSlice);
    return 0;
}
