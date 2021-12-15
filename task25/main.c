#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

void printStat(pid_t pid, int stat)
{
    printf("Process status - %d\n", stat);
    if(WCOREDUMP(stat)) {
        printf("Process %d ended with core dump %d\n", pid, WCOREDUMP(stat));
    }
    if(WEXITSTATUS(stat)) {
        printf("Process %d ended with exit code %d\n", pid, WEXITSTATUS(stat));
    }
    if(WIFCONTINUED(stat)) {
        printf("Process %d ended with continued %d\n", pid, WIFCONTINUED(stat));
    }
    if(WIFEXITED(stat)) {
        printf("Process %d terminated normally with %d\n", pid, WIFEXITED(stat));
    }
    if(WIFSIGNALED(stat)) {
        printf("Process %d terminated due to signal %d\n", pid, WIFSIGNALED(stat));
    }
    if(WIFSTOPPED(stat)) {
        printf("Process %d currently stopped\n", pid);
    }
    if(WSTOPSIG(stat)) {
        printf("Process %d stopped due to signal %d\n", pid, WSTOPSIG(stat));
    }
    if(WTERMSIG(stat)) {
        printf("Process %d terminated due to signal %d\n", pid, WTERMSIG(stat));
    }
}

void close_pipe(int pipes[2]) {
    int close_status1, close_status2;
    close_status1 = close(pipes[0]);
    close_status2 = close(pipes[1]);
    if (close_status1 == -1 || close_status2  == -1) {
        perror("Failed to close pipe\n");
    }
}

int main(int argc, char* argv[]) {
    if(argc != 2){
        printf("Usage: ./a.out {Your_Text}\n"
               "Max size of your text should <= %d bytes\n", BUFSIZ);
        return 1;
    }
    char buffer[BUFSIZ];

    memcpy(buffer, argv[1], BUFSIZ);

    int my_pipe[2];

    int pipe_status = pipe(my_pipe);

    if (pipe_status == -1) {
        perror("Can't create a pipe\n");
        return 1;
    }

    pid_t child = fork();

    switch (child) {
        case -1: {
            perror("Can't fork the init process\n");
            close_pipe(my_pipe);
            return 1;
        }
        case 0: {
            size_t size = read(my_pipe[0], buffer, BUFSIZ);
            int close_status = close(my_pipe[0]);
            if (close_status == -1) {
                perror("Can't close my_pipe[0]\n");
            }
            for (size_t i = 0; i < size; ++i) {
                buffer[i] = toupper(buffer[i]);
            }
            printf("%s\n", buffer);
            return 0;
        }
        default: {
            ssize_t write_status = write(my_pipe[1], buffer, strlen(buffer) + 1);
            if (write_status == -1) {
                perror("Can't write to buffer\n");
                close_pipe(my_pipe);
                return 1;
            }
            if (close(my_pipe[1]) == -1) {
                perror("Can't close my_pipe[1]\n");
            }
            printf("%s\n", buffer);
            int status;
            pid_t wait_status = wait(&status);
            if(wait_status == -1){
                perror("Can't wait for child process\n");
                return 1;
            }
            printStat(child, status);
            break;
        }
    }
    return 0;
}
