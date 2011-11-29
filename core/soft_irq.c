/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  $Id: soft_irq.c,v 1.9 2011/01/26 20:14:19 Sasi Exp $
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 *
 */
#include <stdio.h>
#include <setjmp.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>

#include "list.h"
#include "sched_sfs.h"

void *softirqd (void *arg);

enum irq_status {
	SLEEPING = 0,
	WAKE_UP
};

struct soft_irq {
	struct list_head nxtirq;
	unsigned long id;
	void (*action) (void);
	char status; 
};

static struct list_head  sft_irq;

static struct tsk *softirq_d;

void init_soft_irq (void)
{
	struct attr attr;

	attr.prio = 1;
	attr.stksze = 20 * 1024;

	INIT_LIST_HEAD (&sft_irq);

	softirq_d = spawn_task ("SIRQ", attr, softirqd, NULL);
}

static struct soft_irq * alloc_soft_irq (void)
{
	return tm_calloc (1, sizeof(struct soft_irq));
}

void  free_softirq (struct soft_irq *p) 
{
	tm_free (p, sizeof(*p));
}

unsigned long softirq_create (void (*action) (void), int flags)
{
	struct soft_irq  * nsirq  = alloc_soft_irq ();  

	if (!nsirq) {
		return -1;
	}
	nsirq->id = (unsigned long)nsirq;
	nsirq->action = action;
	nsirq->status =  SLEEPING;
	INIT_LIST_HEAD (&nsirq->nxtirq);
	list_add_tail (&nsirq->nxtirq, &sft_irq);
	return nsirq->id;
}


void *softirqd (void *arg)
{
	while (1) {

		struct soft_irq  * sftirq = NULL;
		struct list_head  *pnode = NULL;

		list_for_each (pnode, &sft_irq) {
			sftirq = list_entry(pnode, struct soft_irq, 
					    nxtirq);
			if ((sftirq->status == WAKE_UP) && sftirq->action) {
				sftirq->action  ();
				sftirq->status = SLEEPING;
			}
		}
		add_current_task_wait_Q ();
		schedule ();
	}	
}

void softirq_wakeup (void)
{
	wakeup_task (softirq_d);
}

void wake_irq (unsigned long irqid)
{
	((struct soft_irq  *)irqid)->status = WAKE_UP;
}
