// UID: 3035770800
// Student name: Jiang Feiyu
// Development platform: VsCode (SSH connect to workbench2 Linux)
// Remark:
// | Task | Progress |
// | --- | --- |
// | Process creation and execution | Finished |
// | Process creation and execution use of pipe | Finished |
// | Print process’s running statistics| Finished |
// | Use of signals | Finished |
// | Built-in command: exit| Finished |

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
    char cmd[MAX_COMMAND_LENGTH];
} ProcessId;

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

void printProcessInfo(pid_t pid, int i, ProcessInfo* processes) {
    char str[50];
    char comm[50];
    char state;
    int excode, ppid;
    unsigned long utime, stime;
    unsigned long voluntary_ctxt_switches = 0, nonvoluntary_ctxt_switches = 0; // Initialization

    // printf("\npid %d \n", (int)pid);

    /* get my own process statistics */
    sprintf(str, "/proc/%d/stat", (int)pid);
    FILE *file = fopen(str, "r");
    if (file == NULL) {
        printf("Error opening stat file: %s\n", str);
        exit(0);
    }

    // read the file
    fscanf(file, "%*d %s %c %d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu", comm, &state, &ppid, &utime, &stime);

    // close the file
    fclose(file);

    sprintf(str, "/proc/%d/status", (int)pid);
    file = fopen(str, "r");
    if (file == NULL) {
        printf("Error opening status file: %s\n", str);
        exit(0);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "voluntary_ctxt_switches: %lu", &voluntary_ctxt_switches) == 1) {
            break;
        }
    }

    // close the file
    fclose(file);

    ProcessInfo process;
    process.pid = (int)pid;
    strcpy(process.cmd, comm);
    process.state = state;
    process.excode = excode;
    process.ppid = ppid;
    process.user = utime / (double)sysconf(_SC_CLK_TCK);
    process.sys = stime / (double)sysconf(_SC_CLK_TCK);
    process.vctx = voluntary_ctxt_switches;
    process.nvctx = nonvoluntary_ctxt_switches;
    processes[i] = process;
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
    ProcessId processesID[MAX_COMMANDS]; // create an array for saving processes id

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
            // parent process
            processesID[i].pid = pid; // save child process's PID into processesID
            strncpy(processesID[i].cmd, commands[i], MAX_COMMAND_LENGTH); // save command in processesID.cmd
            if (i != command_count - 1) {
                close(pipes[i][1]);
            }
        }
    }

    // After the loop, end the whole process
    for (i = 0; i < command_count; i++) {
        siginfo_t info;
        int status;
        int ret = waitid(P_PID, processesID[i].pid, &info, WNOWAIT | WEXITED);
        if (!ret) {
            // printf("info.si_pid: %d and %d", info.si_pid, (int)i); // print info.si_pid
            printProcessInfo(info.si_pid, i, processes);
        } else {
            perror("JCshell: waitid failed");
            exit(1);
        }
    }

    for (i = 0; i < command_count - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (i = 0; i < command_count; i++) {
        usleep(1000);
        printf("(PID)%d (CMD)%s (STATE)%c (EXCODE)%d (PPID)%d(USER)%0.2lf (SYS)%0.2lf (VCTX)%lu (NVCTX)%lu\n", processes[i].pid, processes[i].cmd, processes[i].state, processes[i].excode, processes[i].ppid, processes[i].user, processes[i].sys, processes[i].vctx, processes[i].nvctx);
    }
}

int main() {
    printf("## JCshell [%d] ## ", getpid()); // Print the process ID of JCshell
    char command[MAX_COMMAND_LENGTH]; // Store the user input command
    pid_t pid; // Child process ID
    int status; // Exit status of the child process
    int childExited = 0; // Flag to indicate if the child process has exited
    const char *regex_pattern = "\\|[^\\s]+\\|"; // Regular expression pattern to match non-whitespace characters between two vertical bars

    // Ignore SIGINT signal (Ctrl+C)
    signal(SIGINT, handleSignal);

    // get the pid of JCshell
    pid_t jcshell_pid = getpid();

    while (1) {
        if (childExited) {
            printf("## JCshell [%d] ## ", jcshell_pid); // Print the JCshell prompt after the child process exits
            childExited = 0;
        }

        fgets(command, sizeof(command), stdin); // Read user command from standard input
        command[strcspn(command, "\n")] = '\0'; // Remove the newline character from the input command

        if (strcmp(command, "exit") == 0) {
            printf("JCshell: Terminated\n"); // User entered "exit", terminate JCshell
            break;
        } else if (strncmp(command, "exit ", strlen("exit ")) == 0) {
            printf("JCshell: \"exit\" with other arguments!!!\n"); // User entered "exit" followed by other arguments, display an error message
            printf("## JCshell [%d] ## ", jcshell_pid);
            continue;
        }

        // Check for two | symbols without in-between command
        if (strstr(command, "||") != NULL || strstr(command, "| |") != NULL) {
            printf("JCshell: should not have two | symbols without in-between command\n");
            printf("## JCshell [%d] ## ", jcshell_pid);
            continue;
        }

        // Check for multiple spaces between | symbols
        regex_t regex;
        int ret = regcomp(&regex, "\\|\\s+\\|", REG_EXTENDED);
        if (ret != 0) {
            fprintf(stderr, "JCshell: regex compilation failed\n");
            printf("## JCshell [%d] ## ", jcshell_pid);
            continue;
        }

        ret = regexec(&regex, command, 0, NULL, 0);
        regfree(&regex);

        if (ret == 0) {
            printf("JCshell: should not have more than one space between | symbols\n");
            printf("## JCshell [%d] ## ", jcshell_pid);
            continue;
        }

        char* commands[MAX_COMMANDS]; // Store individual commands in the pipeline
        int command_count = 0;

        char* token = strtok(command, "|");
        while (token != NULL) {
            // Remove leading and trailing spaces
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

        execute_pipeline(commands, command_count); // Execute the pipeline commands

        // Wait for all child processes to exit
        for (int i = 0; i < command_count; i++) {
            wait(&status);
        }

        childExited = 1;
    }

    return 0;
}