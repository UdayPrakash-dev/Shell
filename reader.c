#include "shell.h"

void read_command(char *input)
{
    if (fgets(input, MAX_INPUT, stdin) == NULL)
    {
        printf("\n");
        exit(0);
    }
    // remove newline manually
    for (int i = 0; input[i] != '\0'; i++)
    {
        if (input[i] == '\n')
        {
            input[i] = '\0';
            break;
        }
    }
}