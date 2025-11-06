#ifndef SHELL_H
#define SHELL_H
#include<errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include<fcntl.h>
#include<ctype.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64
#define MAX_PIPE_COMMANDS 67
void read_command(char *input);
int execute_command(char **args, char *infile, char *outfile, int append, int background);
int change_directory(char **args);
void parse_command(char *input, char **args, char **infile, char **outfile, int *append, int *background);
int parse_pipe(char *input, char **commands, char **saveptr);
int execute_pipeline(char **commands, int num_cmds);
void handle_signals(int sig);

#endif
