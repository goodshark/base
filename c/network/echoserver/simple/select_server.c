#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>

#define FATAL -1
#define QUEUE 10
#define MAXSIZE 4096
#define PORT 12345
#define FDSIZE 1024

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
    int lsfd, conn_fd, max_fd, fd_array[FDSIZE];
    int i, nbytes;
    char buf[MAXSIZE];
    fd_set rfds, wfds;
    struct timeval tv;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;

    if ( (lsfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
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

    bzero(fd_array, sizeof(int)*FDSIZE);
    max_fd = lsfd;
    while (1) {
        FD_ZERO(&rfds);
        FD_SET(lsfd, &rfds);
        for (i = 0; i < FDSIZE; i++)
            if (fd_array[i] == 1)
                FD_SET(i, &rfds);

        if (select(max_fd+1, &rfds, NULL, NULL, NULL) == -1) {
            printf("select error\n");
            return FATAL;
        }

        if (FD_ISSET(lsfd, &rfds)) {
            if ((conn_fd = accept(lsfd, (struct sockaddr *)&client_addr, &client_len)) == -1) {
                printf("accept error\n");
                return FATAL;
            }
            if (conn_fd > max_fd) {
                max_fd = conn_fd;
                fd_array[conn_fd] = 1;
            }
        }

        for (i = 0; i < FDSIZE; i++) {
            if (fd_array[i] == 1 && FD_ISSET(i, &rfds)) {
                nbytes = custom_read(i, buf);
                if (nbytes == -1) {
                    printf("read error\n");
                    return FATAL;
                } else if (nbytes == 0) {
                    printf("client close\n");
                    fd_array[i] = 0;
                    close(i);
                    continue;
                }
                write(i, buf, nbytes);
            }
        }

    }

    return 0;
}
