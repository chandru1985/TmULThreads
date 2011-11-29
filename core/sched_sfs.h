/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  $Id: sched_sfs.h,v 1.1 2011/01/26 19:59:17 Sasi Exp $
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/times.h>

#define   MAX_TASK_NAME       10
#define DEFAULT_TIME_SLICE    60
#define DEFAULT_STACK_SIZE 10*1024
/*TODO: define compiler attribute*/
#define __INTERNAL_CALL__  

#define              MAX_PRIO               20

#define IS_TASK_READY(p)  p->state == TASK_RUNNING
#define SET_TASK_STATE(p,st)  p->state = st
#define IS_TASK_NOT_ON_RQ(p)  (p)->onrq == 0

enum {
	TRUE = 1,
	FALSE = 0
};

enum tsk_state {
	TASK_INIT = 0,
	TASK_RUNNING,
	TASK_RUNNABLE,
	TASK_STOPPED,
	TASK_INTERRUPTIBLE,
	TASK_UNINTERRUPTIBLE,
	TASK_DEBUG 
};

struct signal_s {
	struct list_head next;
	int sig;
	void (*handler) (int sig);
};

struct cond{
	int cond_wait;	
	int signal;
	struct list_head next_node;
};

typedef struct lock {
	struct list_head wq;	
	struct tsk *owner;
	int         lock;
	int         prio;
}lock_t;


struct attr {
	int stksze;
	int prio;
	int schedalgo;
};

typedef struct sync_lock {
	struct list_head wq;	
	int         lock;
	int         prio;
}sync_lock_t;

struct tsk {
	struct list_head hd;	
	struct list_head sig_hd;
	struct list_head sync_hd;
	struct list_head lock_hd;
	struct list_head cond_hd;
	char             taskname[MAX_TASK_NAME + 4];
	char            *stk;
	int              ptid;
	int 		 tid;
	int 		 onrq;
	int              type; /*IO or CPU*/ /*Combined both*/
	int              cpu; /*LESS,AVG,HIGH*/
	int              stksize;        /*Stack Size*/
	int		 currt;             /*current slice in ms*/
	int		 trt;             /*Run time in ms*/
	clock_t          stime; 
	int		 state;          /*Task State*/
	int	 	 tslice;	
	int              cpuusage;
	int 		 prio;
	int              swct;
	int              needsched;
	int		 signals;
	int		 sig_pending;
	int              ntimers;
	int              event;
	int              wait_evts;
	int 		 size;
	char **		 stk_trc_sym;
	clock_t          stick; /*remove the following two*/
	clock_t          etick;
	jmp_buf 	 regs;           /*Current Execution Registers*/
};

extern unsigned long tswc;
extern unsigned long jiffies;
extern int sched_need;
extern size_t alloc_size;
extern unsigned int task_count;

void clone_thread (void *(*tsk_wrapper) (void *), void *arg);
void set_task_not_rq (void);
void schedule (void);
void wakeup_task (struct tsk *p);
void signals_block (int signo);
void signals_unblock (int signo);
struct tsk * spawn_task(char *taskname, struct attr attr, 
			void *(*tsk_wrapper) (void *), void *arg);
void add_current_task_wait_Q (void);
inline void update_process_time (void);
void add_task_to_wait_Q (struct tsk *ntsk);
int setup_io_interrupt (int fd);
int get_current_task_tslice (void);
inline unsigned int tm_get_ticks_per_second (void) ;
inline unsigned int tm_convert_msec_to_tick (unsigned int msecs);
int nsetjmp(jmp_buf env);
int nlongjmp(jmp_buf env, int val);
struct tsk * find_task (int tid);
struct tsk * io_wait_task_lookup (int tid);
struct list_head * io_get_next_non_empty_task_list (int *fd);
void add_task_to_IO_wait_Q (struct list_head *hd, int fd);
int start_timer (unsigned int ticks, void *data, void (*handler) (void *), int flags);
void *tm_calloc(size_t nmemb, size_t size);
void * tm_malloc (size_t size);
void tm_free (void *p , size_t size);
/*
#define	SIGHUP		1	 Hangup (POSIX).  
#define	SIGINT		2	 Interrupt (ANSI).  
#define	SIGQUIT		3	 Quit (POSIX).  
#define	SIGILL		4	 Illegal instruction (ANSI).  
#define	SIGTRAP		5	 Trace trap (POSIX).  
#define	SIGABRT		6	 Abort (ANSI).  
#define	SIGIOT		6	 IOT trap (4.2 BSD).  
#define	SIGBUS		7	 BUS error (4.2 BSD).  
#define	SIGFPE		8	 Floating-point exception (ANSI). 
#define	SIGKILL		9	 Kill, unblockable (POSIX).  
#define	SIGUSR1		10	 User-defined signal 1 (POSIX).  
#define	SIGSEGV		11	 Segmentation violation (ANSI).  
#define	SIGUSR2		12	 User-defined signal 2 (POSIX). 
#define	SIGPIPE		13	 Broken pipe (POSIX).  
#define	SIGALRM		14	 Alarm clock (POSIX).  
#define	SIGTERM		15	 Termination (ANSI).  
#define	SIGSTKFLT	16	 Stack fault.  
#define	SIGCLD		SIGCHLD	 Same as SIGCHD (System V).  
#define	SIGCHLD		17	 Child status has changed (POSIX).  
#define	SIGCONT		18	 Continue (POSIX).  
#define	SIGSTOP		19	 Stop, unblockable (POSIX).  
#define	SIGTSTP		20	 Keyboard stop (POSIX).  
#define	SIGTTIN		21	 Background read from tty (POSIX).  
#define	SIGTTOU		22	 Background write to tty (POSIX).  
#define	SIGURG		23	 Urgent condition on socket (4.2 BSD).  
#define	SIGXCPU		24	 CPU limit exceeded (4.2 BSD).  
#define	SIGXFSZ		25	 File size limit exceeded (4.2 BSD).  
#define	SIGVTALRM	26	 Virtual alarm clock (4.2 BSD).  
#define	SIGPROF		27	 Profiling alarm clock (4.2 BSD).  
#define	SIGWINCH	28	 Window size change (4.3 BSD, Sun).  
#define	SIGPOLL		SIGIO	 Pollable event occurred (System V).  
#define	SIGIO		29	 I/O now possible (4.2 BSD).  
#define	SIGPWR		30	 Power failure restart (System V).  
*/

void generic_sig_handler (int sig, siginfo_t *p, void *n);
void os_register_signal_handler (int signo, void (*handle) (int sig, siginfo_t *p, void *n));
void wake_irq (unsigned long irqid);
void process_signal (struct tsk *);
void invoke_signal_hanlder (int signo);
void set_current_task_time_slice (unsigned int tslice);
struct tsk *get_current ();
