#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>

#define CRITICAL -1
#define FIFO_NAME "/tmp/test_only_foo"
#define MAXLINE 4096

int main(int argc, char ** argv)
{
    int cnt, i, num1, num2;
    int fd;
    char buf[MAXLINE];

    if (argc <= 1) {
        printf("USAGE: %s NUMBER_PAIRS\n", argv[0]);
        exit(CRITICAL);
    }
    cnt = atoi(argv[1]);

    if (mkfifo(FIFO_NAME, 0644) < 0) {
        printf("mkfifo error\n");
        exit(CRITICAL);
    }

    fd = open(FIFO_NAME, O_WRONLY);
    if (fd < 0) {
        printf("open error\n");
        exit(CRITICAL);
    }

    srand(time(NULL));
    for (i = 0; i < cnt * 2; i++) {
        snprintf(buf, MAXLINE, "%d", rand());
        printf("number: %s\n", buf);
        if (i % 2 != 0)
            printf("\n");
        write(fd, buf, MAXLINE);
    }

    /*
    if (remove(FIFO_NAME) == -1) {
        printf("remove error\n");
        exit(CRITICAL);
    }
    */
    return 0;
}
