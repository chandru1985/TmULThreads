/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  $Id: iostream.c,v 1.15 2011/01/26 20:14:19 Sasi Exp $
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 *
 */

#include <stdarg.h>
#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <setjmp.h>
#include <errno.h>
#include "list.h"
#include "sched_sfs.h"


int wait_on_fd (int fd);

#define MAX_FDS  16

struct io_wait_q {
	struct list_head io_wait_task[MAX_FDS];
	int tc;
};

static struct io_wait_q io_Q;

ssize_t tm_read (int fd, char *buf, size_t size)
{
	ssize_t   rval = 0;

	do {
		rval = read (fd, buf, size);

		if (rval >= 0) {
			return rval;
		} else if (rval == -1) {
			if (errno ==  EINTR) {
				continue;
			}
		}

		wait_on_fd (fd);

	} while (1);

	return -1;
}

ssize_t tm_recvfrom(int fd, void *buf, size_t size, int flags,
                    struct sockaddr *saddr, socklen_t *len)
{
	int rval = 0;

	do {

		rval = recvfrom (fd, buf, size, flags, saddr, len);

		if (rval <= 40) {
			if (errno ==  EINTR) {
				continue;
			}
		} else if (rval > 40) {
			return rval;
		}

		wait_on_fd (fd);

	} while (1);

	return -1;
}

int tm_socket(int domain, int type, int protocol)
{
	int fd = socket(domain, type, protocol);

	if (fd == -1) {
		return -1;
	}

	setup_io_interrupt (fd);

	return fd;
}

int tm_write(int fd, const void *buf, size_t count)
{
	return -1;
}

int tm_open(const char *pathname, int flags)
{
	return -1;
}
ssize_t tm_getline (char **lineptr, size_t *n, FILE *stream)
{
	return -1;
}

int tm_close (int fd)
{

	return -1;
}

int tm_select (int nfds, fd_set *rfd, fd_set *wfd, fd_set *efd, struct timeval *t)
{
	int rval = 0;
	struct timeval tout;
	fd_set wfds,rfds,efds;
	int usec = get_current_task_tslice () * 1000;

        tout.tv_sec = 0;

	while (1) {

        	tout.tv_usec = usec;

		if (rfd) {
			FD_ZERO(&rfds);
			memcpy (&rfds, rfd, sizeof(fd_set));
		}
		if (wfd) {
			FD_ZERO(&wfds);
			memcpy (&wfds, wfd, sizeof(fd_set));
		}
		if (efd) {
			FD_ZERO(&efds);
			memcpy (&efds, efd, sizeof(fd_set));
		}

		if ((rval = select (nfds, &rfds, &wfds, &efds, &tout)) <= 0) {

			t->tv_sec -= usec;

			if (!t->tv_sec && 
   			    !t->tv_usec) {
				return rval;
			}
#if 0
			schedule ();
#endif
		}
		else 
			break;
	}
	return rval;
}

int  tm_mq_receive (int mqid , void *buf, int len,  int *prio)
{
	int rval = 0;

	do {
		rval = mq_receive(mqid , buf, len, prio);
		if (rval < 0) {
			wait_on_fd (mqid);
			continue;
		} 
		return rval;

	} while (1);

	return -1;
}

int tm_mq_open (const char *name, int oflag, mode_t mode, struct mq_attr *attr)
{
	int mqid = 0;

	mqid =  mq_open (name, oflag | O_NONBLOCK, mode, attr);

	if (mqid == -1) {
		perror ("mq_open: ");
		return -1;
	}

	setup_mq_notify (mqid);	

	return mqid;
}

int setup_mq_notify (int mqid)
{
	struct sigevent notify;

	memset (&notify, 0, sizeof(notify));

	notify.sigev_notify = SIGEV_SIGNAL;
	notify.sigev_signo = SIGIO;
	notify.sigev_notify_function = NULL;
	notify.sigev_notify_attributes = NULL;
	notify.sigev_value.sival_int = mqid;

	if (mq_notify(mqid, &notify) < 0) {
		perror ("mq_notify");
		return -1;
	}
	return 0;
}

void init_io (void)
{
	int idx = 0;

	while (idx < MAX_FDS) {
		 INIT_LIST_HEAD (&io_Q.io_wait_task[idx]);
		 idx++;
	}
	io_Q.tc = 0;
}

void wake_io_task (int fd)
{
	if (!list_empty (&io_Q.io_wait_task[fd])) {
		struct list_head *head = &io_Q.io_wait_task[fd];
		struct list_head *p, *n = NULL;
		struct tsk *ptsk = NULL;
		list_for_each_safe (p, n, head) {
			ptsk = list_entry (p, struct tsk, hd);
			list_del (&ptsk->hd);
			wakeup_task (ptsk);
		}
	}
}

void add_task_to_IO_wait_Q (struct list_head *hd, int fd)
{
	list_add_tail (hd, &io_Q.io_wait_task[fd]);	
}

struct tsk * io_wait_task_lookup (int tid)
{
	struct list_head *p = NULL;
	struct tsk *ptsk = NULL;
	int fd = 0;
	struct list_head *head = NULL;

	while (fd < MAX_FDS) {
		if (!list_empty (&io_Q.io_wait_task[fd])) {
			head = &io_Q.io_wait_task[fd];
			list_for_each (p, head) {
				ptsk = list_entry (p, struct tsk, hd);
				if (ptsk->tid == tid) 
					return ptsk;
			}
		}
		fd++;
	}
	return NULL;
}

struct list_head * io_get_next_non_empty_task_list (int *fd)
{
	while (++(*fd) < MAX_FDS) {
		if (!list_empty (&io_Q.io_wait_task[*fd]))
			return &io_Q.io_wait_task[*fd];
	}

	return NULL;
}
