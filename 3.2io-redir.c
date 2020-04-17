#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
	int backup_des, stdout_des, ofdes;
	stdout_des = fileno(stdout);
	backup_des = dup(stdout_des);

	printf("Hello, world! (1)\n");

	ofdes = open("test.txt", O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);

	dup2(ofdes,stdout_des);
	printf("Hello, world! (2)\n");

	dup2(backup_des, stdout_des);
	printf("Hello, world! (3)\n");

	close(ofdes);
}
