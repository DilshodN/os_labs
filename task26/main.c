#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


void printStat(int stat)
{
	if(WCOREDUMP(stat)) {
        printf("Process ended with core dump %d\n", WCOREDUMP(stat));
    }
	if(WEXITSTATUS(stat)){
		printf("Process ended with exit code %d\n", WEXITSTATUS(stat));
    }
	if(WIFCONTINUED(stat)){
		printf("Process continued %d", WIFCONTINUED(stat));
    }
	if(WIFSIGNALED(stat)) {
        printf("Process terminated due to signal", WIFSIGNALED(stat));
    }
	if(WIFSTOPPED(stat)){
		printf("Process currently stopped\n");
    }
	if(WSTOPSIG(stat)){
		printf("Process stopped due to signal %d", WSTOPSIG(stat));
    }
	if(WTERMSIG(stat)) {
        printf("Process terminated due to signal %d", WTERMSIG(stat));
    }
}

int main()
{
	FILE* in = popen("cat input.txt", "r");
	if(in == NULL){
		perror("Error in opening pipe\n");
		return 1;
	}

	size_t count;
	char buff[BUFSIZ];

    count = fread(buff, sizeof(char), BUFSIZ, in);
    if(ferror(in)){
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
	}else if(!WIFEXITED(stat)){
		printStat(stat);
		return 1;
	}

    for(size_t i = 0; i < count; ++i) {
        buff[i] = toupper(buff[i]);
    }

    fwrite(buff, sizeof(char), count, stdout);
    if(ferror(stdout)){
        perror("Error in writing to stdout\n");
        return 1;
    }

	return 0;
}