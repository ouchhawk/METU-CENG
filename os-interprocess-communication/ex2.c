#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
int main()
{
    int fd[2][2];
    int pid[3];
char *arg1[] = {"/bin/ls", "-l", NULL};
char *arg2[] = {"/usr/bin/grep", "pp", NULL};
char *arg3[] = {"/usr/bin/wc", NULL};
	
	pipe(fd[0]);
	pipe(fd[1]);
	int filedes = open("test.txt", O_WRONLY | O_APPEND | O_CREAT);


	dup2(filedes, 1);
     close(fd[0][0]);
	 close(fd[0][1]);
	 close(fd[1][1]);
	 close(fd[1][1]);
		

	for(int i=0; i<3; i++){
		pid[i] = fork();
		
	execv(arg1[0], arg1);
	close(filedes);
		
		if(pid[i]==0){
				if(i==0)
				{
					dup2(fd[0][1], STDOUT_FILENO);
					close(filedes);  
					execv(arg1[0], arg1);
				}
				else if(i==1)
				{
					dup2(fd[0][0], STDIN_FILENO);
					dup2(fd[1][1], STDOUT_FILENO);
					
					close(filedes);  
					execv(arg2[0], arg2);
				}
				else if(i==2)
				{
					dup2(fd[1][0], STDIN_FILENO);
					dup2(filedes, STDOUT_FILENO);
					printf("girdi %d", filedes);
					
					
					
					
					execv(arg3[0], arg3);
				}
			}
			
			else
			{
			}
	
	}
	

    
	
        
    return 0;
}