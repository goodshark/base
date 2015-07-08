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
    int fd, cnt, i;
    double num1, num2, result;
    char buf[MAXLINE], opreation;

    if (argc <= 1) {
        printf("USAGE: %s [+|-|*|/]\n", argv[0]);
        exit(CRITICAL);
    }
    opreation = argv[1][0];

    fd = open(FIFO_NAME, O_RDONLY);
    if (fd == -1) {
        printf("open error\n");
        exit(CRITICAL);
    }

    for (i = 0; (cnt = read(fd, buf, MAXLINE)) > 0; i++) {
        if (i % 2 == 0) {
            num1 = atof(buf);
        } else {
            num2 = atof(buf);
            switch (opreation) {
                case '+':
                    result = num1 + num2;
                    break;
                case '-':
                    result = num1 - num2;
                    break;
                case '*':
                    result = num1 * num2;
                    break;
                case '/':
                    result = num1 / num2;
                    break;
                default:
                    printf("opreation error\n");
                    exit(CRITICAL);
            }
            printf("%f %c %f = %f\n", num1, opreation, num2, result);
        }
    }

    if (cnt == -1) {
        printf("read error\n");
        exit(CRITICAL);
    } 

    return 0;
}
