#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <strings.h>
#include <netinet/in.h>

#define FATAL -1
#define PORT 12345
#define QUEUE 10
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
    int ls_fd, conn_fd;
    int nbytes;
    struct sockaddr_in server_addr, client_addr;
    char buf[MAXSIZE], * buf_ptr;
    socklen_t client_len;

    if ( (ls_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("socket error\n");
        return FATAL;
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(ls_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        printf("bind error\n");
        return FATAL;
    }

    if (listen(ls_fd, QUEUE) == -1) {
        printf("listen error\n");
        return FATAL;
    }

    while (1) {
        conn_fd = accept(ls_fd, (struct sockaddr *)&client_addr, &client_len);
        if (conn_fd == -1) {
            printf("accept error\n");
            return FATAL;
        }
        /*
        for (buf_ptr = buf; (nbytes = read(conn_fd, buf_ptr, MAXSIZE)) > 0; buf_ptr += nbytes) {
            ;
        }
        */
        while (1) {
            nbytes = custom_read(conn_fd, buf);
            if (nbytes == -1) {
                printf("read error\n");
                return FATAL;
            } else if (nbytes == 0) {
                printf("client close conn\n");
                close(conn_fd);
                break;
            }
            write(conn_fd, buf, nbytes);
        }
    }

    return 0;
}
