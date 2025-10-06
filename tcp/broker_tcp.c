#include "common.h"

typedef struct {
    int fd;
    char topic[128];
    int is_sub;
} client_t;

static client_t clients[MAX_CLIENTS];

static void broadcast_to_topic(const char *topic, const char *line, int except_fd) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd > 0 && clients[i].is_sub && strcmp(clients[i].topic, topic) == 0) {
            if (clients[i].fd != except_fd) {
                send(clients[i].fd, line, strlen(line), 0);
            }
        }
    }
}

static void handle_line(int fd, char *line) {
    if (strncmp(line, "SUBSCRIBE ", 10) == 0) {
        char topic[128] = {0};
        sscanf(line + 10, "%127s", topic);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].fd == fd) {
                clients[i].is_sub = 1;
                strncpy(clients[i].topic, topic, sizeof(clients[i].topic)-1);
                char ack[MAXLINE];
                snprintf(ack, sizeof(ack), "OK SUB %s\n", topic);
                send(fd, ack, strlen(ack), 0);
                break;
            }
        }
        return;
    }
    if (strncmp(line, "PUBLISH ", 8) == 0) {
        const char *msg = line + 8;
        char topic[128] = {0};
        const char *bar = strchr(msg, '|');
        if (!bar) return;
        size_t tlen = (size_t)(bar - msg);
        if (tlen >= sizeof(topic)) tlen = sizeof(topic)-1;
        memcpy(topic, msg, tlen);
        broadcast_to_topic(topic, msg, fd);
        return;
    }
}

int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "Uso: %s <puerto>\n", argv[0]); return 1; }
    int port = atoi(argv[1]);

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) die("socket");
    int yes = 1; setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in serv = {0};
    serv.sin_family = AF_INET; serv.sin_addr.s_addr = htonl(INADDR_ANY); serv.sin_port = htons(port);
    if (bind(listenfd, (struct sockaddr*)&serv, sizeof(serv)) < 0) die("bind");
    if (listen(listenfd, 128) < 0) die("listen");

    for (int i = 0; i < MAX_CLIENTS; i++) clients[i].fd = -1;
    fd_set rfds; int maxfd = listenfd;
    printf("broker_tcp escuchando en %d…\n", port);

    while (1) {
        FD_ZERO(&rfds); FD_SET(listenfd, &rfds); maxfd = listenfd;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].fd > 0) { FD_SET(clients[i].fd, &rfds); if (clients[i].fd > maxfd) maxfd = clients[i].fd; }
        }
        int n = select(maxfd+1, &rfds, NULL, NULL, NULL);
        if (n < 0) { if (errno == EINTR) continue; die("select"); }

        if (FD_ISSET(listenfd, &rfds)) {
            struct sockaddr_in cli; socklen_t clilen = sizeof(cli);
            int cfd = accept(listenfd, (struct sockaddr*)&cli, &clilen);
            if (cfd >= 0) {
                int placed = 0;
                for (int i = 0; i < MAX_CLIENTS; i++) if (clients[i].fd <= 0) {
                    clients[i].fd = cfd; clients[i].is_sub = 0; clients[i].topic[0] = '\0'; placed = 1; break;
                }
                if (!placed) close(cfd);
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            int fd = clients[i].fd;
            if (fd > 0 && FD_ISSET(fd, &rfds)) {
                char buf[MAXLINE]; ssize_t r = recv(fd, buf, sizeof(buf)-1, 0);
                if (r <= 0) { close(fd); clients[i].fd = -1; clients[i].topic[0]='\0'; clients[i].is_sub=0; continue; }
                buf[r] = '\0';
                char *saveptr = NULL; char *line = strtok_r(buf, "\n", &saveptr);
                while (line) { char tmp[MAXLINE]; snprintf(tmp, sizeof(tmp), "%s\n", line); handle_line(fd, tmp); line = strtok_r(NULL, "\n", &saveptr); }
            }
        }
    }
}
