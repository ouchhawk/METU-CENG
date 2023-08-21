#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
int main()
{
	int half[2];
	
    int pid;
    int pid2;
	int pid3;
	int status;
	char *arg1[] = {"/bin/ls", "-l", NULL};
	char *arg2[] = {"/usr/bin/grep", "pp", NULL};
	char *arg3[] = {"/usr/bin/wc", NULL};
	
	pipe(half);
	
pid3=fork();

if(pid3==0)
{
		int fd1[3][2];
		pipe(fd1[0]);
		pipe(fd1[1]);
		pipe(fd1[2]);
	
		pid = fork();
		
		if(pid==0){

			dup2(fd1[0][1], 1);

			close(fd1[0][0]);
			close(fd1[0][1]);
			close(fd1[1][1]);
			close(fd1[1][0]);
			close(fd1[2][1]);
			close(fd1[2][0]);
			close(half[1]);
			close(half[0]);

			execv(arg1[0], arg1);
		}
		else
		{
			char buffer[256];
				
			wait(NULL);
			exit(0);
			int n;
            while ((n = read(fd1[0][0], buffer, sizeof(buffer))) > 0)
            {
				write(half[1], buffer, n);
            }
			close(fd1[0][0]);
			close(fd1[0][1]);
			close(fd1[1][1]);
			close(fd1[1][0]);
			close(fd1[2][1]);
			close(fd1[2][0]);
			close(half[1]);
			close(half[0]);
			//printf("line: %s \n", buffer);
			
			

		}
}


else{
	int fd[3][2];
		pipe(fd[0]);
		pipe(fd[1]);
		pipe(fd[2]);

		pid2 = fork();
		
		if(pid2==0){
			
				
				dup2(half[0], 0);
				dup2(fd[0][1], 1);

				close(fd[0][0]);
				close(fd[0][1]);
				close(fd[1][1]);
				close(fd[1][0]);
				close(fd[2][1]);
				close(fd[2][0]);
				close(half[0]);
				close(half[1]);

				execv(arg2[0], arg2);
			

		}
		else
		{
			int filedes = open("TEST.txt",O_APPEND|O_WRONLY|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO);

			char buffer[256];
				
				
			int n;
            while ((n = read(fd[0][0], buffer, 256)) > 0){
				write(filedes, buffer, n);
            }
			
			close(filedes);
			
				
			//printf("line: %s \n", buffer);
			close(fd[0][0]);
			close(fd[0][1]);
			close(fd[1][1]);
			close(fd[1][0]);
			close(fd[2][1]);
			close(fd[2][0]);
			close(half[0]);
			close(half[1]);
			close(filedes);
			exit(0);
			
			wait(NULL);
			}
}

		while(wait(&status) > 0);


    return 0;
}