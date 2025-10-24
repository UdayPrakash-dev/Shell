#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64

// function prototypes
void read_command(char *input);
void parse_command(char *input, char **args);
void execute_command(char **args);
void change_directory(char **args);

#endif