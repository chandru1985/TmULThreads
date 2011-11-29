/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  $Id: thread_lib.c,v 1.25 2011/01/26 20:14:19 Sasi Exp $
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 *  
 *  Description  To create system call interface
 */
#include "list.h"
#include "sched_sfs.h"

inline void task_exit (struct tsk *);
void * tm_malloc (size_t size);
void *tm_calloc(size_t nmemb, size_t size);
int alloc_tid (void);

unsigned int task_count = 0;

void sys_fork (void)
{
 /*does nothing*/
}
void clone_thread (void *(*tsk_wrapper) (void *), void *arg)
{
#if 0
        unsigned long *val;
	/* Mask Out the Return Address*/	
	unsigned long __force_order;
        asm volatile("movl %%ebp,%0\n\t" : "=r" (val), "=m" (__force_order));

	val[1] = &sys_fork;
#endif

	tsk_wrapper (arg);	
	
	task_exit (get_current ());

	schedule ();
}
/*Acutal function for spawning a new task*/
struct tsk * spawn_task(char *taskname, struct attr attr, void *(*tsk_wrapper) (void *), void *arg)
{
	struct tsk  *new_task;

	new_task = (struct tsk *) tm_calloc (1, sizeof (struct tsk ));

	if (!new_task) {
		return NULL;
	}

	memset (new_task, 0, sizeof(struct tsk));

	INIT_LIST_HEAD (&new_task->hd);

	INIT_LIST_HEAD (&new_task->sig_hd);

	INIT_LIST_HEAD (&new_task->lock_hd);

	INIT_LIST_HEAD (&new_task->sync_hd);

	new_task->tid = alloc_tid ();

	new_task->needsched = TRUE;

	new_task->prio = attr.prio;

	new_task->stksize = attr.stksze;

	new_task->tslice = DEFAULT_TIME_SLICE;

	new_task->stk = tm_malloc (new_task->stksize);

	if (!new_task->stk) {
		free(new_task);
		return NULL;
        }

	strncpy (new_task->taskname, (char *)taskname, MAX_TASK_NAME);

	if (!nsetjmp (new_task->regs) && tsk_wrapper) {
		/*
		   The jmp_buf is assumed to contain the following, in order:
		   %ebx
		   %esp
		   %ebp
		   %esi
		   %edi
		   <return address>
		   */
		memcpy(&new_task->stk[new_task->stksize - 4], &arg, 
		       sizeof(unsigned int));
		memcpy(&new_task->stk[new_task->stksize - 8],&tsk_wrapper,
		       sizeof(unsigned int));
		new_task->regs->__jmpbuf[1] = (unsigned int)
				&new_task->stk[new_task->stksize - (3*sizeof(int))];
		new_task->regs->__jmpbuf[5] = (unsigned int)clone_thread;

		task_count++;

		wakeup_new_task (new_task);
	}

	return new_task;
}

int thread_attr_init (struct attr *at)
{
	at->stksze = DEFAULT_STACK_SIZE;
	at->prio   = 100;
	at->schedalgo   = -1 ;
	return 0;
}

int thread_attr_setstacksize (struct attr *tsk_attr, int tsk_stk_size)
{
	tsk_attr->stksze = tsk_stk_size;
	return 0;
}

int thread_attr_setschedpolicy (struct attr *tsk_attr, int sched_alg)
{
	tsk_attr->schedalgo = sched_alg;
	return 0;
}

int thread_attr_setschedprio (struct attr *tsk_attr, int tsk_pri)
{
	tsk_attr->prio = tsk_pri;
	return 0;
}


unsigned int sleep (unsigned int secs)
{
	if (!secs)
		return;
	start_timer (secs * tm_get_ticks_per_second (), get_current (), 
		     (void (*) (void *))&wakeup_task, 0);
	add_task_to_wait_Q (get_current ());
	schedule ();
	return 0;
}

int tm_msleep (unsigned int msecs)
{
	if (msecs < 20) {
		return -1;
	}
	start_timer (tm_convert_msec_to_tick(msecs), get_current (),
		     (void (*) (void *))&wakeup_task, 0);
	add_task_to_wait_Q (get_current ());
	schedule ();
	return 0;
}


int wait_for_event (unsigned int events,  int *evt)
{
	while (!(get_current ()->event & events)) {
		/*no event happend add task to wait q and schedule*/
		add_current_task_wait_Q ();			
		schedule ();
	}	

	/*Capture the events*/
	*evt = get_current ()->event;

	/*Re-set event*/
	get_current ()->event = 0;

	return 0;
}

int  post_event  (int tid, int event)
{
	struct tsk * p = (struct tsk  *) find_task (tid);
	if (!p)
		return -1;
	p->event |= event;
	wakeup_task (p);
	return 0;
}

int tm_signal (int signo, void (*handler)(int))
{
	static unsigned int reg_signals  = 0;

	unsigned int bit_pos = 0x1 << (signo - 1);

	struct signal_s *sig =  tm_malloc (sizeof(struct signal_s));

	if (!sig)
		return -1;

	sig->sig = signo;

	sig->handler = handler;

	INIT_LIST_HEAD (&sig->next);

	list_add (&sig->next, &get_current ()->sig_hd);

	set_current_signal_mask (bit_pos);

	set_idle_signal_mask (bit_pos);
	

	if (!(reg_signals & bit_pos)) {
		os_register_signal_handler (signo, generic_sig_handler);
		reg_signals |= bit_pos;
	}
	return 0;
}

int alloc_tid (void)
{
	static int taskid = 0;
	return ++taskid;
}

int kill_task (int tid) 
{
	struct tsk * p = (struct tsk  *) find_task (tid);
	if (!p)
		return -1;
	task_exit (p);
	return 0;
}

int task_cancel (int taskid)
{
	kill_task (taskid);
	return 0;
}


size_t alloc_size = 0;

void *tm_calloc(size_t nmemb, size_t size)
{
	alloc_size += size;

	return calloc (nmemb, size);
}

void * tm_malloc (size_t size)
{
	return tm_calloc (1, size);
}

void tm_free (void *p , size_t size)
{
	alloc_size -= size;
	free (p);
}

int task_selfid (void)
{
	return get_current ()->tid;
}
 
void invoke_signal_hanlder (int signo)
{
	struct list_head  *pnode = NULL;
	struct list_head  *head = &get_current ()->sig_hd;
	struct signal_s *p =  NULL;

	list_for_each (pnode, head) {

		p = list_entry (pnode, struct signal_s , next);

		if ((p->sig == signo) && p->handler) {
			p->handler (signo);
			return;
		}
	}
}

int tm_exit (int status)
{
	return 0;
}

int tm_raise (int sig)
{
	set_current_signal_pend_mask (0x1 << (sig - 1));

	return 0;
}
void sleep_forever (void)
{
        add_task_to_wait_Q (get_current ());
        schedule ();
}
