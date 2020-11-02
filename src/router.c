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
    printf("%-13s %s\n", "GET", pattern);
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
    printf("%-13s %s\n", "POST", pattern);
    return 0;
}

void printTemp(void *src) {
    printf("%p", src);
}

int ServeHTTP(Router* r, Context* ctx) {
    Map *table;
    node *iter;

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
    ctx->status = StatusNotFound;
    SendMsg(ctx, StatusNotFound, "404 error");
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
    
    for (int i = 0; i < num; i++) {
        if (patternSlice[i] == NULL || urlSlice[i] == NULL) {
            if (patternSlice[i] == urlSlice[i]) {
                continue;
            } else {
                free(_pattern);
                free(patternSlice);
                free(_url);
                free(urlSlice);
                return -1;
            }
        } else if (strcmp(patternSlice[i], urlSlice[i]) == 0){
            continue;
        } else if (strlen(patternSlice[i]) > 0 && patternSlice[i][0] == ':') {
            SetData(map, patternSlice[i]+1, urlSlice[i], cpyString, freeString);
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

// 디렉토리 문자열 앞, 뒤로 "/"가 없어야함
int Static(Router* r, const char *dir) {
    char *pattern;
    const char *patternFmt = "/%s/:filename";
    
    pattern = (char *)malloc(strlen(dir) + 12); // "/$(dir)/:filename"
    sprintf(pattern, patternFmt, dir);
    GET(r, pattern, staticHandler);
    free(pattern);
    return 0;
}

void staticHandler(Context *ctx) {
    char *filename;
    if (GetData(ctx->req.map, "filename", (void**)&filename, cpyString) >= 0) {
        if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0) {
            SendMsg(ctx, StatusBadRequest, "invalid filename");
        } else {
            SendFile(ctx, StatusOK, ctx->req.url+1);
        }
        free(filename);
    } else {
        SendMsg(ctx, StatusInternalServerError, "param error");
    }

}