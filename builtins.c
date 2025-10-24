#include "shell.h"

void change_directory(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "cd: expected argument\n");
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("cd failed");
        }
    }
}
