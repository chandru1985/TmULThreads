/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  $Id: msg_queue.c,v 1.9 2011/02/04 16:48:07 Sasi Exp $
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
#include <mqueue.h>
#include <time.h>
#include "list.h"
#include "task.h"


#define MAX_MSG_Q   10
#define MAX_Q_NAME  8

#ifndef SFS_WANTED
enum mq_flags {
	MQ_FREE = 0x1,
	MQ_ACTIVE = 0x2
};

struct msg {
	struct list_head next;
	void *msg;
};

struct Q {
	char name[MAX_Q_NAME];
	int max_msg; 
	int size;
	int mpool_id;
	int flags;
        tskmtx_t q_mtx;
        tskcnd_t q_cnd;
	struct list_head msg_list;
};

static struct Q Queue[MAX_MSG_Q];

int get_qid (void)
{
	int i = MAX_MSG_Q;

	while (--i >= 0) {
		if (Queue[i].flags == MQ_FREE) 
			return i;
	}
	return -1;
}
#endif

int  msg_Q_init (void)
{
#ifndef SFS_WANTED
	int i = MAX_MSG_Q - 1;


	while (i >= 0) {
		memset (Queue[i].name, 0, MAX_Q_NAME);
		Queue[i].max_msg = 0;
		Queue[i].size = 0;
		Queue[i].mpool_id = -1;
		Queue[i].flags = MQ_FREE;
		INIT_LIST_HEAD (&Queue[i].msg_list);
		i--;
	}
#endif
	return 0;
}

int msg_create_Q (char *name, int maxmsg, int size)
{
#ifdef SFS_WANTED
	mqd_t qid = -1;
        struct mq_attr attr;
#define  MAX_MQ_NAME 12
	char mq_name[MAX_MQ_NAME];
	
	memset (mq_name, 0, MAX_MQ_NAME);
        memset (&attr, 0, sizeof(attr));

	sprintf (mq_name, "/%s",name);

	mq_unlink (mq_name);

        attr.mq_flags = 0;
        attr.mq_maxmsg = maxmsg;
        attr.mq_msgsize = size;

        qid =  tm_mq_open (mq_name, O_RDWR | O_CREAT,
                           S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH , &attr);

        if (qid == (mqd_t)-1) {
                perror ("mq_open: ");
		return -1;
        }
	return qid;
#else
	int  qid = get_qid ();

	if (qid < 0)  {
		return -1;
	}

	Queue[qid].mpool_id = mem_pool_create (name, maxmsg * sizeof (struct msg) , 
                                               sizeof(struct msg), 0);

	if (Queue[qid].mpool_id < 0) {
		printf ("Mem Pool Creation failed ..\n");
		return -1;
	}

	memcpy (Queue[qid].name, name, MAX_Q_NAME);
	Queue[qid].max_msg = maxmsg;
	Queue[qid].size = size;
	Queue[qid].flags = MQ_ACTIVE;

	pthread_cond_init (&Queue[qid].q_cnd, NULL);
	pthread_mutex_init (&Queue[qid].q_mtx, NULL);

	return qid;

#endif
}


int msg_rcv (mqd_t qid, char **msg, int size)
{
#ifdef SFS_WANTED
	return tm_mq_receive (qid , msg, size, NULL);
#else
	struct msg * p = NULL;
	
	pthread_mutex_lock (&Queue[qid].q_mtx);

	while (1) {
		if (!list_empty (&Queue[qid].msg_list)) {
			p = list_first_entry (&Queue[qid].msg_list, struct msg,
                                               next);
			*msg = p->msg; 
			list_del (&p->next);
			free_blk (Queue[qid].mpool_id, p);
			pthread_mutex_unlock (&Queue[qid].q_mtx);
			return 0;
		}
		pthread_cond_wait (&Queue[qid].q_cnd, &Queue[qid].q_mtx);
	}
	return -1;
#endif
}

int msg_send (mqd_t qid, void *msg, int size)
{
#ifdef SFS_WANTED
        if (mq_send (qid, msg, size, 0) < 0) {
                perror("-ERR- MQ_SEND ");
        }
#else
	struct msg * p = NULL;
	pthread_mutex_lock (&Queue[qid].q_mtx);
	
	if ((p = alloc_block (Queue[qid].mpool_id))) {
		INIT_LIST_HEAD (&p->next);
		p->msg = msg;
		list_add_tail (&p->next, &Queue[qid].msg_list);
	}

	pthread_cond_signal (&Queue[qid].q_cnd);
	pthread_mutex_unlock (&Queue[qid].q_mtx);
#endif
	return 0;
}
