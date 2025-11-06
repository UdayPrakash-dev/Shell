#include "shell.h"

char *trim_whitespace(char *str) {
    char *end;

    while (isspace((unsigned char)*str)) str++;

    if (*str == 0) {
        return str;
    }

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    *(end + 1) = 0;
    return str;
}

int parse_pipe(char *input, char **commands, char **saveptr) {
    int i = 0;
    char *command = strtok_r(input, "|", saveptr);

    while (command != NULL && i < MAX_PIPE_COMMANDS - 1) {
        commands[i++] = command;
        command = strtok_r(NULL, "|", saveptr);
    }

    commands[i] = NULL;
    return i;
}

void parse_command(char *input, char **args, char **infile, char **outfile, int *append, int *background)
{
    int i = 0;
    *infile = NULL;
    *outfile = NULL;
    *append = 0;
    *background = 0;
    char *saveptr;

    for (int j = 0; j < MAX_ARGS; j++) {
        args[j] = NULL;
    }

    char *command = trim_whitespace(input);

    if (*command == 0) {
        args[0] = NULL;
        return;
    }

    char *token = strtok_r(command, " ", &saveptr);

    while (token != NULL && i < MAX_ARGS - 1)
    {
        if (strcmp(token, "<") == 0)
        {
            token = strtok_r(NULL, " ", &saveptr);
            if (token != NULL) {
                *infile = token;
            } else {
                fprintf(stderr, "shell: syntax error near unexpected token `newline'\n");
                args[0] = NULL;
                return;
            }
        }
        else if (strcmp(token, ">") == 0)
        {
            token = strtok_r(NULL, " ", &saveptr);
            if (token != NULL) {
                *outfile = token;
                *append = 0;
            } else {
                fprintf(stderr, "shell: syntax error near unexpected token `newline'\n");
                args[0] = NULL;
                return;
            }
        }
        else if (strcmp(token, ">>") == 0)
        {
            token = strtok_r(NULL, " ", &saveptr);
            if (token != NULL) {
                *outfile = token;
                *append = 1;
            } else {
                fprintf(stderr, "shell: syntax error near unexpected token `newline'\n");
                args[0] = NULL;
                return;
            }
        }
        else
        {
            args[i++] = token;
        }

        token = strtok_r(NULL, " ", &saveptr);
    }
    args[i] = NULL;

    if (i > 0 && args[i-1] != NULL && strcmp(args[i-1], "&") == 0) {
        *background = 1;
        args[i-1] = NULL;
    }
}
