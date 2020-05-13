#include <stdio.h>
#include <pthread.h>
#include "linkedlist.c"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void *producer(void*);
void *consumer(void*);
#define MAX_BUF 100;

struct LinkedList buffer;

int count = 0;
int in = -1, out = -1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_has_space = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_has_data = PTHREAD_COND_INITIALIZER;

int main(){
	int i;
	pthread_t threads[2];
	ListInit(&buffer); //initialize linked list buffer

	pthread_create(&threads[0], NULL, producer, NULL);	//create two thread
	pthread_create(&threads[1], NULL, consumer, NULL);
	for(i=0; i<2; i++){
		pthread_join(threads[i], NULL);
	}
	return 0;
}

void *producer(void *v){
	int i;
	srand((unsigned int)time(NULL));	//set random seed
	for(i=0; i<1000; i++){	
		pthread_mutex_lock(&mutex);	//producer get mutex lock
		if(isFull(&buffer))		//if buffer is full,
		{
			printf("producer wait\n");	//producer will wait for signal
			pthread_cond_wait(&buffer_has_space, &mutex);
		}
		int n=(int)(rand()%100);
		insertItem(&buffer, n);		//insert data to buffer
		printf("[%d]insert %d\n", i, n);
		pthread_cond_signal(&buffer_has_data);	//send signal to consumer thread
		pthread_mutex_unlock(&mutex);	//return mutex lock
		usleep(10000);
	}
}

void *consumer(void *v){
	int i, data;
	for(i=0; i<1000; i++){
		pthread_mutex_lock(&mutex);	//consumer get mutex lock
		if(isEmpty(&buffer)){		//is buffer is empty,
			printf("consumer wait\n");	//consumer will wait for signal
			pthread_cond_wait(&buffer_has_data, &mutex);
		}
		data=getItem(&buffer);	//pop and get data from buffer
		printf("[%d]pop = %d\n",i, data);
		pthread_cond_signal(&buffer_has_space);	//send signal to producer thread
		pthread_mutex_unlock(&mutex);	//return mutex lock
		usleep(10000);
	}
}


