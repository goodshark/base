#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define CRITICAL -1
#define MAXLINE 4096

int main(int argc, char ** argv) 
{
    FILE * command_p = NULL;
    char command_str[MAXLINE] = "", buf[MAXLINE];
    char * temp_str = NULL;
    int i, n;

    if (argc <= 1) {
        printf("USAGE: %s COMMAND ...\n", argv[0]);
        exit(CRITICAL);
    }
    
    // concate all command into a whole command
    for (i = 1, temp_str = command_str; i < argc; i++) {
        strncpy(temp_str, argv[i], MAXLINE);
        if (temp_str[strlen(temp_str)] == '\0' && i != argc - 1) {
            n = strlen(temp_str);
            temp_str[n++] = ' ';
            temp_str += n;
        }
    }
    printf("command: %s\n", command_str);

    // execute command with popen, and read the result
    command_p = popen(command_str, "r");
    if (command_p == NULL) {
        printf("popen error\n");
        exit(CRITICAL);
    }
    while (fgets(buf, MAXLINE, command_p) != NULL) {
        fputs(buf, stdout);
    }

    pclose(command_p);
    return 0;
}
