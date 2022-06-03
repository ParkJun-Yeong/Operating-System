/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 32181854 
*	    Student name : Park Jun-Yeong
*
*   lab1_sched.c :
*       - Lab1 source file.
*       - Must contains scueduler algorithm test code.
*
*/

#include <aio.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <asm/unistd.h>

#include "lab1_sched_types.h"

/*
 * you need to implement scheduler simlator test code.
 *
 */

int main(int argc, char *argv[]){
	int i;
	int count;
	int position_index = 5;
	Process* jobs;

	printf("How many processes do you create? > ");
	scanf("%d", &count);
	jobs = (Process *)malloc(sizeof(Process) * count);

	//create processes
	for(i = 0; i < count; i++)
	{
		jobs[i].pid = i;
		jobs[i].position = position_index++;
		jobs[i].next = NULL;
		printf("arrived_time, service_time of process %d >> ", i);
		scanf("%d %d", &(jobs[i].arrived_time), &(jobs[i].service_time));
		jobs[i].remained_time = jobs[i].service_time;
		jobs[i].wait_time = 0;
	}

	int ch;
	int q;
	printf("Scheduler: 1)FCFS 2)SPN 3)RR");
	scanf("%d",&ch);

	Timeline(count);
	while(1) {
		switch(ch) {
			case 1:
				Timeline(count);
				FCFS(jobs, count);
				break;
			case 2:
				Timeline(count);
				SPN(jobs, count);
				break;
			case 3:
				printf("time quantum >> ");
				scanf("%d", &q);
				Timeline(count);
				RR(jobs, count, q);
				break;
			default:
				printf("wrong number\n");
		}
	}

	return 0;

}

