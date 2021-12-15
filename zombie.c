#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
int main(){
	pid_t pid = fork();
	switch(pid){
	case 0:
		exit(0);
		break;
	default:
		sleep(10);
		break;
	}
	return 0;
}
