#include "shell.h"

#define MAX_INPUT 1024
#define MAX_ARGS 64

static void run_shell_loop(void);

static void start_server(int port) {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket failed\n");
        exit(1);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed\n");
        exit(1);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))) {
        perror("bind failed\n");
        exit(1);
    }
    if (listen(server_fd, 1) < 0) {
        perror("Listen failed\n");
        exit(1);
    }

    printf("Server listening on port %d. Waiting for a client....\n", port);
    client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

    if (client_fd < 0) {
        perror("accept failed\n");
        exit(1);
    }
    printf("Client connected. Redirecting I/O to client.\n");
    fflush(stdout);
    dup2(client_fd, STDIN_FILENO);
    dup2(client_fd, STDOUT_FILENO);
    dup2(client_fd, STDERR_FILENO);

    close(client_fd);
    close(server_fd);

    run_shell_loop();
}

static void start_client(char *host, int port) {
    int client_fd;
    struct sockaddr_in server_addr;
    struct hostent *server;

    server = gethostbyname(host);
    if (server == NULL) {
        fprintf(stderr, "Client: Error, no such host\n");
        exit(1);
    }
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        fprintf(stderr, "Socket failed to open\n");
        exit(1);
    }

    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr_list[0], (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(port);

    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect failed\n");
        exit(1);
    }
    printf("Connected to %s:%d. Type 'exit' to quit.\n", host, port);
    fd_set read_fds;
    char buffer[MAX_INPUT];
    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        FD_SET(client_fd, &read_fds);

        if (select(client_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("Client: select failed\n");
            exit(1);
        }

        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            int n = read(STDIN_FILENO, buffer, MAX_INPUT - 1);
            if (n <= 0) {
                break;
            }
            buffer[n] = '\0';

            if (write(client_fd, buffer, n) < 0) {
                perror("Client: Write to socket failed\n");
                break;
            }
        }

        if (FD_ISSET(client_fd, &read_fds)) {
            int n = read(client_fd, buffer, MAX_INPUT - 1);
            if (n <= 0) {
                printf("Server disconnected\n");
                break;
            }
            buffer[n] = '\0';

            if (write(STDOUT_FILENO, buffer, n) < 0) {
                perror("Client: Write to stdout failed\n");
                break;
            }
        }
    }
    close(client_fd);
}

// **FIXED FUNCTION**
static int execute_command_block(char *command_block) {
    char *args[MAX_ARGS];
    char *commands[MAX_PIPE_COMMANDS];
    int append = 0;
    char *outfile = NULL;
    char *infile = NULL;
    int background;
    int status = 0;

    char *current_and_command = command_block;
    char *next_and_command;
    
    while (current_and_command != NULL && *current_and_command != '\0') {
        if (status != 0) { // Stop executing "&&" chain on failure
            break;
        }

        next_and_command = strstr(current_and_command, "&&");
        if (next_and_command != NULL) {
            *next_and_command = '\0'; // Null-terminate the current command
            next_and_command += 2;    // Move pointer past the "&&"
        }

        char block_copy[MAX_INPUT];
        strncpy(block_copy, current_and_command, MAX_INPUT - 1);
        block_copy[MAX_INPUT - 1] = '\0';

        char *saveptr_pipe;
        int num_cmds = parse_pipe(block_copy, commands, &saveptr_pipe);

        if (commands[0] == NULL || num_cmds == 0) {
            status = 0;
        } else if (num_cmds == 1) {
            parse_command(commands[0], args, &infile, &outfile, &append, &background);
            if (args[0] == NULL) {
                status = 0;
            } else if (strcmp(args[0], "exit") == 0) {
                printf("Bye!\n");
                exit(0);
            } else if (strcmp(args[0], "wait") == 0) {
                while (waitpid(-1, NULL, 0) > 0) { }
                status = 0;
            } else if (strcmp(args[0], "cd") == 0) {
                status = change_directory(args);
            } else {
                status = execute_command(args, infile, outfile, append, background);
            }
        } else if (num_cmds > 1) {
            status = execute_pipeline(commands, num_cmds);
        }
        
        current_and_command = next_and_command;
    }

    return status;
}

// **FIXED FUNCTION**
static void run_shell_loop(void) {
    char input[MAX_INPUT];
    while (1) {
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

        if (input[0] == '\0') {
            continue;
        }

        char input_copy[MAX_INPUT];
        strncpy(input_copy, input, MAX_INPUT - 1);
        input_copy[MAX_INPUT - 1] = '\0';

        char *current_or_command = input_copy;
        char *next_or_command;
        int last_status = 1; // Assume failure to run the first command

        while (current_or_command != NULL && *current_or_command != '\0') {
            if (last_status == 0) { // Stop executing "||" chain on success
                break;
            }

            next_or_command = strstr(current_or_command, "||");
            if (next_or_command != NULL) {
                *next_or_command = '\0'; // Null-terminate the current command
                next_or_command += 2;    // Move pointer past the "||"
            }

            last_status = execute_command_block(current_or_command);
            
            current_or_command = next_or_command;
        }
    }
}

int main(int argc, char *argv[])
{
    signal(SIGCHLD, handle_signals);

    if (argc > 1) {
        if (strcmp(argv[1], "--serve") == 0) {
            if (argc != 3) {
                fprintf(stderr, "Usage: %s --serve <port>\n", argv[0]);
                return 1;
            }
            int port = atoi(argv[2]);
            start_server(port);
        } else if (strcmp(argv[1], "--connect") == 0) {
            if (argc != 4) {
                fprintf(stderr, "Usage: %s --connect <host> <port>\n", argv[0]);
                return 1;
            }
            char *host = argv[2];
            int port = atoi(argv[3]);
            start_client(host, port);
        } else {
            fprintf(stderr, "unknown arguments.\n");
            return 1;
        }
    } else {
        run_shell_loop();
    }
    return 0;
}


