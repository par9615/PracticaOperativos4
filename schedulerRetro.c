#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>

extern THANDLER threads[MAXTHREAD];
extern int currthread;
extern int blockevent;
extern int unblockevent;

QUEUE waitinginevent[MAXTHREAD];
QUEUE *ready;
QUEUE *actual;
QUEUE *last;

int queues;
int only_one;

QUEUE* first_not_empty()
{
	for(int i = 0; i < queues; i++)
		if(!_emptyq(&ready[i]))
			return &ready[i];

	return NULL;
}

void initialize()
{
	if (ready == NULL) {
		ready = (QUEUE *)malloc(sizeof(QUEUE) * 10000);
		actual = &ready[0];
		last = &ready[0];
		queues = 1;
		only_one = 1;
	}
}

void scheduler(int arguments)
{
	initialize();
	int old,next;
	int changethread=0;
	int waitingthread=0;
	
	int event=arguments & 0xFF00;
	int callingthread=arguments & 0xFF;

	if(event==NEWTHREAD)
	{
		only_one = 0;
		actual = &ready[0];
		threads[callingthread].status=READY;
		_enqueue(actual,callingthread);		//Encola el nuevo hilo en la cola 0
	}

	if(event==TIMER)
	{
		if (actual == last) { // Agregar nueva cola
			queues++;
			last = &ready[queues -1];
		}
		threads[callingthread].status=READY;
		if (only_one)
			_enqueue(&(*actual), callingthread);
		else
			_enqueue(&(*(actual+1)),callingthread);
		changethread=1;
	}
	
	if(event==BLOCKTHREAD)
	{
		threads[callingthread].status=BLOCKED;
		_enqueue(&waitinginevent[blockevent],callingthread);
		changethread=1;
	}

	if(event==ENDTHREAD)
	{
		threads[callingthread].status=END;
		changethread=1;
	}
	
	if(event==UNBLOCKTHREAD)
	{
			actual = &ready[0];
			threads[callingthread].status=READY;
			_enqueue(actual,callingthread);
	}

	
	if(changethread)
	{
		actual = first_not_empty();
		old=currthread;
		next=_dequeue(actual);
		threads[next].status=RUNNING;
		_swapthreads(old,next);
	}

}
