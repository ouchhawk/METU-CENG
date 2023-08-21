#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
int main()
{
    int fd[2][2];
    
	
	pipe(fd[0]);
	pipe(fd[1]);
	

    int pid = fork();
    if(pid==0){
        dup2(fd[0][1], STDOUT_FILENO); //gonderildi 1
        execl("/bin/ls", "ls", "-l", NULL);
    }
	

    int pid2 = fork();
    if(pid2==0){
        dup2(fd[0][0], STDIN_FILENO); //alindi 1
		dup2(fd[1][1], STDOUT_FILENO); //gonderildi 2

        close(fd[0][1]); // kullanildi
        execl("/usr/bin/grep", "grep", "pp", NULL);
    }
	
	int pid3 = fork();
    if(pid3==0){
        dup2(fd[1][0], STDIN_FILENO); // alindi 2
		
		
		close(fd[1][1]); // kullanildi
        close(fd[0][1]); // kullanildi
        execl("/usr/bin/wc", "wc", NULL);
    }
	
    
        
    return 0;
}