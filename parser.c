#include "shell.h"

//
// void parse_command(char *input, char **args)
// {
//     char *token = strtok(input, " ");
//     int i = 0;
//     while (token != NULL && i < MAX_ARGS - 1)
//     {
//         args[i++] = token;
//         // printf("%p\n\n", token);
//         token = strtok(NULL, " ");
//     }
//     args[i] = NULL;
//     // for (int i = 0; i < sizeof(args) / sizeof(args[0]) + 1; i++)
//     // {
//     //     printf("\n%s\n\n", args[i]);
//     // }
// }
//
//
char *trim_whitespace(char *str) {
    char *end;

    
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0) 
        return str;

    
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    
    *(end+1) = 0;

    return str;
}
int parse_pipe(char *input, char **commands) {
    int i = 0;
    char *command = strtok(input, "|");

    while (command != NULL && i < MAX_PIPE_COMMANDS - 1) {
        commands[i++] = command; 
        command = strtok(NULL, "|");
    }

    commands[i] = NULL;
    return i; 
}
void parse_command(char *input, char **args, char **infile, char **outfile, int *append)
{
    int i = 0;
    *infile = NULL;
    *outfile = NULL;
    *append = 0;

    
    char *command = trim_whitespace(input);

    if (*command == 0) { 
        args[0] = NULL;
        return;
    }

    char *token = strtok(command, " ");
    
    while (token != NULL && i < MAX_ARGS - 1)
    {
        if (strcmp(token, "<") == 0)
        {
            token = strtok(NULL, " "); 
            if (token != NULL) {
                *infile = token;
            } else {
                fprintf(stderr, "shell: syntax error near unexpected token `newline'\n");
                args[0] = NULL; return;
            }
        }
        else if (strcmp(token, ">") == 0)
        {
            token = strtok(NULL, " "); 
            if (token != NULL) {
                *outfile = token;
                *append = 0; 
            } else {
                fprintf(stderr, "shell: syntax error near unexpected token `newline'\n");
                args[0] = NULL; return;
            }
        }
        else if (strcmp(token, ">>") == 0)
        {
            token = strtok(NULL, " "); 
            if (token != NULL) {
                *outfile = token;
                *append = 1;
            } else {
                fprintf(stderr, "shell: syntax error near unexpected token `newline'\n");
                args[0] = NULL; return;
            }
        }
        else
        {
            
            args[i++] = token;
        }
        
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
}
