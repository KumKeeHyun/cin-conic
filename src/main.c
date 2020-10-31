#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "web/handler.h"
#include "web/router.h"
#include "web/http.h"
#include "map.h"

void helloWorld(Context *ctx) {
    char *resq = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Lenght: 14\r\n\r\nHello World\r\n";
    write(ctx->sockfd, resq, strlen(resq));
}

void png(Context *ctx) {
    PNG(ctx, StatusOK, "4week-2.PNG");
}

int main(int argc, char *argv[]) {
    Router *r = NewRouter();
    GET(r, "/", helloWorld);
    GET(r, "/image/png", png);

    StartServer(r, "temp");

    return 0;
}

