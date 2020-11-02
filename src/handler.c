#include "web/handler.h"
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

static const char *respHeaderFmt = "HTTP/1.1 %d %s\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n";
static const char *suffix[4] = {"text/html", "image/jpg", "image/png", "text/plain"};
static enum {HTML, JPG, PNG, PLAIN,};

const char *GetStatusMsg(int code) {
    switch (code) {
        case StatusOK: return "OK";
        case StatusBadRequest: return "Bad Request";
        case StatusNotFound: return "Not Found";
        case StatusInternalServerError: return "Internal Server Error";
    }
    return "";
}

void SendFile(Context *ctx, int code, const char *file) {
    int fd, suffixID;
    struct stat fileStat;
    char header[128];

    ctx->status = code;
    if ((fd = open(file, O_RDONLY)) == -1) {
        SendMsg(ctx, StatusInternalServerError, "cannot open file");
        return;
    }
    if (fstat(fd, &fileStat) == -1) {
        SendMsg(ctx, StatusInternalServerError, "cannot read file info");
        return;
    }
    if (S_ISDIR(fileStat.st_mode)) {
        SendMsg(ctx, StatusBadRequest, "cannot serve directory");
    } else {
        suffixID = checkSuffix(file);
        sprintf(header, respHeaderFmt, code, GetStatusMsg(code), suffix[suffixID], fileStat.st_size);
        sendFile(ctx, header, fd);
    }
    close(fd);
}

int checkSuffix(const char *file) {
    int idx = -1;
    for (int i = 0; file[i] != '\0'; i++)
        if (file[i] == '.')
            idx = i;
    if (idx == -1) {
        return PLAIN;
    }

    if (strcmp(file+idx+1, "html") == 0 || strcmp(file+idx+1, "HTML") == 0) {
        return HTML;
    } else if (strcmp(file+idx+1, "jpg") == 0 || strcmp(file+idx+1, "JPG") == 0) {
        return JPG;
    } else if (strcmp(file+idx+1, "png") == 0 || strcmp(file+idx+1, "PNG") == 0) {
        return PNG;
    } else {
        return PLAIN;
    }
}

void sendFile(Context *ctx, const char *header, int fd) {
    char buf[1024];
    int readLen;

    write(ctx->sockfd, header, strlen(header));
    while ((readLen = read(fd, buf, sizeof(buf))) > 0) {
        write(ctx->sockfd, buf, readLen);
    }
}

void SendMsg(Context *ctx, int code, const char *msg) {
    char header[128];

    ctx->status = code;
    sprintf(header, respHeaderFmt, code, GetStatusMsg(code), "text/plain", strlen(msg));
    write(ctx->sockfd, header, strlen(header));
    write(ctx->sockfd, msg, strlen(msg));
}


void cpyHandler(void** des, void* src) {
    *des = src;
}

void freeHandler(void* src) {

}