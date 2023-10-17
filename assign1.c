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

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGUMENTS 30

// Print the info of the process
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void printProcessInfo() {
    pid_t myid = getpid();
    char str[50];
    char comm[50];
    char state;
    int excode, ppid;
    unsigned long utime, stime;
    unsigned long voluntary_ctxt_switches, nonvoluntary_ctxt_switches;

    sprintf(str, "/proc/%d/stat", (int)myid);
    FILE *file = fopen(str, "r");
    if (file == NULL) {
        printf("Error opening stat file: %s\n", str);
        exit(0);
    }

    fscanf(file, "%*d %s %c %d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %lu %lu %*d %*d %*d %*d %*d %d", comm, &state, &ppid, &utime, &stime, &excode);
    fclose(file);

    sprintf(str, "/proc/%d/status", (int)myid);
    file = fopen(str, "r");
    if (file == NULL) {
        printf("Error opening status file: %s\n", str);
        exit(0);
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "voluntary_ctxt_switches", strlen("voluntary_ctxt_switches")) == 0) {
            sscanf(line, "%*s %lu", &voluntary_ctxt_switches);
        } else if (strncmp(line, "nonvoluntary_ctxt_switches", strlen("nonvoluntary_ctxt_switches")) == 0) {
            sscanf(line, "%*s %lu", &nonvoluntary_ctxt_switches);
        }
    }
    fclose(file);

    printf("(PID)%d (CMD)%s (STATE)%c (EXCODE)%d (PPID)%d\n", (int)myid, comm, state, excode, ppid);
    printf("(USER)%0.2lf (SYS)%0.2lf (VCTX)%lu (NVCTX)%lu\n", utime / (double)sysconf(_SC_CLK_TCK), stime / (double)sysconf(_SC_CLK_TCK), voluntary_ctxt_switches, nonvoluntary_ctxt_switches);
}

void handleSignal(int signal) {
    // Do nothing
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
                char *path = getenv("PATH"); //acquire and解析环境变量"PATH"，以查找可执行文件的路径。

                if (path != NULL) {
                    char *dir = strtok(path, ":");
                    while (dir != NULL) {
                        char fullPath[MAX_COMMAND_LENGTH];
                        snprintf(fullPath, sizeof(fullPath), "%s/%s", dir, arguments[0]);
                        if (access(fullPath, X_OK) == 0) {
                            execv(fullPath, arguments);
                        }
                        dir = strtok(NULL, ":");
                    }
                }

                printf("%s: Command not found\n", arguments[0]);
            }

            exit(EXIT_FAILURE);
        } else {
            // Parent process
            waitpid(pid, &status, 0);
            pid_t myid;

            // Print the process info
            printProcessInfo(pid);
        }
    }

    return 0;
}
