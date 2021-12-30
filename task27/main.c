#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>

int checkStat(int stat)
{
    if(WIFEXITED(stat))
    {
        int exitCode = WEXITSTATUS(stat);
        if(exitCode !=0)
        {
            printf("Process exited with exit code");
            return -1;
        }
    }
    else if(WIFSIGNALED(stat))
    {
        printf("Process terminated due to signal", WTERMSIG(stat));
        return -1;
    }
    else if(WIFSTOPPED(stat))
    {
        printf("Process stopped due to signal %d", WSTOPSIG(stat));
        return -1;
    }

    return 0;
}

int main(int argc, char** argv){
    FILE* in = popen("grep -E '^$' ../input.txt | wc -l", "r");
    if(in == NULL){
        perror("Error in opening pipe");
        return 1;
    }

    char buff[BUFSIZ];
    char* numLines;
    numLines = fgets(buff, BUFSIZ, in);
    if(numLines == NULL){
        perror("Error in reading from pipe\n");
        if(pclose(in) == -1) {
            perror("Error in closing pipe\n");
        }
        return 1;
    }


    int stat = pclose(in);
    if(stat == -1){
        perror("Error in closing pipe\n");
        return -1;
    } else if(checkStat(stat)){
        return -1;
    }

    printf("Blank lines num: %ld\n", atol(buff));
    return 0;
}