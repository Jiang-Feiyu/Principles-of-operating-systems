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

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGUMENTS 30
#define MAX_COMMANDS 5

void printProcessInfo(pid_t pid) {
    char str[50];
    char comm[50];
    char state;
    int excode, ppid;
    unsigned long utime, stime;
    unsigned long voluntary_ctxt_switches, nonvoluntary_ctxt_switches;

    /* get my own procss statistics */
    sprintf(str, "/proc/%d/stat", (int)pid);
    FILE *file = fopen(str, "r");
    if (file == NULL) {
        printf("Error opening stat file: %s\n", str);
        exit(0);
    }

    // read the file
    fscanf(file, "%*d %s %c %d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %lu %lu %*d %*d %*d %*d %*d %*d %*d %*d %*d %d", comm, &state, &ppid, &utime, &stime, &excode);
    // close the file
    fclose(file);

    printf("\n"); // Insert an empty line before process information
    printf("(PID)%d (CMD)%s (STATE)%c (EXCODE)%d (PPID)%d\n", (int)pid, comm, state, excode, ppid);
    printf("(USER)%0.2lf (SYS)%0.2lf (VCTX)%lu (NVCTX)%lu\n", utime / (double)sysconf(_SC_CLK_TCK), stime / (double)sysconf(_SC_CLK_TCK), voluntary_ctxt_switches, nonvoluntary_ctxt_switches);
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
    perror("JCshell: command execution failed");
    exit(1);
}

void execute_pipeline(char* commands[MAX_COMMANDS], int command_count) {
    int pipes[MAX_COMMANDS - 1][2];
    int i;

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
        }
    }

    for (i = 0; i < command_count - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (i = 0; i < command_count; i++) {
        wait(NULL);
    }
}

int main() {
    printf("## JCshell [%d] ## ", getpid());
    char command[MAX_COMMAND_LENGTH];
    pid_t pid;
    int status;
    int childExited = 0; // Flag to indicate if the child process has exited

    // Ignore SIGINT signal (Ctrl+C)
    signal(SIGINT, handleSignal);

    // get the pid of JCshell
    pid_t jcshell_pid = getpid();

    while (1) {
        if (childExited) {
            printf("\n"); // Add a new line after child process exits
            printf("## JCshell [%d] ## ", jcshell_pid);
            childExited = 0;
        }

        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0'; // Remove trailing newline character

        if (strcmp(command, "exit") == 0) {
            break;
        }

        char* commands[MAX_COMMANDS];
        int command_count = 0;

        char* token = strtok(command, "|");
        while (token != NULL) {
            commands[command_count] = token;
            command_count++;
            token = strtok(NULL, "|");
        }

        execute_pipeline(commands, command_count);

        childExited = 1;
    }

    return 0;
}
