/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *  $Id: sched.c,v 1.27 2011/01/26 20:14:19 Sasi Exp $
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 *
 *	Description:  A Small attempt to Write a new scheduler for my task lib
 *   		      without giving the control to OS
 */

#include "list.h"
#include "sched_sfs.h"


#define TICK_TIMER_GRANULARITY  20  /*20 milli secs*/
#define LOAD_THRESHOLD    40  /**/
#define DEF_HPT_TSLICE   60
#define DEF_LOW_TSLICE   30
#define TASK_HIGH_PRIO(p)   p->prio < 5


void signal_int (int sig);
void seg_fault (int sig);
void init_soft_irq (void);
int init_timer_mgr (void);
void init_io (void);
void setup_interrupt (void);
static inline void calc_current_cpu_usage (void);
static inline unsigned int rq_task_count ();

enum TASK_TYPE {
	TASK_IO,
	TASK_NON_IO
};

struct tskwaitq {
	struct list_head hd;	
	int    nwp;
};

struct tskrunq {
	struct list_head hd[MAX_PRIO];	
	int    nrp;
};

static struct tsk * next_task_ready_to_run ();
static inline void rm_n_move_task_new_rq (struct tsk *p);

static struct tsk  *current; /*Holds the Current Task*/
static struct tsk  *idle = NULL, *parent = NULL;
static struct tsk *nxt_imm_task = NULL; /*Immediate task to run */

static struct tskrunq rq;
static struct tskwaitq wq;

unsigned long tswc = 0;
unsigned long jiffies = 0;

int sched_need = FALSE;
int os_sched_enabled = 0;
int switch_bk = 0;
int im_count = 0, not_im_count = 0;

struct tsk *get_next_eligible_task_to_run ()
{
	struct tsk *next = nxt_imm_task;

	if (!rq.nrp) {
		im_count++;
		return idle;
	}

	if (next)  {
		if (current != nxt_imm_task) {
			im_count++;
			nxt_imm_task = NULL;
		}
	}
	else {
		next = next_task_ready_to_run ();
		if (next)
			not_im_count++;
	}

	return next;
}
/*
 *  XXX: Change this sched make it more reliable
 *       At present it works based on RoundRobin
 */
static struct tsk * next_task_ready_to_run ()
{
	int prio = 0;
	struct tsk *p = NULL;

	if (!rq.nrp) {
		return NULL;;
	}

	while (prio < MAX_PRIO) {

		if (!list_empty (&rq.hd[prio])) {

			p = list_first_entry (&rq.hd[prio], struct tsk, hd);
#ifdef DBG
			printf (" Task Switch : %s -> %s\n" , current->taskname, p->taskname);
#endif
			if (p != current)
				rm_n_move_task_new_rq (p);
			return p;
		}
		prio++;
	}
	return NULL;
}

static struct tsk *get_nxt_tsk (void)
{
}

static inline void update_stats (void) 
{
	tswc++;
	calc_current_cpu_usage ();

	current->currt = 0;
	/*task finished its time slice
	  schedule for another task*/
	current->swct++;
	current->etick = times (NULL);
	current->trt += ((current->etick - current->stick) * 10);
}

static inline int context_switch (struct tsk *curr, struct tsk *next)
{
	if (!nsetjmp (curr->regs)) {
		current = next;
		SET_TASK_STATE (current, TASK_RUNNING);
		current->stick = times (NULL);
		signals_unblock (SIGALRM);
		nlongjmp (next->regs, 1);
	}
	return 0;
}

static inline int get_load_on_system (void)
{
	return ((rq_task_count () * 100)/ task_count);
}

static inline int share_load_across_system (int load)
{
	int msec = 1000;	
	int share  = 0;

	if (!load)
		load = 1;

	share = (msec * load) / 100;

	share /= (rq_task_count() + 1);

	return share;
}

static inline void find_n_move_high_cpu_usage_tasks (struct tsk *tsk)
{
}

static inline void calculate_time_slice (struct tsk *tsk)
{
	int load = 0, task_share = 0;

	if ((tsk == idle) || (tsk == current))
		return;

	load = get_load_on_system ();

	if (load > LOAD_THRESHOLD) {
#ifdef DBG
		/*system too much loaded*/
		struct timespec ts;

		ts.tv_sec = 1;
		/*XXX: inc delay in future*/
		ts.tv_nsec = 0; /*sleep for 1 ms*/

		printf ("Ooops !!!! ..... CROSSED System threshold ....Limit : %u\n", load);

		nanosleep (&ts, NULL);
#endif
		if (tsk->cpuusage > LOAD_THRESHOLD) {
			tsk->prio = 19;
		}
	} 

	task_share = share_load_across_system (load);

	tsk->tslice = task_share;

	if (TASK_HIGH_PRIO(tsk)) {
		tsk->tslice += DEF_HPT_TSLICE;			
	} else {
		tsk->tslice += DEF_LOW_TSLICE;			
	}

	return;
}

static inline int sig_pending (void)
{
	if (current->signals & current->sig_pending) {
		return 1;
	}
	return 0;
}

void schedule (void)
{
	struct tsk * etsk =  NULL;
	
	if (!os_sched_enabled) {

		if (sig_pending ()) {
			process_signal (current);
		}

		etsk = get_next_eligible_task_to_run ();

		sched_need = FALSE;

		if (current != etsk) {
			collect_stack_trace (current);
			update_stats ();
			calculate_time_slice (etsk);
			SET_TASK_STATE (current, TASK_INTERRUPTIBLE);
			context_switch (current, etsk);
		} else {
			calc_current_cpu_usage ();
		}

	} else {
		fprintf (stderr, "[BUG] : Ooops !!!! ..... OS SCHED MODE SCHEDULER Got the trigger ... \n");
		struct timespec ts;
		/* Delay for a bit */
		ts.tv_sec = 0;
		/*XXX: inc delay in future*/
		ts.tv_nsec = 1 * 1000; /*sleep for 1 ms*/

		nanosleep (&ts, NULL);
	}

	return;
}

static void idle_task(void)
{
	struct timespec ts;

	set_current_task_time_slice (0);

	while(1) {
		/* Delay for a bit */
		ts.tv_sec = 0;
		/*XXX: inc delay in future*/
		ts.tv_nsec = 1 * 1000; /*sleep for 1 ms*/

		nanosleep (&ts, NULL);

		if (sched_need) {
			SET_TASK_STATE (idle, TASK_INTERRUPTIBLE);
			schedule ();
		}
		if (switch_bk) {
			switch_back ();	
			switch_bk = 0;
		}
	}
}

void create_idle_task (void)
{
	struct attr attr;

	attr.prio = 19;
	attr.stksze = 32 * 1024;

	idle = spawn_task ("IDLE", attr, (void *(*)(void *))idle_task, NULL);

	/*XXX:spawn task by default add the newly created task to rq
 	  we are deleting the idle task from the rq, since it is useless
	  to make the idle task as part of runqueue*/

	SET_TASK_STATE (idle, TASK_INTERRUPTIBLE);

	list_del (&idle->hd);
}

void create_parent_task (clock_t sticks)
{
	struct attr attr;

	attr.prio = 1;
	attr.stksze = 10000;

	current  = spawn_task ("Main", attr, NULL, NULL);

	parent = current;

	current->stick = sticks;

	current->onrq = 1;

	SET_TASK_STATE (current, TASK_RUNNING);
}

void init_scheduler (void)
{
	int prio = MAX_PRIO;

	while (prio--) {
		 INIT_LIST_HEAD (&rq.hd[prio]);
	}
	INIT_LIST_HEAD (&wq.hd);
}

static inline void task_find_right_RQ (struct tsk *ntsk)
{
	list_add_tail (&ntsk->hd, &rq.hd[ntsk->prio]);

	ntsk->onrq = 1;

	++rq.nrp;
}

static inline void rm_n_move_task_new_rq (struct tsk *p)
{
	list_del (&p->hd);

	list_add_tail (&p->hd, &rq.hd[p->prio]);
}

void add_task_to_wait_Q (struct tsk *ntsk)
{
	if (ntsk->onrq) {

		list_del (&ntsk->hd);

		rq.nrp--;

		list_add_tail (&ntsk->hd, &wq.hd);

		SET_TASK_STATE (ntsk, TASK_INTERRUPTIBLE);

		ntsk->onrq = 0;

		ntsk->stime = (clock_t)times(NULL);

		++wq.nwp;
	} else {
		printf ("BUG:  WAIT_Q task -> WAIT_Q TASK - It should be RQ->WQ\n");
	}

	
}

void wakeup_task (struct tsk *p)
{
	if (!IS_TASK_NOT_ON_RQ (p)) {
		printf ("BUG: Trying to wake up the task more the one time .......\n"); 
		fflush (stdout);
	}
	if (IS_TASK_NOT_ON_RQ (p)) {
		list_del (&p->hd);
		list_add (&p->hd, &rq.hd[p->prio]);
		p->onrq = 1;
		rq.nrp++;
		wq.nwp--;
	}
#ifndef RT_CONSTRAINT
	if (current->prio > p->prio) {
		sched_need = TRUE;
	}
#endif
	if (nxt_imm_task) { 
		if (nxt_imm_task->prio > p->prio)
			nxt_imm_task = p;
	}
	else
		nxt_imm_task = p;

	p->stime = (times(NULL) - p->stime);
}

void wakeup_new_task (struct tsk *ntsk)
{
#ifndef RT_CONSTRAINT
	if (current->prio > ntsk->prio) {
		sched_need = TRUE;
	}
#endif
	task_find_right_RQ (ntsk);

	if (nxt_imm_task) { 
		if (nxt_imm_task->prio > ntsk->prio)
			nxt_imm_task = ntsk;
	}
	else
		nxt_imm_task = ntsk;

	ntsk->stime = times(NULL);
}

void cpu_yield (void)
{
	schedule ();
}

unsigned long get_current_task_id (void)
{
	return  current->tid;
}

void add_current_task_wait_Q (void)
{
	if (!os_sched_enabled)
		add_task_to_wait_Q (current);
}

void account_task_time_stats (void)
{
	current->etick = times (NULL);

	current->currt = (current->etick - current->stick) * 10;

        if (current->currt >= current->tslice) {
                sched_need = TRUE;
        }
        return;
}

inline void update_process_time (void)
{
	account_task_time_stats ();
}

inline void task_exit (struct tsk *p)
{
	list_del (&p->hd);
	memset (p,  0,  sizeof (struct tsk));
	free (p->stk);
	free (p);
	task_count--;
}

void set_current_task_time_slice (unsigned int tslice)
{
	current->tslice = tslice;
}

int get_current_task_tslice (void)
{
	return current->tslice;
}

int wait_on_fd (int fd)
{
	list_del (&current->hd);
	--rq.nrp;
	add_task_to_IO_wait_Q (&current->hd, fd);
	++wq.nwp;
	current->onrq = 0;
	schedule ();
	return 0;
}

struct tsk * find_task (int tid)
{
	struct tsk *p = NULL;
	struct list_head  *pnode = NULL;
	struct list_head  *head = &(wq.hd);
	int prio = 0;

	if ((p = io_wait_task_lookup (tid))) 
		goto task_found;

	list_for_each (pnode, head) {
		p = list_entry (pnode, struct tsk, hd);
		if (p->tid == tid)
			goto task_found;
	}

	if (rq.nrp) {
		while (prio < MAX_PRIO) {
			if (!list_empty (&rq.hd[prio])) {
				head = &rq.hd[prio];
				list_for_each (pnode, head) {
					p = list_entry (pnode, struct tsk, hd);
					if (p->tid == tid) 
						goto task_found;
				}
			}
			prio++;
		}
	}
	return NULL;
task_found:
	return p;
}

void print_task_count (void) 
{
	printf ("TASK in RUN Q : %d TASK in WAIT Q : %d\r\n",
		rq.nrp,  wq.nwp);
}

static inline void print_task_info (struct tsk *p)
{
	char  * tsk_state[] = {"INIT", "R", "R+", "T", "S", "S+"};
	int     trt = p->trt + p->currt;
	printf
		("%10s   %4d     %d    %6d    %3s  %5d   %5d.%-5d   %5d     %5d\r\n",
		 p->taskname, p->tid, p->event, p->stksize, tsk_state[p->state],
		 p->prio, trt/1000, trt%1000, p->swct,p->cpuusage);
}

void dump_task_info (void)
{
	struct tsk *p = NULL;
	struct list_head  *pnode = NULL;
	struct list_head  *head = &(wq.hd);
	int prio = 0;
	int fd = -1;

	printf ("\n\r");
	printf ("    task       tid  event   stack    state   prio   time(secs)   switches    cpu\r\n");
	printf (" ----------    ---  -----   -----    -----   ----  -----------   --------    ---\r\n");
	print_task_info (idle);
	list_for_each (pnode, head) {
		p = list_entry (pnode, struct tsk, hd);
		print_task_info (p);
	}

	if (rq.nrp) {
		while (prio < MAX_PRIO) {
			if (!list_empty (&rq.hd[prio])) {
				head = &rq.hd[prio];
				list_for_each (pnode, head) {
					p = list_entry (pnode, struct tsk, hd);
					if (p == current)
						continue;
					print_task_info (p);
				}
			}
			prio++;
		}
	}
	while ((head = io_get_next_non_empty_task_list (&fd))) {
		list_for_each (pnode, head) {
			p = list_entry (pnode, struct tsk, hd);
			print_task_info (p);
		}
	}

	print_task_info (current);

	print_task_count (); 

	printf ("Total Memory Allocated : %d.%d KB\r\n", 
		alloc_size/1024,alloc_size%1024);

	printf ("Time : %-5u:%-2u:%-2u:%-3u\r\n", get_hrs (), get_mins () % 60, 
                        get_secs () % 60, get_ticks () % tm_get_ticks_per_second ());

	printf ("IMM Cache : %d  NOT IMM Cache : %d Cache HIT Percentage : %.2lf\n", im_count, not_im_count, 
		((double)(im_count * 100)) / ((double)(im_count + not_im_count)));

	fflush (stdout);
}

void set_task_not_rq (void)
{
	current->onrq  = 0;
	wq.nwp++;
}

static inline void calc_current_cpu_usage (void)
{
	if (current->tslice) {

		current->cpuusage = ((current->currt) * 100) / current->tslice;

		if (current->cpuusage > 100) {
			current->cpuusage = 100;
		}
	}
	else
		current->cpuusage = 100;
}

static inline unsigned int rq_task_count ()
{
	return rq.nrp;
}

void switch_to_os_threading (void)
{
	struct tsk *p = NULL;
	struct list_head  *pnode = NULL;
	struct list_head  *head = NULL;
	int ncpu = 3;
	int prio = 0;
	int fd = -1;

	if (os_sched_enabled)
		return;
	signals_block (SIGALRM);
	signals_block (SIGIO);

	os_sched_enabled = 1;

	if (idle != current)
		create_os_thread (idle);


	list_for_each (pnode, &(wq.hd)) {
		p = list_entry (pnode, struct tsk, hd);
		if (p == parent)
			continue;
		create_os_thread (p);
	}

	pnode = NULL;

	if (rq.nrp) {
		while (prio < MAX_PRIO) {
			if (!list_empty (&rq.hd[prio])) {
				head = &rq.hd[prio];
				list_for_each (pnode, head) {
					p = list_entry (pnode, struct tsk, hd);
					if ((p == current) || (p == parent))
						continue;
					create_os_thread (p);
				}
			}
			prio++;
		}
	}
	pnode = NULL;
	while ((head = io_get_next_non_empty_task_list (&fd))) {
		list_for_each (pnode, head) {
			p = list_entry (pnode, struct tsk, hd);
			if ((p == current) || (p == parent))
				continue;
			create_os_thread (p);
		}
	}


	if (!nsetjmp (current->regs)) {
		create_os_thread (current);
		nlongjmp (parent->regs, 1);
	}

	signals_unblock (SIGIO);
	signals_unblock (SIGALRM);
}

void set_switch_back (void)
{
	switch_bk = 1;
}

void switch_back (void)
{
	struct tsk *p = NULL;
	struct list_head  *pnode = NULL;
	struct list_head  *head = NULL;
	int ncpu = 3;
	int prio = 0;
	int fd = -1;

	signals_block (SIGALRM);
	signals_block (SIGIO);

	os_sched_enabled = 0;

	list_for_each (pnode, &(wq.hd)) {
		p = list_entry (pnode, struct tsk, hd);
		if (p != current)
			 pthread_cancel(p->ptid);
	}

	pnode = NULL;

	if (rq.nrp) {
		while (prio < MAX_PRIO) {
			if (!list_empty (&rq.hd[prio])) {
				head = &rq.hd[prio];
				list_for_each (pnode, head) {
					p = list_entry (pnode, struct tsk, hd);
					if (p != current)
						pthread_cancel(p->ptid);
				}
			}
			prio++;
		}
	}
	pnode = NULL;
	while (!(head = io_get_next_non_empty_task_list (&fd))) {
		list_for_each (pnode, head) {
			p = list_entry (pnode, struct tsk, hd);
			if (p != current)
				pthread_cancel(p->ptid);

		}
	}

	if (idle != current) {
		pthread_cancel (idle->ptid);
	}

	signals_unblock (SIGALRM);
	signals_unblock (SIGIO);
}

void stack_trace ()
{
	struct tsk *p = NULL;
	struct list_head  *pnode = NULL;
	struct list_head  *head = &(wq.hd);
	int prio = 0;
	int fd = -1;

	print_task_stack_trace (idle);
	list_for_each (pnode, head) {
		p = list_entry (pnode, struct tsk, hd);
		print_task_stack_trace (p);
	}

	if (rq.nrp) {
		while (prio < MAX_PRIO) {
			if (!list_empty (&rq.hd[prio])) {
				head = &rq.hd[prio];
				list_for_each (pnode, head) {
					p = list_entry (pnode, struct tsk, hd);
					if (p == current)
						continue;
					print_task_stack_trace (p);
				}
			}
			prio++;
		}
	}
	while ((head = io_get_next_non_empty_task_list (&fd))) {
		list_for_each (pnode, head) {
			p = list_entry (pnode, struct tsk, hd);
			print_task_stack_trace (p);
		}
	}

	print_task_stack_trace (current);

	dump_task_info();
}

void collect_stack_trace (struct tsk *p)
{
	void *array[15];
	size_t i = 0;

	memset (array, 0, 15);

	if (p->stk_trc_sym) {
		free (p->stk_trc_sym);
		p->stk_trc_sym = NULL;
	}

	p->size = backtrace (array, 15);

	p->stk_trc_sym = backtrace_symbols (array, p->size);
}

void print_task_stack_trace (struct tsk *p)
{
	int i = 0;

	if (!p->stk_trc_sym || !p->size) {
		return 0;
	}

	printf ("Task : %s\n", p->taskname);
	printf ("-------------\n");
	for (i = 1; i < p->size; i++) {
		if (!strcmp (p->stk_trc_sym[i], "[(nil)]"))
			break;
		printf ("%s\n", p->stk_trc_sym[i]);
	}
	printf ("\n");
}

void set_current_signal_mask (int sig_no)
{
	current->signals |= sig_no;
}

void set_current_signal_pend_mask (int sig_no)
{
	current->sig_pending |= sig_no;
}

void set_idle_signal_mask (int sig_no)
{
	idle->signals |= sig_no;
}


struct tsk * get_current (void)
{
	return current;
}
