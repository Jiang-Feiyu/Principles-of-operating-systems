// Development platform: VsCode (SSH connect to workbench2 Linux)
// Remark:

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

void printProcessInfo(pid_t pid) {
    char str[50];
    char comm[50];
    char state;
    int excode, ppid;
    unsigned long utime, stime;
    unsigned long voluntary_ctxt_switches, nonvoluntary_ctxt_switches;

    sprintf(str, "/proc/%d/stat", (int)pid);
    FILE *file = fopen(str, "r");
    if (file == NULL) {
        printf("Error opening stat file: %s\n", str);
        exit(0);
    }

    fscanf(file, "%*d %s %c %d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %lu %lu %*d %*d %*d %*d %*d %*d %*d %*d %*d %d", comm, &state, &ppid, &utime, &stime, &excode);

    fclose(file);

    printf("\n"); // Insert an empty line before process information
    printf("(PID)%d (CMD)%s (STATE)%c (EXCODE)%d (PPID)%d\n", (int)pid, comm, state, excode, ppid);
    printf("(USER)%0.2lf (SYS)%0.2lf (VCTX)%lu (NVCTX)%lu\n", utime / (double)sysconf(_SC_CLK_TCK), stime / (double)sysconf(_SC_CLK_TCK), voluntary_ctxt_switches, nonvoluntary_ctxt_switches);
}

void handleSignal(int signal) {
    // Do nothing
}

void printChildProcessInfo(pid_t pid) {
    int status;

    // This is the parent process
    // Wait for the child process to complete
    waitpid(pid, &status, 0);
}


// Helper function to check if a struct dirent from /proc is a PID folder.
int is_pid_folder(const struct dirent *entry) {
    const char *p;

    for (p = entry->d_name; *p; p++) {
        if (!isdigit(*p))
            return 0;
    }

    return 1;
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
            printf("## JCshell [%d] ## ", jcshell_pid); // print PID
            childExited = 0; // Reset the flag
        }

        fgets(command, MAX_COMMAND_LENGTH, stdin);

        // Remove trailing newline character
        command[strcspn(command, "\n")] = '\0';

        // Exit if the command is "exit"
        char *token = strtok(command, " ");
        if (token != NULL && strcmp(token, "exit") == 0) {
            token = strtok(NULL, " ");
            if (token == NULL) {
                break;
            } else {
                printf("JCshell: \"exit\" with other arguments!!!\n");
                printf("## JCshell [%d] ## ", getpid());
                continue;
            }
        }

        char *arguments[MAX_ARGUMENTS];
        int argCount = 0;

        while (token != NULL && argCount < MAX_ARGUMENTS - 1) {
            arguments[argCount] = token;
            token = strtok(NULL, " ");
            argCount++;
        }

        arguments[argCount] = NULL;

        // Fork a child process
        pid = fork();

        if (pid < 0) {
            printf("Fork failed\n");
            return 1;
        } else if (pid == 0) {
            // This is the child process
            // Execute the command in the child process
            execvp(arguments[0], arguments);

            // If execvp returns, there was an error
            printf("Error executing the command: %s\n", strerror(errno));
            exit(1);
        } else {
            // This is the parent process
            // siginfo_t is only a placeholder here, can also use siginto.si_pid to access pid
            siginfo_t info;
            int status;

            // wait for child to terminate and kept as zombie process
            // 1st param: P_ALL := any child process; P_PID := process specified as 2nd param
            // WNOWAIT: Leave the child in a waitable state;
            //    so that later another wait call can be used to again retrieve the child status information.
            // WEXITED: wait for processes that have exited
            int ret = waitid(P_ALL, 0, &info, WNOWAIT | WEXITED);  
            if (!ret) {
                printProcessInfo(pid);
                waitpid(info.si_pid, &status, 0);
            } else {
                perror("waitid");
            }

            childExited = 1; // Set the flag to indicate that the child process has exited
        }
    }

    return 0;
}
