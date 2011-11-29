/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  $Id: lock.c,v 1.5 2011/01/26 20:14:19 Sasi Exp $
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
*/

#include <stdio.h>
#include <setjmp.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include "list.h"
#include "sched_sfs.h"

int create_sync_lock (sync_lock_t *slock, int type, int count)
{
	slock->lock = 0;
	slock->prio = 0;
	INIT_LIST_HEAD (&slock->wq);
	return 0;
}

int sync_lock (sync_lock_t *slock)
{
	while (slock->lock) {
		list_del (&(get_current())->hd);
		list_add_tail (&(get_current())->hd, &slock->wq);
		set_task_not_rq ();
	        signals_block (SIGALRM);
		schedule ();
	}

	++slock->lock;

	if (slock->lock > 1) {
		raise (SIGSEGV);
	}

	
	return 0;
}
int sync_unlock (sync_lock_t *slock)
{
	if (!slock->lock) {
		return -1;
	}

	--slock->lock;	

	if (!list_empty (&slock->wq)) {
		struct tsk *p = NULL;
		p= list_first_entry (&slock->wq, struct tsk, hd);
		wakeup_task (p);
	        signals_block (SIGALRM);
		schedule ();
	}
	return 0;
}

int create_lock (lock_t *lock)
{
	lock->lock = 0;
	lock->owner = NULL;
	lock->prio = 0;
	INIT_LIST_HEAD (&lock->wq);
	return 0;
}

int lock (lock_t *lock)
{
	while (lock->lock) {
		list_del (&(get_current())->hd);
		list_add_tail (&(get_current())->hd, &lock->wq);
		set_task_not_rq ();
	        signals_block (SIGALRM);
		schedule ();
	}

	++lock->lock;
	
	if (lock->lock > 1) {
		raise (SIGSEGV);
	}

	lock->owner = get_current ();

	return 0;
}

int unlock (lock_t *lock)
{
	if (lock->owner != get_current ())  {
		return -1;
	}

	--lock->lock;	

	if (lock->lock < 0) {
		raise (SIGSEGV);
	}

	lock->owner = NULL;

	if (!list_empty (&lock->wq)) {
		struct tsk *p = NULL;
		p= list_first_entry (&lock->wq, struct tsk, hd);
		wakeup_task (p);
	}

	return 0;
}
