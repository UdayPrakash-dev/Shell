#include "shell.h"

#define MAX_INPUT 1024
#define MAX_ARGS 64

// // Function prototypes
// void read_command(char *input);
// void parse_command(char *input, char **args);
// void execute_command(char **args);
// void change_directory(char **args);
static int execute_command_block(char *command_block,char **saveptr_or){
  char *args[MAX_ARGS];
  char *commands[MAX_PIPE_COMMANDS];
  int append = 0;
  char *outfile = NULL;
  char *infile = NULL;
  int background;
  int status = 0;

  char *saveptr_and;
  char *and_command = strtok_r(command_block,"&&",&saveptr_and);

  while(and_command !=NULL){
    if(status != 0){
      break;
    }
  

  char block_copy[MAX_INPUT];
  strncpy(block_copy,command_block, MAX_INPUT);
  block_copy[MAX_INPUT -1 ] = '\0';

  char *saveptr_pipe;

  int num_cmds = parse_pipe(block_copy, commands,&saveptr_pipe);

  if(commands[0] == NULL || num_cmds == 0){
    return 0;
  }

  if(num_cmds == 1){
    parse_command(commands[0],args,&infile,&outfile,&append,&background);

    if(args[0]==NULL){
      return 0;
    }

    if(strcmp(args[0],"exit")==0){
      printf("Bye!\n");
      exit(0);
    }

    if(strcmp(args[0],"cd")==0){
      return change_directory(args);
    }else if(strcmp(args[0],"wait")==0){
          while(waitpid(-1,NULL,0)>0){}
          status = 0;
    }else{
          status = execute_command(args,infile,outfile,append,background);
    }
  }else if(num_cmds >1){
    status = execute_pipeline(commands, num_cmds);
  }

    and_command = strtok_r(NULL,"&&",&saveptr_and);
  }

  return status;
}
int main()
{
  // char input[MAX_INPUT];
  // char *args[MAX_ARGS];
  // char *commands[MAX_PIPE_COMMANDS];
  // int append = 0;
  // char *outfile = NULL;
  // char *infile = NULL;
  // int background;
  // int status = 0;
  // signal(SIGCHLD,handle_signals);
  // while (1)
  // {
  //   background = 0;
  //   printf("Enigma> ");
  //   fflush(stdout);
  //
  //   read_command(input);
  //
  //   int num_cmds = parse_pipe(input,commands);
  //
  //   if(commands[0]== NULL || num_cmds == 0){
  //     continue; 
  //   } 
  //
  //   if(num_cmds == 1){
  //     parse_command(commands[0],args,&infile,&outfile,&append,&background);
  //
  //     if(args[0]==NULL){
  //       continue;
  //     }
  //
  //     if(strcmp(args[0],"exit")==0){
  //       printf("Bye!\n");
  //       break;
  //     }
  //
  //     if(strcmp(args[0],"wait")==0){
  //       while(waitpid(-1,NULL,0)>0){
  //
  //       }
  //       continue;
  //     }
  //
  //     if(strcmp(args[0],"cd")==0){
  //       change_directory(args);
  //       continue;
  //     }
  //
  //     execute_command(args,infile,outfile,append,background);
  //   }
  //
  //   else if(num_cmds > 1){
  //     execute_pipeline(commands,num_cmds);
  //   }
  // }
  //
  // return 0;
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
  char input[MAX_INPUT];
  signal(SIGCHLD,handle_signals);

  while(1){
    printf("Enigma >");
    fflush(stdout);

    read_command(input);

    if(input[0] == '\0'){
      continue;
    }
    char *saveptr_or;

    char *or_command = strtok_r(input,"||",&saveptr_or);
    int last_status = 1;

    while(or_command !=NULL){

      if(last_status == 0){
        break;
      }

      last_status = execute_command_block(or_command,&saveptr_or);

      or_command = strtok_r(NULL,"||",&saveptr_or);
    }
  }
  return 0;
}

