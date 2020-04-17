#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]){
	int fdi, fdo;
	char buf[BUFFER_SIZE];
	ssize_t n;

	if(argc != 3){
		perror(argv[0]);
		return 1;
	}

	if((fdi = open(argv[1], O_RDONLY)) == -1){
		perror(argv[1]);
		return 2;
	}
	if((fdo = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC|O_EXCL, S_IRUSR|S_IWUSR)) == -1){
		perror(argv[2]);
		return 3;
	}
	while((n = read(fdi, buf, BUFFER_SIZE)) > 0)
		write(fdo, buf, n);

	close(fdi);
	close(fdo);
	return 0;
}

