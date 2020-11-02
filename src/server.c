#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#include "web/handler.h"
#include "web/router.h"
#include "map.h"

void waitHandler(int signo) {
	int status;
	pid_t pid;
	pid = wait(&status);
}

int StartServer(Router* r, char *portNum) {
    int listenSocket, handleSocket;
    socklen_t sockAddrLen;
    struct sockaddr_in srvSockAddr, cliSockAddr;

    signal(SIGCHLD, waitHandler);

    if ((listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket error ");
		exit(-1);
	}

    memset(&srvSockAddr, sizeof(srvSockAddr), 0);
	srvSockAddr.sin_family = PF_INET;
	srvSockAddr.sin_port = htons(atoi(portNum)); 
	srvSockAddr.sin_addr.s_addr = htonl(INADDR_ANY); 

	if (bind(listenSocket, (struct sockaddr *)&srvSockAddr, sizeof(struct sockaddr_in)) < 0) {
		perror("bind ");
		exit(-1);
	}
	if (listen(listenSocket, MAX_LISTEN) == -1) {
		perror("listen ");
		exit(-1);
	}

    sockAddrLen = sizeof(struct sockaddr_in);
    while (1) {
        handleSocket = accept(listenSocket, (struct sockaddr *)&cliSockAddr, &sockAddrLen);
        if (handleSocket == -1) {
			switch (errno) {
				case EINTR:
					printf("Interrupt\n");
					continue;
				default: 
					perror("accept ");
			}
			return -1;
		}

        if (fork() == 0) {
			close(listenSocket);
			handleRequest(r, handleSocket);
            exit(0);
		} else {
			close(handleSocket);
		}
    }

}

void handleRequest(Router *r, int sd) {
    while (1) {
        int isClose = 0;
        char *connection = NULL;
        Context ctx = {0, };
        Map *header = NewMap(10);

        if (parseHeader(header, sd) < 0) {
            FreeMap(header, freeString);
            break;
        }
        if (parseBody(header, sd) < 0) {
            FreeMap(header, freeString);
            break;
        }

        MapToRequest(&(ctx.req), header);
        ctx.sockfd = sd;
        
        ServeHTTP(r, &ctx);
        printf("%-7s%-6d%s\n", ctx.req.method, ctx.status, ctx.req.url);

        if (GetData(header, "Connection", (void **)&connection, cpyString) != 1) {
            if (strcmp("close", connection) == 0) 
                isClose = 1;
            free(connection);
        }
        FreeMap(header, freeString);
        FreeRequest(&(ctx.req));
        if (isClose)
            break;
    }
    close(sd);    
}

// 헤더를 읽는 것을 가정
// 헤더의 한줄이 버퍼크기보다 큰 경우는 생각 안하겠음
int readLine(int fd, char *buf) {
    int idx = 0, res;
    char ch;
    
    while (1) {
        res = read(fd, &ch, 1);
        if (res == -1) {
            return -1;
        } else if(res == 0) {
            return 0;
        } else {
            buf[idx++] = ch;
            if (ch == '\n')
                break;
        }
    }
    buf[idx] = '\0';
    return idx;
}

int parseHeader(Map *map, int fd) {
    char buf[2048];
    if (readLine(fd, buf) <= 0) 
        return -1;
    parseHeaderMethod(map, buf);
    while (1) {
        if (readLine(fd, buf) <= 0) 
            return -1;
        if (buf[1] == '\n')
            break;
        parseHeaderMeta(map, buf);
    }
    return 0;
}

int parseHeaderMethod(Map *map, char *buf) {
    char method[10], url[1024], version[10];
    sscanf(buf, "%s %s %[^\r]", method, url, version);
    SetData(map, "Method", method, cpyString, freeString);
    SetData(map, "Url", url, cpyString, freeString);
    SetData(map, "Version", version, cpyString, freeString);

    return 0;
}

int parseHeaderMeta(Map *map, char *buf) {
    char key[64], value[1024];
    sscanf(buf, "%[^:]%*[:]%*[ ]%[^\r]", key, value);
    SetData(map, key, value, cpyString, freeString);
    return 0;
}

int parseBody(Map *map, int fd) {
    char *method, *contentLenStr;
    int contentLen, readLen;

    if (GetData(map, "Method", (void**)&method, cpyString) == -1) {
        return -1;
    }
    if (strcmp(method, "POST") == 0 || strcmp(method, "PUT") == 0) {
        GetData(map, "Content-Length", (void **)&contentLenStr, cpyString);
        contentLen = atoi(contentLenStr);

        char *body = (char *)malloc(contentLen + 1);
        readLen = read(fd, body, contentLen);
        if (readLen <= 0) return -1;
        body[readLen+1] = '\0';
        SetData(map, "Body", body, cpyString, freeString);

        free(body);
        free(contentLenStr);
        free(method);
    }
    return 0;
}