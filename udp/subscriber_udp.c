#include "common.h"
int main(int argc, char **argv) {
    if (argc != 4) { fprintf(stderr, "Uso: %s <host> <puerto> <topic>\n", argv[0]); return 1; }
    const char *host = argv[1]; int port = atoi(argv[2]); const char *topic = argv[3];
    int sock = socket(AF_INET, SOCK_DGRAM, 0); if (sock < 0) die("socket");
    struct sockaddr_in srv = {0}; srv.sin_family = AF_INET; srv.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &srv.sin_addr) <= 0) die("inet_pton");
    char line[MAXLINE]; snprintf(line, sizeof(line), "SUBSCRIBE %s", topic);
    sendto(sock, line, strlen(line), 0, (struct sockaddr*)&srv, sizeof(srv));
    printf("SUBSCRIBER UDP topic=%s\n", topic);
    while (1) {
        char buf[MAXLINE]; struct sockaddr_in from; socklen_t fromlen = sizeof(from);
        ssize_t r = recvfrom(sock, buf, sizeof(buf)-1, 0, (struct sockaddr*)&from, &fromlen);
        if (r <= 0) continue; buf[r] = '\0'; printf("%s\n", buf); fflush(stdout);
    }
}
