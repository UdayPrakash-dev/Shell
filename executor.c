#include "shell.h"

void execute_command(char **args)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork failed");
        return;
    }
    else if (pid == 0)
    {
        // Child process
        if (execvp(args[0], args) == -1)
        {
            perror("exec failed");
        }
        exit(1);
    }
    else
    {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        // printf("\n\n%d\n%d\n\n", status, pid);
    }
}
