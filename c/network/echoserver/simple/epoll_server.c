#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>

#define PORT 12345
#define FATAL -1
#define QUEUE 10
#define MAXEVENTS 1024
#define MAXSIZE 4096

int custom_read(int fd, char * buf_ptr)
{
    int nbytes, cnt = 0;
    char ch = '\0';
    while (ch != '\n') {
        nbytes = read(fd, &ch, 1);
        if (nbytes == 0) {
            printf("custom read meet close\n");
            return cnt;
        } else if (nbytes == -1) {
            printf("custom read error\n");
            return FATAL;
        }
        *buf_ptr++ = ch;
        cnt++;
    }
    *buf_ptr++ = ch;
    cnt++;
    return cnt;
}

int main(void)
{
    int lsfd, conn_fd, epfd;
    int i, ep_cnt, nbytes;
    socklen_t client_len;
    char buf[MAXSIZE];
    struct sockaddr_in server_addr, client_addr;
    struct epoll_event ep_event;
    struct epoll_event * ep_ptr;

    if ((lsfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("socket error\n");
        return FATAL;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(lsfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        printf("bind error\n");
        return FATAL;
    }

    if (listen(lsfd, QUEUE) == -1) {
        printf("listen error\n");
        return FATAL;
    }

    if ((epfd = epoll_create(MAXEVENTS)) == -1) {
        perror("epoll_create error\n");
        return FATAL;
    }

    bzero(&ep_event, sizeof(ep_event));
    ep_event.events = EPOLLIN;
    ep_event.data.fd = lsfd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, lsfd, &ep_event) == -1) {
        printf("epoll_ctl error\n");
        return FATAL;
    }

    if ((ep_ptr = calloc(MAXEVENTS, sizeof(struct epoll_event))) == NULL) {
        printf("calloc for epoll events error\n");
        return FATAL;
    }
    while (1) {
        if ((ep_cnt = epoll_wait(epfd, ep_ptr, MAXEVENTS, -1)) == -1) {
            printf("epoll_wait error\n");
            return FATAL;
        }
        for (i = 0; i < ep_cnt; i++) {
            if (ep_ptr[i].data.fd == lsfd) {
                if ((conn_fd = accept(lsfd, (struct sockaddr *)&client_addr, &client_len)) == -1) {
                    printf("accept error\n");
                    return FATAL;
                }
                ep_event.events = EPOLLIN;
                ep_event.data.fd = conn_fd;
                if (epoll_ctl(epfd, EPOLL_CTL_ADD, conn_fd, &ep_event) == -1) {
                        printf("epoll_ctl error\n");
                        return FATAL;
                }
            } else {
                nbytes = custom_read(ep_ptr[i].data.fd, buf);
                if (nbytes == -1) {
                    printf("read error\n");
                    return FATAL;
                } else if (nbytes == 0) {
                    printf("client close\n");
                    close(ep_ptr[i].data.fd);
                }
                write(ep_ptr[i].data.fd, buf, nbytes);
            }
        }
    }

    return 0;
}
