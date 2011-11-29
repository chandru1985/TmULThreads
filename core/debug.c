/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 *  
 *  Description  Debug the thread execution follow code ....
 */

static int dbg_state = FALSE;

int thread_dbg_enable (void)
{
	dbg_state = TRUE;	
}

int thread_dbg_disable (void)
{
	dbg_state = FALSE;	
}


int task_debug (struct tsk *p)
{
	if (dbg_state)
		return;
	SET_TASK_STATE (p, TASK_DEBUG);
}
