/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  $Id: calls.h,v 1.1 2010/11/05 16:55:27 Sasi Exp $
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 *
 */

int spawn_task(char *taskname, tskattr_t attr, void *(*tsk_wrapper) (void *), void *arg);
int task_cancel (unsigned long taskid);
int tm_sleep (unsigned int secs);
int tm_msleep (unsigned int msecs);
int wait_for_event (unsigned int events,  int *evt);
int  post_event  (unsigned long task, int event);
int thread_attr_setschedprio (tskattr_t *tsk_attr, int tsk_pri);
int thread_attr_setschedpolicy (tskattr_t *tsk_attr, int sched_alg);
int thread_attr_setstacksize (tskattr_t *tsk_attr, int tsk_stk_size);
int thread_attr_init (tskattr_t *at);
int task_selfid (void);
