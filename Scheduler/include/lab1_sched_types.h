/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 32181854 
*	    Student name : Park Jun-Yeong
*
*   lab1_sched_types.h :
*       - lab1 header file.
*       - must contains scueduler algorithm function's declations.
*
*/

#ifndef _LAB1_HEADER_H
#define _LAB1_HEADER_H
#include <sys/types.h>
#include <stdbool.h>

typedef struct _Process {
	int pid;
	struct _Process* next;
	int arrived_time;
	int service_time;
	int remained_time;
	
	//size_t start[100];
	//size_t end[100];
	int wait_time;
	int position;	//for presenting timeline, Y coordinate
	
	
}Process;
typedef struct _List {
	Process* head;
	Process* tail;
}List;

void Timeline(int length);
void draw(int Y, int X, int start, int end);
void MinSort(int* _times, int length);
bool IsEmpty(List* sched);
void Push(List* sched, Process* job);
void Pop(List* sched);
void Delete(List* list, Process* job);
void FCFS(Process* jobs, int length);
void RR(Process* job, int length, int q);
void SPN(Process* jobs, int length);
//void MLFQ(int q);

#endif /* LAB1_HEADER_H*/



