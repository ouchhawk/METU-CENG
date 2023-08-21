#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
int main()
{
    int fd[2][2];
    
	int status;
	pipe(fd[0]);
	pipe(fd[1]);
	
	
	
    int pid = fork();
    if(pid==0){
        dup2(fd[0][1], STDOUT_FILENO); //gonderildi 1
		
	close(fd[0][0]); // kullanildi
		close(fd[0][1]);
		close(fd[1][0]);
		close(fd[1][1]);
		
        execl("/bin/ls", "ls", "-l", NULL);
    }
	else{
		dup2(fd[0][0], STDIN_FILENO); //alindi 1
		
		int filedes = open("test.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
		dup2(filedes, STDOUT_FILENO); // alindi 2
	close(fd[0][0]); // kullanildi
		close(fd[0][1]);
		close(fd[1][0]);
		close(fd[1][1]);
		close(filedes);

        execl("/usr/bin/grep", "grep", "pp", NULL);
	}

	
        
    return 0;
}