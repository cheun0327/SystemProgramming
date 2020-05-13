#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0

#define NUM_RECORDS 100

struct record{
	char name[20];
	int id;
	int balance;
};

int reclock(int fd, int recno, int len, int type);
void display_record(struct record *curr);

int main(){
	struct record current;
	int record_no;
	int tran_record_no;	//transfer : receiver's record #
	int fd, pos, i, n;
	char yes;
	char operation;
	int amount;		//withdraw, deposit transfer amount
	char buffer[100];
	int quit=FALSE;

	fd = open("./account", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
	while(!quit){
		printf("enter account number (0-99) : ");
		scanf("%d", &record_no);
		fgets(buffer, 100, stdin);
		
		if(record_no < 0&&record_no >= NUM_RECORDS)
			break;
		printf("enter operation name (c/r/d/w/t/q) : ");
		scanf("%c", &operation);
		switch(operation){
			case 'c':
				reclock(fd, record_no, sizeof(struct record), F_WRLCK);
				pos = record_no * sizeof(struct record);
				lseek(fd, pos, SEEK_SET);
				printf("> id ? ");
				scanf("%d", &current.id);
				printf("> name ? ");
				scanf("%s", current.name);
				current.balance = 0;
				n = write(fd, &current, sizeof(struct record));
				display_record(&current);
				reclock(fd, record_no, sizeof(struct record), F_UNLCK);
				break;
			case 'r':
				reclock(fd, record_no, sizeof(struct record), F_RDLCK);
				pos = record_no * sizeof(struct record);
				lseek(fd, pos, SEEK_SET);
				n = read(fd, &current, sizeof(struct record));
				display_record(&current);
				reclock(fd, record_no, sizeof(struct record), F_UNLCK);
				break;
			case 'd':
				reclock(fd, record_no, sizeof(struct record), F_WRLCK);
				pos = record_no * sizeof(struct record);
				lseek(fd, pos, SEEK_SET);
				n = read(fd, &current, sizeof(struct record));
				display_record(&current);
				printf("enter amount : ");
				scanf("%d", &amount);
				current.balance += amount;
				lseek(fd, pos, SEEK_SET);
				n = write(fd, &current, sizeof(struct record));
				reclock(fd, record_no, sizeof(struct record), F_UNLCK);
				break;
			case 'w':	//withdraw
				reclock(fd, record_no, sizeof(struct record), F_WRLCK);	//get writer's lock
				pos = record_no * sizeof(struct record);		//set position in rec
				lseek(fd, pos, SEEK_SET);	//set offset as pos
				n = read(fd, &current, sizeof(struct record));
				display_record(&current);
				printf("enter amount : ");
				scanf("%d", &amount);
				current.balance -= amount;	//do withdraw
				while(current.balance < 0){	//error
					printf("You cannot withdraw more than you can afford\n");
					printf("enter amount : ");
					scanf("%d", &amount);
					current.balance -= amount;
				}
				lseek(fd, pos, SEEK_SET);
				n = write(fd, &current, sizeof(struct record));		//fd write data
				reclock(fd, record_no, sizeof(struct record), F_UNLCK);	//unlock
				break;
			case 't':	//transfer
				//withdraw account
				reclock(fd, record_no, sizeof(struct record), F_WRLCK);	//get writer's lock
				pos=record_no * sizeof(struct record);
				lseek(fd, pos, SEEK_SET);
				n = read(fd, &current, sizeof(struct record));
				display_record(&current);
				printf("enter account to transfer : ");
				scanf("%d", &tran_record_no);	//get record number to transfer
				printf("enter amount : ");
				scanf("%d", &amount);
				current.balance -= amount;	//update sender's balance
				while(current.balance < 0){
					printf("You cannot withdraw more than you can afford\n");
					printf("enter amount : ");
					scanf("%d", &amount);
					current.balance -= amount;
				}
				lseek(fd, pos, SEEK_SET);
				n = write(fd, &current, sizeof(struct record));	//fd write sender's data
				reclock(fd, record_no, sizeof(struct record), F_UNLCK);//unlock sender's rec

				//deposit account
				reclock(fd, tran_record_no, sizeof(struct record), F_WRLCK);//get writer's lock
				pos = tran_record_no * sizeof(struct record);	//get receiver's position
				lseek(fd, pos, SEEK_SET);
				n = read(fd, &current, sizeof(struct record));
				current.balance += amount;	//update receiver's balance
				lseek(fd, pos, SEEK_SET);
				n = write(fd, &current, sizeof(struct record));	//write receiver's rec
				display_record(&current);
				reclock(fd, tran_record_no, sizeof(struct record), F_UNLCK);//unlcock receiver's rec
				break;
			case 'q':
				quit = TRUE;
				break;
			default :
				printf("illegal input\n");
				continue;
		}
	}
	close(fd);
	fflush(NULL);
}

int reclock(int fd, int recno, int len, int type){	//record lock
	struct flock fl;
	switch(type){
		case F_RDLCK:
		case F_WRLCK:
		case F_UNLCK:
			fl.l_type = type;
			fl.l_whence = SEEK_SET;
			fl.l_start = recno * len;
			fl.l_len = len;
			fcntl(fd, F_SETLKW, & fl);
			return 1;
		default:
			return -1;
	}
}

void display_record(struct record *curr){
	printf("\n");
	printf("id : %d \n", curr->id);
	printf("namd : %s \n", curr->name);
	printf("balance : %d \n", curr->balance);
	printf("\n");
}


		
