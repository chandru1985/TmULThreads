/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  $Id: common.c,v 1.3 2011/02/04 16:46:58 Sasi Exp $
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
#include <fcntl.h>
#include <mqueue.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/times.h>

ssize_t tm_read (int fd, char *buf, size_t size)
{
	return read (fd, buf, size);
}
ssize_t tm_recvfrom(int fd, void *buf, size_t size, int flags,
                    struct sockaddr *saddr, socklen_t *len)
{
	ssize_t rval = 0;
	do {

		rval = recvfrom (fd, buf, size, flags, saddr, len);

		if (rval <= 40) {
			if (errno ==  EINTR) {
				continue;
			}
		} else if (rval > 40) {
			return rval;
		}

	} while (1);

	return -1;
}

int tm_socket(int domain, int type, int protocol)
{
	return socket(domain, type, protocol);
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
	return select (nfds, rfd, wfd, efd, t);
}

int  tm_mq_receive (int mqid , void *buf, int len,  unsigned int *prio)
{
	return mq_receive(mqid , buf, len, prio);
}

int tm_mq_open (const char *name, int oflag, mode_t mode, struct mq_attr *attr)
{
	int mqid = 0;

	mqid =  mq_open (name, oflag , mode, attr);

	if (mqid == -1) {
		perror ("mq_open: ");
		return -1;
	}

	return mqid;
}

int sleep_forever ()
{
	sleep (-1);
}
