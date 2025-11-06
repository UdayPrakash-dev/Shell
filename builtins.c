#include "shell.h"

int change_directory(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "cd: expected argument\n");
        return 1;
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("cd failed");
            return 1;
        }
    }
  return 0;
}
