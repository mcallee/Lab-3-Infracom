#include "common.h"
int main(int argc, char **argv) {
    if (argc < 4) { fprintf(stderr, "Uso: %s <host> <puerto> <topic> [n=10]\n", argv[0]); return 1; }
    const char *host = argv[1]; int port = atoi(argv[2]); const char *topic = argv[3];
    int n = (argc >= 5) ? atoi(argv[4]) : 10;
    int sock = socket(AF_INET, SOCK_DGRAM, 0); if (sock < 0) die("socket");
    struct sockaddr_in srv = {0}; srv.sin_family = AF_INET; srv.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &srv.sin_addr) <= 0) die("inet_pton");
    printf("PUBLISHER UDP topic=%s sending %d msgs…\n", topic, n);
    for (int i = 1; i <= n; i++) {
        char ts[64]; now_ts(ts, sizeof(ts)); char msg[MAXLINE];
        snprintf(msg, sizeof(msg), "PUBLISH %s|%d|%s|%s|%s", topic, i, ts, "EVENT", "mensaje");
        sendto(sock, msg, strlen(msg), 0, (struct sockaddr*)&srv, sizeof(srv));
        usleep(120 * 1000);
    }
    return 0;
}
