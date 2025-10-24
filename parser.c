#include "shell.h"

void parse_command(char *input, char **args)
{
    char *token = strtok(input, " ");
    int i = 0;
    while (token != NULL && i < MAX_ARGS - 1)
    {
        args[i++] = token;
        // printf("%p\n\n", token);
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
    // for (int i = 0; i < sizeof(args) / sizeof(args[0]) + 1; i++)
    // {
    //     printf("\n%s\n\n", args[i]);
    // }
}