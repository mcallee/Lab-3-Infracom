#include "common.h"
typedef struct { struct sockaddr_in addr; char topic[128]; int in_use; } sub_t;
static sub_t subs[MAX_CLIENTS];

static void add_subscriber(const struct sockaddr_in *cli, const char *topic) {
    for (int i = 0; i < MAX_CLIENTS; i++) if (!subs[i].in_use) {
        subs[i].in_use = 1; subs[i].addr = *cli; strncpy(subs[i].topic, topic, sizeof(subs[i].topic)-1); return;
    }
}
static void fanout(int sock, const char *topic, const char *msg) {
    for (int i = 0; i < MAX_CLIENTS; i++) if (subs[i].in_use && strcmp(subs[i].topic, topic) == 0) {
        sendto(sock, msg, strlen(msg), 0, (struct sockaddr*)&subs[i].addr, sizeof(subs[i].addr));
    }
}
int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "Uso: %s <puerto>\n", argv[0]); return 1; }
    int port = atoi(argv[1]); int sock = socket(AF_INET, SOCK_DGRAM, 0); if (sock < 0) die("socket");
    struct sockaddr_in serv = {0}; serv.sin_family = AF_INET; serv.sin_addr.s_addr = htonl(INADDR_ANY); serv.sin_port = htons(port);
    if (bind(sock, (struct sockaddr*)&serv, sizeof(serv)) < 0) die("bind");
    printf("broker_udp escuchando en %d…\n", port);
    char buf[MAXLINE];
    while (1) {
        struct sockaddr_in cli; socklen_t clilen = sizeof(cli);
        ssize_t r = recvfrom(sock, buf, sizeof(buf)-1, 0, (struct sockaddr*)&cli, &clilen);
        if (r < 0) continue; buf[r] = '\0';
        if (strncmp(buf, "SUBSCRIBE ", 10) == 0) {
            char topic[128] = {0}; sscanf(buf + 10, "%127s", topic);
            add_subscriber(&cli, topic); const char *ack = "OK SUB\n";
            sendto(sock, ack, strlen(ack), 0, (struct sockaddr*)&cli, clilen);
        } else if (strncmp(buf, "PUBLISH ", 8) == 0) {
            const char *msg = buf + 8; char topic[128] = {0};
            const char *bar = strchr(msg, '|'); if (!bar) continue;
            size_t tlen = (size_t)(bar - msg); if (tlen >= sizeof(topic)) tlen = sizeof(topic)-1;
            memcpy(topic, msg, tlen); fanout(sock, topic, msg);
        }
    }
}
