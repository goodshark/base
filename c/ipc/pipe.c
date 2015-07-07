#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/**********************************************************
  parent process write msg to child process
**********************************************************/

int main(void)
{
    int n, fd[2];
    pid_t pid;
    char line[1024];

    if (pipe(fd) < 0) {
        printf("pipe error\n");
        exit(-1);
    }
    if ((pid = fork()) < 0) {
        printf("fork error\n");
        exit(-1);
    } else if (pid > 0) {
        // parent
        close(fd[0]);
        write(fd[1], "hi, child process\n", 18);
    } else {
        // child
        close(fd[1]);
        n = read(fd[0], line, 1024);
        write(STDOUT_FILENO, line, n);
        //printf("msg: %s", line);
    }
    return 0;
}
