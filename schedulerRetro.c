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

/*QUEUE* first_not_empty()
{
	for(int i = 0; i < queues; i++)
		if(!_emptyq(&ready[i]))
			return &ready[i];

	return NULL;
}*/

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
		// Un nuevo hilo va a la cola de listos
		//printf("NEWTHREAD-inicio\n");
		only_one = 0;
		actual = &ready[0];
		threads[callingthread].status=READY;
		_enqueue(actual,callingthread);		//Encola el nuevo hilo en la cola 0
		//printf("NEWTHREAD-fin\n");
	}

	if(event==TIMER)
	{
		//printf("TIMER-inicio\n");
		if (actual == last) { // Agregar nueva cola
			queues++;
			//ready = (QUEUE *)realloc(ready, sizeof(QUEUE) * (queues + 1)); // Crea espacio para nueva cola
			last = &ready[queues -1];
		}

		threads[callingthread].status=READY;
		//printf("Aqui llega\n");

		if (only_one)
			_enqueue(&(*actual), callingthread);
		else
			_enqueue(&(*(actual+1)),callingthread);
		//if (first_not_empty() != NULL)
		changethread=1;
		//printf("TIMER-fin\n");
	}
	
	if(event==BLOCKTHREAD)
	{
		//printf("BLOCKTHREAD-inicio\n");
		threads[callingthread].status=BLOCKED;
		_enqueue(&waitinginevent[blockevent],callingthread);

		changethread=1;
		//printf("BLOCKTHREAD-fin\n");
	}

	if(event==ENDTHREAD)
	{
		//printf("ENDTHREAD-inicio\n");
		threads[callingthread].status=END;
		changethread=1;
		//printf("ENDTHREAD-fin\n");
	}
	
	if(event==UNBLOCKTHREAD)
	{
			//printf("UNBLOCKTHREAD-inicio\n");
			actual = &ready[0];
			threads[callingthread].status=READY;
			_enqueue(actual,callingthread);
			//printf("UNBLOCKTHREAD-fin\n");
	}

	
	if(changethread)
	{
		//printf("changethread-inicio\n");
		//actual = first_not_empty();
		old=currthread;
		next=_dequeue(actual);
		//printf("old:%d\n", old);
		//printf("next:%d\n", next);
		if (_emptyq(actual))
			actual = &(*(actual + 1));
		
		threads[next].status=RUNNING;
		_swapthreads(old,next);
		//printf("changethread-fin\n");
	}

}
