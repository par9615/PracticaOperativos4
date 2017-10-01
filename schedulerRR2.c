#include "scheduler.h"
#include <stdio.h>

extern THANDLER threads[MAXTHREAD];
extern int currthread;
extern int blockevent;
extern int unblockevent;
int q = 0;											//contador del quantum


QUEUE ready;
QUEUE waitinginevent[MAXTHREAD];


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
		_enqueue(&ready,callingthread);
	}
	
	if(event==TIMER)
	{
		q++;										//Se incrementa conteo del quantum

		if(q == 2)									//Se ha llegado al quantum
		{
			threads[callingthread].status=READY;	//Se pone el estado del hilo actual en READY	
			_enqueue(&ready, callingthread);		//Se pone el hilo actual en cola de listos
			changethread=1;							//Se cambian los contextos
			q = 0;									//Se resetea el conteo del quantum
		}

	}

	if(event==BLOCKTHREAD)
	{

		threads[callingthread].status=BLOCKED;
		_enqueue(&waitinginevent[blockevent],callingthread);
		changethread=1;

		q=0;
	}

	if(event==ENDTHREAD)
	{
		threads[callingthread].status=END;
		changethread=1;

		q=0;
	}
	
	if(event==UNBLOCKTHREAD)
	{
			threads[callingthread].status=READY;
			_enqueue(&ready,callingthread);
	}

	
	if(changethread)
	{
		old=currthread;
		next=_dequeue(&ready);
		
		threads[next].status=RUNNING;
		_swapthreads(old,next);
	}

}
