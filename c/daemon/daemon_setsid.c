#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FATAL -1

void func()
{
    while (1) {
        sleep(2);
    }
}

int main(void)
{
    int pid, fd;

    if ((pid = fork()) == -1) {
        perror("fork error\n");
        return FATAL;
    }

    if (pid > 0)
        return 0;
    setsid();

    if ((fd = open("/dev/null", O_RDWR, 0)) == -1) {
        perror("open error\n");
        return FATAL;
    }
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    if (fd > STDERR_FILENO)
        close(fd);

    func();

    return 0;
}
