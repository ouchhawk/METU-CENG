#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
int main()
{
    int fd[3][2];
    int pid[3];
char *arg1[] = {"/bin/ls", "-l", NULL};
char *arg2[] = {"/usr/bin/grep", "pp", NULL};
char *arg3[] = {"/usr/bin/wc", NULL};
	
	pipe(fd[0]);
	pipe(fd[1]);
	pipe(fd[2]);
	int filedes = open("test.txt", O_WRONLY  | O_CREAT);


	 

		pid[0] = fork();

if(pid[0]==0){
					dup2(filedes, 1);
					
					close(filedes);  
					close(fd[0][1]);
					
					execv(arg1[0], arg1);
				
	}
	

    
	
        
    return 0;
}