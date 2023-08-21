#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
int main()
{
    int fd[2][2];
    int pid[2];
    int pid2[1];
char *arg1[] = {"/bin/ls", "-l", NULL};
char *arg2[] = {"/usr/bin/grep", "pp", NULL};
char *arg3[] = {"/usr/bin/wc", NULL};
char *arg4[] = {"/bin/ls", "-i", NULL};
	
	pipe(fd[0]);
	pipe(fd[1]);
	

    
	for(int i=0; i<2; i++){
		pid[i] = fork();
		if(pid[i]==0){
			if(i==0)
			{
				dup2(fd[0][1], STDOUT_FILENO);
				execv(arg1[0], arg1);
			}
			else{
				dup2(fd[0][1], STDOUT_FILENO);
				execv(arg4[0], arg4);
			}
	}
	
	for(int i=0; i<1; i++){
		pid2[i] = fork();
		if(pid2[i]==0){
			if(i==0)
			{
				dup2(fd[0][0], STDIN_FILENO);
				close(fd[0][0]);
				close(fd[0][1]);
				close(fd[1][0]);
				close(fd[1][1]);
				execv(arg3[0], arg3);
			}
	}}
	
	
	
	}
	

    
	
        
    return 0;
}