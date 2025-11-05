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
  char *commands[MAX_PIPE_COMMANDS];
  int append = 0;
  char *outfile = NULL;
  char *infile = NULL;
  int background;
  signal(SIGCHLD,handle_signals);
  while (1)
  {
    background = 0;
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
      printf("Enigma:%s> ", cwd);
    }
    else
    {
      perror("getcwd failed");
    }
    fflush(stdout);

    read_command(input);

    int num_cmds = parse_pipe(input,commands);

    if(commands[0]== NULL || num_cmds == 0){
      continue; 
    } 

    if(num_cmds == 1){
      parse_command(commands[0],args,&infile,&outfile,&append,&background);

      if(args[0]==NULL){
        continue;
      }

      if(strcmp(args[0],"exit")==0){
        printf("Bye!\n");
        break;
      }

      if(strcmp(args[0],"wait")==0){
        while(waitpid(-1,NULL,0)>0){

        }
        continue;
      }

      if(strcmp(args[0],"cd")==0){
        change_directory(args);
        continue;
      }

      execute_command(args,infile,outfile,append,background);
    }

    else if(num_cmds > 1){
      execute_pipeline(commands,num_cmds);
    }
  }

  return 0;
        // Parse input
    //     parse_command(input, args);
    //
    //     // Empty input → skip
    //     if (args[0] == NULL)
    //         continue;
    //
    //     // Built-in: exit
    //     if (strcmp(args[0], "exit") == 0)
    //     {
    //         printf("Exiting shell... bye!\n");
    //         break;
    //     }
    //
    //     // Built-in: cd
    //     if (strcmp(args[0], "cd") == 0)
    //     {
    //         change_directory(args);
    //         continue;
    //     }
    //
    //     // Otherwise, execute external command
    //     execute_command(args);
}

