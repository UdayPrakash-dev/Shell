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


int parse_pipe(char *input,char **commands){
  int i = 0;
  char *command = strtok(input, "|");

  while(command != NULL && i< MAX_PIPE_COMMANDS -1){
    commands[i++] = command;
    command = strtok(NULL, "|");
  }

  commands[i] = NULL;
  return i;
}
