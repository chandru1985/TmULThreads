/*
 *  Authors:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  $Id: tsk_stub.c,v 1.3 2011/01/16 20:00:18 Sasi Exp $
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 */

#include "inc.h"
#include <unistd.h>
#include <sys/syscall.h>

retval_t
__deinit_tsk_attr__ (tmtask_t * ptskinfo)
{
    return TSK_SUCCESS;
}

retval_t
__deinit_tsk_mtx_and_cond__ (tmtask_t * ptskinfo)
{
    return TSK_SUCCESS;
}

retval_t
start_task (tmtask_t * ptskinfo, tmtaskid_t * ptskid)
{
    return TSK_SUCCESS;
}

tmtaskid_t
tsk_selfid ()
{
}

void
__tsk_cancel__ (tmtaskid_t task_id)
{
}

retval_t
__init_tsk_mtx_and_cond__ (tmtask_t * ptskinfo)
{
    return TSK_SUCCESS;
}

retval_t
__init_tsk_attr__ (tmtask_t * ptskinfo)
{
    return TSK_SUCCESS;
}

void
__tsk_delay__ (int secs, int nsecs)
{
    while (secs--)
    {
        nsecs--;
    }
}

void
__tsk_sleep__ (int secs)
{
    while (secs--)
    {
        ;
    }
}

void
tsk_delay (int msecs)
{
    msecs *= 1000;
    while (msecs--)
    {
        ;
    }
}

pid_t
get_tsk_pid ()
{
}

int
evt_rx (int tskid, int *pevent, int event)
{
    tmtask_t           *tskinfo = get_tsk_info_frm_id (tskid);

    if (!tskinfo)
        return TSK_FAILURE;

    while (1)
    {
        if (tskinfo->curr_evt & event)
        {
            *pevent = tskinfo->curr_evt;
            tskinfo->curr_evt &= 0;
            return TSK_SUCCESS;
        }
    }
}

void
evt_snd (int tskid, int event)
{
    tmtask_t           *tskinfo = get_tsk_info_frm_id (tskid);
    tskinfo->prev_evt = tskinfo->curr_evt;
    tskinfo->curr_evt |= event;
}
