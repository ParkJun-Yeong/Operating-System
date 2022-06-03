/*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student pd : 32181854 
*	    Student name : Park Jun-Yeong
*
*   lab1_sched.c :
*       - Lab1 source file.
*       - Must contains scueduler algorithm function'definition.
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
#include <stdbool.h>
#include <ncurses.h>

#define __FAVOR_BSD
#include "lab1_sched_types.h"

void Timeline(int length)
{
	int i, j;
	initscr();
	for(i = 0; i<30; i++){
		move(4, 5+i);
		printw("%d",i);
	}
	for(i = 1; i<=length; i++) {
		move(5+i,4);
		printw("%d",i);
	}	
	refresh();
	endwin();
}
void MinSort(int* _times, int length)
{
	int i, j;
	int tmp;

	//sorting
	for(i = 0; i < length-1; i++) {
		for(j = i+1; j < length; j++) {
			if(_times[i] >= _times[j]) {	//swap times[i] and times[j]
				tmp = _times[j];
				_times[j] = _times[i];
				_times[i] = tmp;	}
		}}
	
	
}
bool IsEmpty(List* sched)
{
	if(!(sched->head))
		return true;
	else	return false;
}
void Push(List* sched, Process* job)
{
	if(IsEmpty(sched))	//empty List
		sched->head = sched->tail = job;
	else {
		sched->tail->next = job;
		sched->tail = job;
	}
}
void Pop(List* sched)
{
	if(IsEmpty(sched)) {
		printf("Empty Queue\n");
		return;
	}
	sched->head = sched->head->next;
	if(IsEmpty(sched)) {
		sched->tail = NULL;
	}
}
void Delete(List* list, Process* job)
{
	//search
	Process* p = list->head;
	if(p->pid == job->pid) {	//head node
		list->head = list->head->next;
		return;
	}

	while(p)
	{
		if(p->next->pid == job->pid) {
			p->next = job->next;
			return;
		}
		else
			p = p->next;
	}
}
void draw(int Y,int X, int start, int end)
{
	int width = end - start;
	int i;
	X += 5;

	initscr();

	for(i = 0; i<width; i++) {
		move(Y,X);
		printw("%c",'-');
		X++;
	}
	refresh();
	sleep(3);
	endwin();

}
void FCFS(Process* jobs, int length)	
{
	int i, j;
	int times[length];
	Process* _jobs = (Process *)malloc(sizeof(Process) * length);

	for(i = 0; i<length; i++)
		_jobs[i] = jobs[i];	//copy the elements

	//sorting
	for(i=0; i<length; i++)
		times[i] = _jobs[i].arrived_time;


	MinSort(times, length);

	List* _scheduled = (List *)malloc(sizeof(List));
	_scheduled->head = NULL;
	_scheduled->tail = NULL;

	//scheduling
	for(i=0; i<length; i++)
		for(j = 0; j<length; j++)
			if(_jobs[j].arrived_time == times[i]) {
				Push(_scheduled,&(_jobs[i]));
			}

	//executing (drawing timeline)
	Process* _current = _scheduled->head;
	int timer = 0;
	Process* p;
	while(!IsEmpty(_scheduled))
	{
		for(timer; timer <= _current->arrived_time-1; timer++);
		draw(_current->position, timer, timer, timer+_current->service_time);
		
		for(p=_current->next; p; p=p->next) {
			p->wait_time += _current->service_time;
		}

		timer += _current->service_time;
		_current->remained_time = 0;
		_current = _current->next;
		Pop(_scheduled);	//current process complete
	}

}
void SPN(Process* jobs, int length)
{
	int i, j;
	int count;
	int times[length];
	Process* _jobs = (Process *)malloc(sizeof(Process) * length);

	for(i = 0; i<length; i++)
		_jobs[i] = jobs[i];	//copy the elements

	//initial sorting
	for(i = 0; i<length; i++)
		times[i] = _jobs[i].service_time;

	MinSort(times, length);

	Process* _scheduled;

	List* _temp = (List *)malloc(sizeof(List));
	_temp->head = NULL;
	_temp->tail = NULL;

	//initially, push jobs in temp list as sorted_times (guarantee that this order is the shortest job order)
	for(i=0; i<length; i++)
		for(j = 0; j<length; j++)
			if(_jobs[j].service_time == times[i])
				Push(_temp, &(_jobs[j]));

	//drawing timeline
	Process* _current;
	int timer = 0;
	Process* p = _temp->head;
	while(!IsEmpty(_temp)){
		_scheduled = NULL;
		p = _temp->head;

		while(!_scheduled)	//scheduling
		{
			if(!p) {
				timer++;
				p = _temp->head;
			}
			if(p->arrived_time <= timer) {
				_scheduled = p;
				Delete(_temp, p);
			} else
				p = p->next;

		}
		

		draw(_scheduled->position, timer, timer, timer+_scheduled->service_time);

		timer += _scheduled->service_time;
	}
}
void RR(Process* jobs, int length, int q)
{
	int i, j;
	int _times[length];
	Process _jobs[length];
	for(i = 0; i<length; i++)
		_jobs[i] = jobs[i];

	for(i = 0; i<length; i++)
		_times[i] = _jobs[i].arrived_time;

	MinSort(_times, length);
	
	Process* temp = (Process *)malloc(sizeof(Process) * length);
	//jobs sorting
	for(i = 0; i<length; i++)
		for(j = 0; j<length; j++)
			if(_jobs[j].arrived_time == _times[i])
				temp[i] = _jobs[j];
	for(i = 0; i<length; i++)
		_jobs[i] = temp[i];

	free(temp);


	List* _scheduled = (List *)malloc(sizeof(List));
	_scheduled->head = NULL;
	_scheduled->tail = NULL;
	
	Push(_scheduled, &(_jobs[0]));

	int timer = _scheduled->head->arrived_time;
	int proc_index = 1;
	Process* current = _scheduled->head;

	while(!IsEmpty(_scheduled))
	{
		draw(current->position, timer, timer, timer+q);
		current->remained_time -= q;
		timer += q;


		for(i=proc_index; i < length; i++)
			if(_jobs[i].arrived_time <= timer) {
				Push(_scheduled, &(_jobs[i]));
				proc_index++;
			}
		if(current->remained_time > 0)
			Push(_scheduled, current);
		
		Pop(_scheduled);

		current = _scheduled->head;

	}
}
/*void HRRN(Process* jobs, int length)
{
	
}*/


