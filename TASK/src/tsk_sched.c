/*
 *  Authors:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  $Id: tsk_sched.c,v 1.13 2011/01/26 20:14:16 Sasi Exp $
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 */

#include "inc.h"
#include <unistd.h>
#include <sys/syscall.h>
#include "calls.h"

retval_t deinit_tsk_attr (tmtask_t * ptskinfo)
{
#if 0
    thread_attr_deinit (&ptskinfo->tsk_attr);
#endif
    return TSK_SUCCESS;
}

retval_t deinit_tsk_mtx_and_cond (tmtask_t * ptskinfo)
{
#if 0
    thread_cond_destroy (&ptskinfo->tsk_cnd);

    thread_mutex_destroy (&ptskinfo->tsk_mtx);

    thread_mutex_destroy (&ptskinfo->evt_mtx);
#endif

    return TSK_SUCCESS;
}


retval_t start_task (tmtask_t * ptskinfo, tmtaskid_t * ptskid)
{

    if (!(*ptskid = spawn_task(ptskinfo->task_name, ptskinfo->tsk_attr, ptskinfo->start_routine, 
	            (void *) ptskinfo->tskarg)))
    {
        return TSK_FAILURE;
    }

    tsk_dealloc (ptskinfo);

    return TSK_SUCCESS;
}

tmtaskid_t tsk_selfid ()
{
    return task_selfid ();
}

tmtaskid_t get_tsk_pid () {
	return tsk_selfid ();
}
void tsk_cancel (tmtaskid_t task_id)
{
    task_cancel (task_id);
}

retval_t init_tsk_mtx_and_cond (tmtask_t * ptskinfo)
{
#if 0
    thread_cond_init (&ptskinfo->tsk_cnd, NULL);

    thread_mutex_init (&ptskinfo->tsk_mtx, NULL);

    thread_mutex_init (&ptskinfo->evt_mtx, NULL);
#endif
    return TSK_SUCCESS;

}

retval_t init_tsk_attr (tmtask_t * ptskinfo)
{
    ptskinfo->tsk_attr.stksze = ptskinfo->stksze;

    ptskinfo->tsk_attr.schedalgo = ptskinfo->schedalgo;

    ptskinfo->tsk_attr.prio = ptskinfo->prio;

    return TSK_SUCCESS;
}
