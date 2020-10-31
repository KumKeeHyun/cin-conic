#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int ServeHTTP(Router* r, Context* ctx) {
    Map *table;
    node *iter;

    if (GetData(r->table, ctx->req.method, (void**)&table, cpyMap) == -1) {
        // error
        return -1;
    }

    for (int i = 0; i < table->size; i++) {
        for (iter = table->nodes[i]; iter != NULL; iter = iter->next) {
            if (matchUrlPath(iter->key, ctx->req.url, ctx->req.map)) {
                HandlerFunc h = (HandlerFunc)iter->value;
                h(ctx);
                return 0;
            }
        }
    }
    return -1; // TODO : 404
}

int matchUrlPath(char *pattern, char *url, Map *map) {

    return 0;
}
