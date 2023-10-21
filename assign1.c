#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include <regex.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGUMENTS 30
#define MAX_COMMANDS 5

typedef struct {
    pid_t pid;
    char cmd[50];
    char state;
    int excode;
    int ppid;
    double user;
    double sys;
    unsigned long vctx;
    unsigned long nvctx;
} ProcessInfo;

void printProcessInfo(ProcessInfo process) {
    printf("\n");
    printf("(PID)%d (CMD)%s (STATE)%c (EXCODE)%d (PPID)%d\n", process.pid, process.cmd, process.state, process.excode, process.ppid);
    printf("(USER)%0.2lf (SYS)%0.2lf (VCTX)%lu (NVCTX)%lu\n", process.user, process.sys, process.vctx, process.nvctx);
}

void handleSignal(int signal) {
    // Do nothing
}

void execute_command(char* command) {
    char* args[MAX_ARGUMENTS];
    int arg_count = 0;

    char* token = strtok(command, " ");
    while (token != NULL) {
        args[arg_count] = token;
        arg_count++;
        token = strtok(NULL, " ");
    }

    args[arg_count] = NULL;

    execvp(args[0], args);
    struct stat path_stat;
    if (stat(args[0], &path_stat) == 0) {
        if (S_ISDIR(path_stat.st_mode)) {
            fprintf(stderr, "%s: Is a directory\n", args[0]);
            exit(1);
        } else if (access(args[0], F_OK) == -1) {
            fprintf(stderr, "JCshell: '%s': No such file or directory\n", args[0]);
            exit(1);
        } else if (access(args[0], X_OK) == -1) {
            fprintf(stderr, "%s: Permission denied\n", args[0]);
            exit(1);
        }
    } else {
        fprintf(stderr, "JCshell: '%s': No such file or directory\n", args[0]);
        exit(1);    
    }
    exit(1);
}

void execute_pipeline(char* commands[MAX_COMMANDS], int command_count) {
    int pipes[MAX_COMMANDS - 1][2];
    int i;

    ProcessInfo processes[MAX_COMMANDS]; // create an array for saving processes

    for (i = 0; i < command_count - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("JCshell: pipe creation failed");
            exit(1);
        }
    }

    for (i = 0; i < command_count; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("JCshell: fork failed");
            exit(1);
        } else if (pid == 0) {
            if (i != 0) {
                if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1) {
                    perror("JCshell: dup2 failed");
                    exit(1);
                }
            }

            if (i != command_count - 1) {
                if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                    perror("JCshell: dup2 failed");
                    exit(1);
                }
            }

            int j;
            for (j = 0; j < command_count - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            execute_command(commands[i]);
        } else {
            // 父进程保存子进程的信息
            ProcessInfo process;
            process.pid = pid;
            strncpy(process.cmd, commands[i], sizeof(process.cmd));
            process.cmd[sizeof(process.cmd) - 1] = '\0';
            process.state = 'Z'; // 所有子进程初始状态设为Z
            process.excode = 0;
            process.ppid = getpid();
            process.user = 0.0;
            process.sys = 0.0;
            process.vctx = 0;
            process.nvctx = 0;
            processes[i] = process;
        }
    }

    for (i = 0; i < command_count - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (i = 0; i < command_count; i++) {
        waitpid(processes[i].pid, NULL, 0); // 等待子进程结束
        usleep(1000);
        printProcessInfo(processes[i]); // 输出子进程信息
    }
}

int main() {
    printf("## JCshell [%d] ## ", getpid());
    char command[MAX_COMMAND_LENGTH];
    pid_t pid;
    int status;
    int childExited = 0; // Flag to indicate if the child process has exited
    const char *regex_pattern = "\\|[^\\s]+\\|"; // Pattern to match | followed by one or more non-space characters and then another |

    // Ignore SIGINT signal (Ctrl+C)
    signal(SIGINT, handleSignal);

    // get the pid of JCshell
    pid_t jcshell_pid = getpid();

    while (1) {
        if (childExited) {
            printf("## JCshell [%d] ## ", jcshell_pid);
            childExited = 0;
        }

        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0'; // Remove trailing newline character

        if (strcmp(command, "exit") == 0) {
            break;
        } else if (strncmp(command, "exit ", strlen("exit ")) == 0) {
            printf("JCshell: \"exit\" with other arguments!!!\n");
            printf("## JCshell [%d] ## ", jcshell_pid);
            continue;
        }

        // Check for two | symbols without in-between command
        if (strstr(command, "||") != NULL || strstr(command, "| |") != NULL) {
            printf("JCshell: should not have two | symbols without in-between command\n");
            printf("## JCshell [%d] ## ", jcshell_pid);
            continue;
        }

        char* commands[MAX_COMMANDS];
        int command_count = 0;

        char* token = strtok(command, "|");
        while (token != NULL) {
            // Trim leading and trailing spaces
            char* trimmed_token = token;
            while (isspace(*trimmed_token)) {
                trimmed_token++;
            }
            int len = strlen(trimmed_token);
            while (len > 0 && isspace(trimmed_token[len - 1])) {
                trimmed_token[--len] = '\0';
            }

            commands[command_count] = trimmed_token;
            command_count++;
            token = strtok(NULL, "|");
        }

        execute_pipeline(commands, command_count);

        // Wait for all child processes to exit
        for (int i = 0; i < command_count; i++) {
            wait(&status);
        }

        childExited = 1;
    }

    return 0;
}
