#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "web/handler.h"
#include "web/router.h"
#include "web/http.h"
#include "map.h"

void helloWorld(Context *ctx) {
    SendMsg(ctx, StatusOK, "Hello World");
}

void png(Context *ctx) {
    SendFile(ctx, StatusOK, "view/img/gin-gonic.png");
}

void param(Context *ctx) {
    char *arg;
    if (GetData(ctx->req.map, "arg", (void**)&arg, cpyString) >= 0) {
        SendMsg(ctx, StatusOK, arg);
        free(arg);
    } else {
        SendMsg(ctx, StatusInternalServerError, "param error");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: %s <portNum>\n", argv[0]);
        return -1;
    }
    Router *r = NewRouter();
    Static(r, "view");
    Static(r, "view/img");

    GET(r, "/", helloWorld);
    GET(r, "/image/png", png);
    GET(r, "/param/:arg", param);

    StartServer(r, argv[1]);

    return 0;
}

