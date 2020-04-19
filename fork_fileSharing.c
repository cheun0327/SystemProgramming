#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 10

int main(int argc, char *argv[]){
	int fd, fdo; //file descriptor
	int pid;	//process id
	char buf[BUFFER_SIZE];	//buffer
	ssize_t n;	//read bytes

	if(argc != 2){ //argument error
		perror("argv[0]");
		return 1;
	}
	
	fd = open(argv[1], O_RDONLY); //open file with readonly flag
	if (fd<0){	//read file open error
		printf("read file open error\n");
	}

	fdo = open("output.txt", O_RDWR | O_CREAT, 0766);
	if(fdo < 0){
		printf("file create error");
		perror("open output file error");
	}

	if((pid=fork()) != 0){
		if((pid=fork()) != 0){ //parent process
			while((n = read(fd, buf, BUFFER_SIZE)) != 0){
				write(fdo, "0 ", 2);
				write(fdo, buf, n);
				write(fdo, "\n", 1);
				printf("pid = %d, chars = %s\n", pid, buf);
				sleep(1);
			}
			return 0;
		}else{ //second forked child process
			while((n = read(fd, buf, BUFFER_SIZE)) != 0){
				write(fdo, "2 ", 2);
				write(fdo, buf, n);
				write(fdo, "\n", 1);
				printf("pid = %d, chars = %s\n", pid, buf);
				sleep(1);
			}
			return 0;
		}
	}else{ //first forked child process
		while((n = read(fd, buf, BUFFER_SIZE)) != 0){
			write(fdo, "1 ", 2);
			write(fdo, buf, n);
			write(fdo, "\n", 1);
			printf("pid = %d, chars = %s\n", pid, buf);
			sleep(1);
		}
		return 0;
	}
	close(fd);
	close(fdo);
	return 0;
}
