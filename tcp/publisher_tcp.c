#include "common.h"
int main(int argc, char **argv) {
    if (argc < 4) { fprintf(stderr, "Uso: %s <host> <puerto> <topic> [n=10]\n", argv[0]); return 1; }
    const char *host = argv[1]; int port = atoi(argv[2]); const char *topic = argv[3];
    int n = (argc >= 5) ? atoi(argv[4]) : 10;
    int fd = socket(AF_INET, SOCK_STREAM, 0); if (fd < 0) die("socket");
    struct sockaddr_in srv = {0}; srv.sin_family = AF_INET; srv.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &srv.sin_addr) <= 0) die("inet_pton");
    if (connect(fd, (struct sockaddr*)&srv, sizeof(srv)) < 0) die("connect");
    printf("PUBLISHER TCP topic=%s sending %d msgs…\n", topic, n);
    for (int i = 1; i <= n; i++) {
        char ts[64]; now_ts(ts, sizeof(ts));
        char line[MAXLINE];
        snprintf(line, sizeof(line), "PUBLISH %s|%d|%s|%s|%s\n", topic, i, ts, "EVENT", "mensaje");
        send(fd, line, strlen(line), 0);
        usleep(200 * 1000);
    }
    close(fd); return 0;
}
