#include "web/handler.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

static const char *respHeaderFmt = "HTTP/1.1 %d %s\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n";

const char *GetStatusMsg(int code) {
    switch (code) {
        case StatusOK: return "OK";
        case StatusNotFound: return "Not Found";
    }
    return "";
}

void HTML(Context *ctx, int code, char *file) {

}

void JPG(Context *ctx, int code, char *file) {

}

void PNG(Context *ctx, int code, char *file) {
    int fd;
    struct stat fileStat;
    char header[128];

    if ((fd = open(file, O_RDONLY)) == -1) {
        sendMsg(ctx, 404, "invalid file");
        return;
    }
    if (fstat(fd, &fileStat) == -1) {
        sendMsg(ctx, 404, "cannot read file info");
        return;
    }
    sprintf(header, respHeaderFmt, code, GetStatusMsg(code), "image/png", fileStat.st_size);
    printf("%s\n", header);
    sendFile(ctx, header, fd);
    close(fd);
}

void sendFile(Context *ctx, char *header, int fd) {
    char buf[1024];
    int readLen;

    write(ctx->sockfd, header, strlen(header));
    while ((readLen = read(fd, buf, sizeof(buf))) > 0) {
        write(ctx->sockfd, buf, readLen);
    }
}

void sendMsg(Context *ctx, int code, char *msg) {
    char header[128];
    sprintf(header, respHeaderFmt, code, GetStatusMsg(code), "text/plain", strlen(msg));
    write(ctx->sockfd, header, strlen(header));
    write(ctx->sockfd, msg, strlen(msg));
}


void cpyHandler(void** des, void* src) {
    *des = src;
}

void freeHandler(void* src) {

}