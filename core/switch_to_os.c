/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *  $Id: switch_to_os.c,v 1.2 2011/01/26 20:14:19 Sasi Exp $
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 *
*/

#include <pthread.h>
#include <sched.h>
#include <sys/types.h>
#include "list.h"
#include "sched_sfs.h"

void *os_thread (void *arg)
{
	struct tsk *p = (struct tsk *)arg;
	nlongjmp (p->regs, 1);
	return NULL;
}

int create_os_thread (struct tsk *p)
{
#if 1
    struct sched_param  param;

    pthread_attr_t  tsk_attr;

    pthread_attr_init (&tsk_attr);

    pthread_attr_setstacksize (&tsk_attr, p->stksize);

    pthread_attr_setschedpolicy (&tsk_attr, SCHED_RR);

    param.sched_priority = p->prio;

    pthread_attr_setschedparam (&tsk_attr, (const struct sched_param *)&param);

    switch (1) {
        case 0:                /* For Root User */
            pthread_attr_setinheritsched (&tsk_attr,
                                          PTHREAD_EXPLICIT_SCHED);
            break;
        default:                /*For Other Users */
            pthread_attr_setinheritsched (&tsk_attr,
                                          PTHREAD_INHERIT_SCHED);
            break;
    }

    if (pthread_create ((pthread_t *)&p->ptid, &tsk_attr, os_thread, (void *) p)) {
	return -1;
    }
#endif
    return 0;
}
