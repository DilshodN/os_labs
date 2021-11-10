#include <stdio.h> // perror
#include <unistd.h> // fork
#include <errno.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){
    pid_t pid;
    printf("pid: %d\n", getpid());

    pid = fork();


    if(pid == -1){
        perror("Fork fail\n");
        return -1;
    }

    else if(pid == 0){
        printf("Parent's pid of child: %d\n", getppid());
        printf("pid of a child: %d\n", getpid());
        execlp("cat", "cat", argv[1], NULL);
        perror("Execute cat failure\n");
    }

    else{
        printf("Parent begin\n");
        wait(NULL);
        printf("Parent end\n");
    }


    return 0;
}