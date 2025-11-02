#include "shell.h"
//
// void execute_command(char **args)
// {
//     pid_t pid = fork();
//     if (pid < 0)
//     {
//         perror("fork failed");
//         return;
//     }
//     else if (pid == 0)
//     {
//         // Child process
//         if (execvp(args[0], args) == -1)
//         {
//             perror("exec failed");
//         }
//         exit(1);
//     }
//     else
//     {
//         // Parent process
//         int status;
//         waitpid(pid, &status, 0);
//         // printf("\n\n%d\n%d\n\n", status, pid);
//     }
// }
//
void execute_pipeline(char **commands, int num_cmds){
  int fd[2];
  int input_fd = 0; 
  
  for(int i = 0; i < num_cmds; i++){

    if(pipe(fd) == -1){
      perror("pipe failed");
      return;
    }

    char *args[MAX_ARGS];
    char *infile = NULL;
    char *outfile = NULL;
    int append = 0;
    
    parse_command(commands[i], args, &infile, &outfile, &append);

    if (args[0] == NULL) {
        fprintf(stderr, "shell: syntax error in pipe\n");
        close(fd[0]);
        close(fd[1]);
        continue; 
    }

    int pid = fork();
    if(pid == -1){
      perror("fork failed");
      return;
    }

    if(pid == 0){

      if (i == 0 && infile != NULL) {
          int in_fd = open(infile, O_RDONLY);
          if (in_fd == -1) {
              perror("open failed");
              exit(1);
          }
          dup2(in_fd, STDIN_FILENO);
          close(in_fd);
      } else {
          dup2(input_fd, STDIN_FILENO);
      }
      
      if (input_fd != 0) {
          close(input_fd);
      }

      if (i < num_cmds - 1) {
          dup2(fd[1], STDOUT_FILENO);
      } else if (outfile != NULL) {
          int out_fd;
          if (append) {
              out_fd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
          } else {
              out_fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
          }
          if (out_fd == -1) {
              perror("open failed");
              exit(1);
          }
          dup2(out_fd, STDOUT_FILENO);
          close(out_fd);
      }
      close(fd[0]);
      close(fd[1]);
      if(execvp(args[0], args) == -1){
        perror("exec failed");
        exit(1);
      }
    } else {
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
  for(int i = 0; i < num_cmds; i++){
    wait(NULL);
  }
}
void execute_command(char **args,char *infile,char *outfile,int append){
  pid_t pid = fork();
  if(pid == 0){

    if(infile != NULL){
      int in_fd = open(infile,O_RDONLY);
      if(in_fd == -1){
        perror("open failed\n");
        exit(1);
      }
      if(dup2(in_fd,STDIN_FILENO)== -1){
        perror("dup2 failed\n");
        exit(1);
      }
      close(in_fd);
    }
    if(outfile != NULL){
      int out_fd;
      if(append){
        out_fd = open(outfile,O_WRONLY| O_CREAT|O_APPEND,0644);
      }else{
        out_fd = open(outfile,O_WRONLY| O_CREAT | O_TRUNC,0644);
      }
      dup2(out_fd,STDOUT_FILENO);
      close(out_fd);
    }
  if(execvp(args[0],args)== -1){
      perror("Exec failed\n");
    }
    exit(1);
  }else{
    int status;
    waitpid(pid,&status,0);
  }
}
