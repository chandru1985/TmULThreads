/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  $Id: interrupt.c,v 1.17 2011/01/26 20:14:19 Sasi Exp $
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
#include <signal.h>
#include <fcntl.h>
#include "list.h"
#include "sched_sfs.h"


void update_times (void);
void wake_io_task (int fd);
static inline int init_tick_timer (void);
static inline int  tick_timer (void);
static void tick_tmr_intr (int sig);


#define SYS_MAX_TICKS_IN_SEC    50 /*Since tick timer runs for 20ms: 1 sec = 1000ms (20ms * 50) */
#define TICK_TIMER_GRANULARITY  20  /*20 milli secs*/

static void tick_tmr_intr (int sig)
{
	signals_block (SIGALRM);

	++jiffies;
	update_times ();
	update_process_time ();

	if (sched_need) {
		schedule ();
	} else {
		signals_unblock (SIGALRM);
	}
}

static inline int  tick_timer (void)
{
	struct itimerval value;

	value.it_interval.tv_sec = 0;
	value.it_interval.tv_usec = TICK_TIMER_GRANULARITY * 1000;
	value.it_value = value.it_interval;
	setitimer (ITIMER_REAL, &value, NULL);
	return 0;
}

static inline int init_tick_timer (void)
{
	struct sigaction newact;

	newact.sa_handler = tick_tmr_intr;

	newact.sa_flags = SA_RESTART;

	sigemptyset (&newact.sa_mask);

	sigaction (SIGALRM, &newact, NULL);

	return 0;
}

void seg_fault (int sig)
{
	signals_block (SIGALRM);
	if (sig == SIGSEGV) {
		collect_stack_trace ();
		stack_trace ();
		exit (0);
	}
}

void setup_interrupt (void)
{
#if 0
	signal (SIGSEGV, seg_fault);
#endif

	setup_io_interrupt (fileno(stdin));

	init_tick_timer ();

	tick_timer ();
}


inline unsigned int tm_convert_msec_to_tick (unsigned int msecs)
{
	return msecs / TICK_TIMER_GRANULARITY;
}

void signals_unblock (int signo)
{
	sigset_t sig;
	sigemptyset (&sig);
	sigaddset(&sig, signo);
	sigprocmask(SIG_UNBLOCK, &sig, NULL);
}

void signals_block (int signo)
{
	sigset_t sig;
	sigemptyset (&sig);
	sigaddset(&sig, signo);
	sigprocmask(SIG_BLOCK, &sig, NULL);
}

void io_interrupt (int sig, siginfo_t *p, void *n)
{
	signals_block (SIGIO);
	if (p->si_signo == SIGIO) {
		if (p->si_code == SI_MESGQ) {
			int mqid = p->si_value.sival_int;		
			setup_mq_notify (mqid);
			wake_io_task (mqid);
		} else 
			wake_io_task (p->si_fd);
	}
	signals_unblock (SIGIO);
}

int setup_io_interrupt (int fd)
{
        int flags = fcntl(fd, F_GETFL);

        if (flags < 0 || fd >= 16)
                return -1;

        flags |= O_ASYNC | O_NONBLOCK;

        if (fcntl(fd, F_SETFL, flags) < 0) {
		return -1;
        }

        if (fcntl (fd, 10, SIGIO) < 0) {
                return -1;
        }

	fcntl(fd, F_SETOWN, getpid());

	os_register_signal_handler (SIGIO, io_interrupt);

	return 0;
}

void generic_sig_handler (int sig, siginfo_t *p, void *n)
{
	unsigned int bit_pos = (0x1 << (sig - 1));
	signals_block (sig);
	set_current_signal_pend_mask (bit_pos);
	signals_unblock (sig);
}

void os_register_signal_handler (int signo, void (*handle) (int sig, siginfo_t *p, void *n))
{
	struct sigaction newact;

	newact.sa_sigaction = handle;

	newact.sa_flags = SA_RESTART | SA_SIGINFO;

	sigemptyset (&newact.sa_mask);

	sigaction (signo, &newact, NULL);
}


void process_signal (struct tsk *p)
{
	unsigned int res = p->signals & p->sig_pending;

	int i = 1;

	do {
		if (res	&& 0x1) {
			invoke_signal_hanlder (i);
		}
		i++;
	} while (res >>= 0x1);

	p->sig_pending = 0;
}
