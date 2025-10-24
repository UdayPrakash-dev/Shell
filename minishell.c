#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "shell.h"

#define MAX_INPUT 1024
#define MAX_ARGS 64

// // Function prototypes
// void read_command(char *input);
// void parse_command(char *input, char **args);
// void execute_command(char **args);
// void change_directory(char **args);

int main()
{
    char input[MAX_INPUT];
    char *args[MAX_ARGS];

    while (1)
    {
        // Print prompt
        printf("Enigma> ");
        fflush(stdout);

        // Read input
        read_command(input);

        // Parse input
        parse_command(input, args);

        // Empty input → skip
        if (args[0] == NULL)
            continue;

        // Built-in: exit
        if (strcmp(args[0], "exit") == 0)
        {
            printf("Exiting shell... bye!\n");
            break;
        }

        // Built-in: cd
        if (strcmp(args[0], "cd") == 0)
        {
            change_directory(args);
            continue;
        }

        // Otherwise, execute external command
        execute_command(args);
    }

    return 0;
}
