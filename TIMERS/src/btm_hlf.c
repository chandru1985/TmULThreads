/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 */

#include "inc.h"

struct list_head expd_tmrs;

void btm_hlf (void)
{

        tm_process_tick_and_update_timers ();

	if (!list_empty (&expd_tmrs)) {
		process_expd_timers ();
	}
}

void process_expd_timers (void)
{
	tmr_t 		  *ptmr = NULL;
	struct list_head  *pnode = NULL;
	struct list_head  *next = NULL;
	struct list_head  *head = &expd_tmrs;

	list_for_each_safe (pnode, next, head) {

		ptmr = list_entry (pnode, tmr_t, elist);


		if (ptmr->time_out_handler) {
			ptmr->time_out_handler (ptmr->data);
		}

		list_del (&ptmr->elist);

		if (ptmr->flags & TIMER_ONCE || ptmr->flags & TIMER_FOREVER) {
			free_timer (ptmr);
		} 
		else if (ptmr->flags & TIMER_REPEAT) {
			timer_restart (ptmr);
		}
	}
}
