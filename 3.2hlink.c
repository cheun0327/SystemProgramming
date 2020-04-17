#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]){
	if(argc !=3){
		perror("argument error");
		return 1;
	}
	if(link(argv[1], argv[2]) < 0){
		perror("link fail");
		return 2;
	}
}
