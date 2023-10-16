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

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGUMENTS 30

// Print the info of the process
void printProcessInfo(pid_t pid) {
    // Add code here to print process information
}

void handleSignal(int signal) {
    // Do nothing
}

int main() {
    char command[MAX_COMMAND_LENGTH];
    pid_t pid;
    int status;

    // Ignore SIGINT signal (Ctrl+C)
    signal(SIGINT, handleSignal);

    // get the pid of JCshell
    pid_t jcshell_pid = getpid();

    while (1) {
        printf("## JCshell [%d] ## ", jcshell_pid); // print PID
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

        pid = fork();

        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            // If execvp fails, try locating and executing the program using absolute or relative path
            struct stat path_stat;
            if (stat(arguments[0], &path_stat) == 0) {
                if (S_ISDIR(path_stat.st_mode)) { // Check if the path is a directory
                    printf("%s: Is a directory\n", arguments[0]);
                    exit(EXIT_FAILURE);
                }
            }

            if (access(arguments[0], X_OK) == 0) { // chk accessible and executable
                execv(arguments[0], arguments);
            } else if (access(arguments[0], F_OK) == 0) { // chk exists but not executable
                perror(arguments[0]);
            } else {
                char *path = getenv("PATH"); //acquire and save value of $PATH
                if (path != NULL) {
                    char *dir = strtok(path, ":");
                    while (dir != NULL) {
                        char fullPath[MAX_COMMAND_LENGTH];
                        snprintf(fullPath, sizeof(fullPath), "%s/%s", dir, arguments[0]);
                        if (access(fullPath, X_OK) == 0) {
                            execv(fullPath, arguments);
                            break;
                        }
                        dir = strtok(NULL, ":");
                    }
                }
                printf("JCshell: \"%s\": No such file or directory\n", arguments[0]);
            }
            exit(EXIT_FAILURE);
        } else {
            wait(&status);
        }
    }

    return 0;
}
