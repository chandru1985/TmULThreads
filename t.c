#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include "cmn/list.h"
#include "cmn/sched_sfs.h"

void seg_fault (int );
void timer_expd (void * k);

char *tskname[10]  = {"T1","T2","T3","T4","T5","T6","T7","T8","T9","T10"};

void *start_rout1 (void *unused)
{
	int i = 0,flag = 0;
	int *p = NULL;
	do	{
		sleep (100);
	}while (1);

}

void *start_rout2 (void *unused)
{

	int i = 1;
	do{
		sleep (100);
	}while (1);

}

void *start_rout3 (void *unused)
{

	int i = 1;
	do{
		sleep (100);
	}while (1);

}

Just_log ()
{
	dump_task_info ();
}

void *id2 = NULL;
exp ()
{
	static int i = 0;
	printf ("Timer Expd : %d\n", i++);
	fflush (stdout);
	mod_timer (id2, 2);

}


int rettskid[100] ;

int main (int argc , char **argv)
{
	int i = 0;

	printf ("My PID : %d\n", getpid ());

	tmlib_init ();
	char task[4] = "TS";


	void* id = start_timer (10 * tm_get_ticks_per_second (), NULL, (void (*) (void *))&Just_log, 0);
	void* id1 = start_timer ((11) * tm_get_ticks_per_second (), NULL, (void (*) (void *))&Just_log, 0);
	
	setup_timer (&id2, exp, NULL);

	mod_timer (id2, 2);
#if 1
	while (1) {
	printf ("Remaining time id (10 secs): %u  id1 (1 min) : %u \n",timer_get_remaining_time(id) / tm_get_ticks_per_second () ,timer_get_remaining_time(id1) / tm_get_ticks_per_second ());
	sleep (3);
	}
#endif


	timer_restart (id1);
	while (1) {
		sleep (1);
		printf ("Remaining time id (10 secs): %u  id1 (1 min) : %u \n",timer_get_remaining_time(id) / tm_get_ticks_per_second () ,timer_get_remaining_time(id1) / tm_get_ticks_per_second ());

	}

	for ( i = 0; i <=25; i++) {
		task[2] = (char)(97+i);
		task_create (task, 10,0 ,32000 + i,
				&(start_rout2), NULL,
				(void *)(i+1), &rettskid[i]);
	}

	sleep_forever ();
}
