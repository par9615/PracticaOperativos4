#include "scheduler.h"

#define NQUEUES 10

extern THANDLER threads[MAXTHREAD];
extern int currthread;
extern int blockevent;
extern int unblockevent;

QUEUE ready[NQUEUES];
QUEUE waitinginevent[MAXTHREAD];

int first_not_empty()
{
	for(int i = 0; i < NQUEUES; i++)
		if(!_emptyq(&ready[i]))
			return i;

	return -1;
}

void scheduler(int arguments)
{
	int old,next;
	int changethread=0;
	int waitingthread=0;
	
	int event=arguments & 0xFF00;
	int callingthread=arguments & 0xFF;

	if(event==NEWTHREAD)
	{
		// Un nuevo hilo va a la cola de listos
		threads[callingthread].status=READY;
		_enqueue(&ready[0],callingthread);		//Encola el nuevo hilo en la cola 0
	}

	if(event==TIMER)
	{
		thread[callingthread].status=READY;
		_enqueue(&ready[(i+1)%NQUEUES],callingthread);
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
			threads[callingthread].status=READY;
			_enqueue(&ready[0],callingthread);
	}

	
	if(changethread)
	{
		int next_queue = first_not_empty();
		old=currthread;
		next=_dequeue(&ready[next_queue]);
		
		threads[next].status=RUNNING;
		_swapthreads(old,next);
	}

}
