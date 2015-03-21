#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
void sigHandler(int);
void sigHandler1(int);


int main(void){
	pid_t pid;

	signal(SIGALRM,sigHandler);
	signal(SIGCHLD,sigHandler1);
	if(!(pid=fork())){
		int i;
		printf("pid: %d, Im a child process\n",getpid());
		kill(getppid(),SIGALRM);//// this kill function uses getpid()
		/// so this fuction send SIGALRM to parent
		/// signal handler is also called in parent process 
		//  this handler exit parent process
		// so this program is closed before SIGCHLD occur
	}

	else{
		printf("pid: %d, im a parent process\n",getpid());
		wait(NULL);
	}
	return 0;
	
}

void sigHandler(int sig){
	if(sig==SIGALRM){
		printf("SIGALRM\n");
		exit(1);
	}
	else
		return;

}
void sigHandler1(int sig){
	if(sig==SIGCHLD)
		printf("SIGCHLD\n");
	else
		return;

}
