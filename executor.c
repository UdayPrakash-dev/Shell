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
int execute_pipeline(char **commands, int num_cmds){
  int fd[2];
  int input_fd = 0; 
  pid_t pids[num_cmds];
  
  for(int i = 0; i < num_cmds; i++){

    if(pipe(fd) == -1){
      perror("pipe failed");
      return -1;
    }

    char *args[MAX_ARGS];
    char *infile = NULL;
    char *outfile = NULL;
    int append = 0;
    int background = 0;
    
    parse_command(commands[i], args, &infile, &outfile, &append,&background);

    if (args[0] == NULL) {
        fprintf(stderr, "shell: syntax error in pipe\n");
        close(fd[0]);
        close(fd[1]);
        continue; 
    }

    int pid = fork();
    if(pid == -1){
      perror("fork failed");
      return -1;
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
      pids[i] = pid;
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
  // for(int i = 0; i < num_cmds; i++){
  //   if(waitpid(pids[i],NULL,0)==-1){
  //     if(errno == ECHILD){
  //       break;
  //     }else{
  //       perror("waitpid failed");
  //     }
  //   }
  // }
  int status = 0;
  int last_pid_status = 0;

  for(int i = 0;i<num_cmds;i++){
      if(waitpid(pids[i],&status,0)==-1){
        if(errno == ECHILD){
          break;
        }else{
          perror("waitpid failed");
        }
      }

      if(i == num_cmds - 1){
        if(WIFEXITED(status)){
          last_pid_status = WEXITSTATUS(status);
        }else{
          last_pid_status = 1;
      }
    }
  }
    return last_pid_status;
}
int execute_command(char **args,char *infile,char *outfile,int append,int background){
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
    if(background == 0){
      int status;
      waitpid(pid,&status,0);

      if(WIFEXITED(status)){
        return WEXITSTATUS(status);
      }else{
        return 1;
      }
    }else{
      printf("Process [%d] started in background.\n",pid);
      return 0;
    }
  }

}

void handle_signals(int sig){
  while(waitpid(-1,NULL,WNOHANG)>0){

  }

}
