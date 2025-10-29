#include "shell.h"

void execute_command(char **args)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork failed");
        return;
    }
    else if (pid == 0)
    {
        // Child process
        if (execvp(args[0], args) == -1)
        {
            perror("exec failed");
        }
        exit(1);
    }
    else
    {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        // printf("\n\n%d\n%d\n\n", status, pid);
    }
}

// command to execute the pipeline

void execute_pipeline(char **commands, int num_cmds){
  int fd[2];
  int input_fd = 0;
  
  for(int i = 0;i<num_cmds;i++){

    if(pipe(fd)==-1){
      printf("pipe failed\n");
      return;
    }

    int pid = fork();
    if(pid == -1){
      printf("fork failed\n");
      return;
    }
    if(pid == 0){
      dup2(input_fd,STDIN_FILENO);
      close(input_fd);
      if(i<num_cmds -1){
        dup2(fd[1],STDOUT_FILENO);
      }

      close(fd[0]);
      close(fd[1]);

      char *args[MAX_ARGS];
      parse_command(commands[i],args);
      if(execvp(args[0],args)==-1){
        printf("exec failed\n");
        exit(1);
      }
    }else{
      close(fd[1]);

      if(input_fd != 0){
        close(input_fd);
      }

      input_fd = fd[0];
    }
  }


  if(input_fd != 0){
    close(input_fd);
  }
  for(int i = 0;i<num_cmds;i++){
    wait(NULL);
  }
}
