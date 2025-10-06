#include "common.h"
int main(int argc, char **argv) {
    if (argc != 4) { fprintf(stderr, "Uso: %s <host> <puerto> <topic>\n", argv[0]); return 1; }
    const char *host = argv[1]; int port = atoi(argv[2]); const char *topic = argv[3];
    int fd = socket(AF_INET, SOCK_STREAM, 0); if (fd < 0) die("socket");
    struct sockaddr_in srv = {0}; srv.sin_family = AF_INET; srv.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &srv.sin_addr) <= 0) die("inet_pton");
    if (connect(fd, (struct sockaddr*)&srv, sizeof(srv)) < 0) die("connect");
    char cmd[MAXLINE]; snprintf(cmd, sizeof(cmd), "SUBSCRIBE %s\n", topic); send(fd, cmd, strlen(cmd), 0);
    printf("SUBSCRIBER TCP topic=%s\n", topic);
    char buf[MAXLINE];
    while (1) { ssize_t r = recv(fd, buf, sizeof(buf)-1, 0); if (r <= 0) break; buf[r] = '\0'; printf("%s", buf); fflush(stdout); }
    close(fd); return 0;
}
