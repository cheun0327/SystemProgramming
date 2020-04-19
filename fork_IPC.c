#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 10

int main(int argc, char *argv[]){
	int pipe1[2], pipe2[2];	//two descriptor for pipe
	int n;		//read byte
	char buf[BUFFER_SIZE];	//buffer
	int input, output;	//input and output file descriptor

	if(pipe(pipe1)==-1){printf("pipe1 create error!");}	//create two pipe1
	if(pipe(pipe2)==-1){printf("pipe2 create error!");}	//create two pipe2
	
	if (fork() != 0) {	
		if (fork() != 0) {	//parent process
			printf("parent\n");
			close(pipe1[0]);	//close pipe1's read pipe
			close(pipe2[0]);	//close pipe2
			close(pipe2[1]);
			if ((input = open(argv[1], O_RDONLY)) < 0)	//open input file
				printf("input file open error\n");

			while ((n = read(input, buf, BUFFER_SIZE)) != 0) {
				write(pipe1[1], buf, n); //write data to pipe1
			}
			close(pipe1[1]);
			close(input);
			wait(NULL);
		}
		else { //second child process
			printf("secondChild\n");
			close(pipe1[0]);	//close pipe1
			close(pipe1[1]);
			close(pipe2[1]);	//close pipe2's write pipe
			if ((output = creat("output2.txt", 0666)) < 0)	//create output file
				printf("create output file error\n");

			while ((n = read(pipe2[0], buf, BUFFER_SIZE)) != 0) {
				//write capital letter to output from pipe2 until pipe2 closed
				write(output, buf, n);
			}
			close(pipe2[0]);
			close(output);
			exit(0);
		}
	}
	else {	//first child process
		printf("firstChild\n");
		close(pipe1[1]);	//close pipe1's write pipe
		close(pipe2[0]);	//close pipe2's read pipe

		while ((n = read(pipe1[0], buf, 10)) != 0) {	//read data from pipe1
			for (int i = 0; i < BUFFER_SIZE; i++) {
				if (buf[i] >= 97 | buf[i] <= 122) {	//convert to capital
					buf[i] = buf[i] - 32;
				}
			}
			write(pipe2[1], buf, n);	//write data to pipe2
		}
		close(input);
		close(pipe1[0]);
		close(pipe2[1]);
		exit(0);
	}
	return 0;
}
