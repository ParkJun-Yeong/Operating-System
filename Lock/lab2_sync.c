/*
*   DKU Operating System Lab
*           Lab2 (Vehicle production Problem)
*           Student id : 32181854
*           Student name : park jun-yeong
*
*   lab2_sync.c :
*       - lab2 main file.
*       - must contains Vehicle production Problem function's declations.
*
*/

#include <aio.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <asm/unistd.h>

#include "lab2_sync_types.h"


int count; // balance of buffer
pthread_cond_t empty, fill;

CQ buffer;
Node car[5];
WL wl;

void QueueInit(CQ* q) {
	q->balance = 0;
	q->front = NULL;
	q->rear = NULL;
}

void put(CQ *q, int car_num) {		//buffer insert
	Node* tmp = (Node*)malloc(sizeof(Node));
	tmp->car_num = car_num;
	tmp->next = NULL;
	
	q->rear->next = tmp;
	q->rear= tmp;

	q->balance++;
	wl.p_num[car_num]--;
}

int get(CQ* q) {			//buffer pop
	int tmp = q->front->car_num;
	q->front = q->front->next;
	q->balance--;
	return tmp;
}

void *producer(void* arg) {
	int* time_quantum = (int*)arg;
	//RR
	int timer = 0;
	int i, j;
	Node* p = buffer.front;
	Node* q;

	while ( wl.p_num[0] | wl.p_num[1] | wl.p_num[2] | wl.p_num[3] | wl.p_num[4] )
	{
		for (i = 0; i < 5; i++)
			if ((wl.p_time[i] <= timer) & wl.p_num[i]) {
				while (buffer.balance == MAX_SIZE)
					pthread_cond_wait(&empty, &(buffer.tail_lock));

				for(j=0; j<*time_quantum; j++)
					put(&buffer, car[i].car_num);		//critical section
			}
		pthread_cond_signal(&fill);
		
		while (p)
		{
			if (wl.p_num[p->car_num] <= *time_quantum)  //parameter timequantum
				wl.p_num[p->car_num] = 0;	// end producing
			else {
				timer += *time_quantum;
				wl.p_num[p->car_num] -= *time_quantum;
				wl.p_time[p->car_num] += *time_quantum;
			}

			q = p;
			p = p->next;
		}
		p = q;
	}
}


void *consumer(void *arg) {
	int i;
	while (buffer.balance)
	{
		pthread_mutex_lock(&(buffer.head_lock));
		while (buffer.balance == 0)
			pthread_cond_wait(&fill, &(buffer.head_lock));
		int tmp = get(&buffer);
		pthread_cond_signal(&empty);
		pthread_mutex_unlock(&(buffer.head_lock));
		printf("%d\n", tmp);
	}
	
}

void lab2_sync_usage(char *cmd) {
	printf("\n Usage for %s : \n",cmd);
    printf("    -c: Total number of vehicles produced, must be bigger than 0 ( e.g. 100 )\n");
    printf("    -q: RoundRobin Time Quantum, must be bigger than 0 ( e.g. 1, 4 ) \n");
}

void lab2_sync_example(char *cmd) {

	printf("\n Example : \n");
    printf("    #sudo %s -c=100 -q=1 \n", cmd);
    printf("    #sudo %s -c=10000 -q=4 \n", cmd);
}
   


int main(int argc, char* argv[]) {
	char op;
	int n; char junk;
	int i;
	int time_quantum = 0;
	int total_car = 0;
	double execution_time = 0;


	if (argc <= 1) {
		lab2_sync_usage(argv[0]);
		lab2_sync_example(argv[0]);
		exit(0);
	}	// error catching

	for (i = 1; i < argc; i++) {
		if (sscanf(argv[i], "-c=%d%c", &n, &junk) == 1) {
			total_car = n;
		}
		else if (sscanf(argv[i], "-q=%d%c", &n, &junk) == 1) {
			time_quantum = n;
		}
		else {
			lab2_sync_usage(argv[0]);
			lab2_sync_example(argv[0]);
			exit(0);
		}
	}

	/*
	   * You need to implement Vehicle production Problem 
	   * If you want to use other header file, it's ok!
	   * If you have any question for this task, please contact me (ghwls03s@gmail.com)
	   * You may be need to use pthread_create function
	*/
	
	printf("==== Vehicle Production Problem ====\n");
	printf("(1) No Lock Experiment\n");
	//make no lock function
	//initialize
	for (i = 0; i < 5; i++)
		car[i].car_num = i;

	int tmp[5] = { 0,2,4,6,8 };
	int tmp2[5] = { 3,6,4,5,2 };

	for (i = 0; i < 5; i++) {
		wl.p_time[i] = tmp[i];
		wl.p_num[i] = tmp2[i];
	}

	QueueInit(&buffer);
	pthread_t pro;
	pthread_t c[5];

	int* arg_tmp = (int*)malloc(sizeof(int));
	*arg_tmp = time_quantum;

	struct timeval start_time, end_time;
	gettimeofday(&start_time, NULL);
	pthread_create(&pro, NULL, producer, (void*) arg_tmp);
	for (i = 0; i < 5; i++)
		pthread_create(&(c[i]), NULL, consumer, NULL);
	gettimeofday(&end_time, NULL);

	execution_time = (end_time.tv_sec - start_time.tv_sec) + ((end_time.tv_usec - start_time.tv_usec) / 1000000);
	
		printf("==== Vehicle Production Problem ====\n");
	printf("(1) Lock/no-Lock Experiment");
	printf("Experiment Info\n");
	printf("\tTotal Produce Number = %d\n",total_car);
	printf("\tFinal Balance Value = %d\n", buffer.balance);
	printf("\tExecution time = %lf\n", execution_time);

	/*printf("==== Vehicle Production Problem ====\n");
    printf("(2) Coarse-grained Lock Experiment\n");
    printf("Experiment Info\n");
    printf("\tTotal Produce Number = \n");
    printf("\tFinal Balance Value = \n");
    printf("\tExecution time = \n");

	printf("==== Vehicle Production Problem ====\n");
    printf("(3) Fine-grained Lock Experiment\n");
    printf("Experiment Info\n");
    printf("\tTotal Produce Number = \n");
    printf("\tFinal Balance Value = \n");
    printf("\tExecution time = \n");*/


	
	return 0;
}
