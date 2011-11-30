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

#define MAX_TIMERS 2000

static inline void timer_rq_init (void);
static void calc_time (tmr_t * ptmr);
static tmr_t * alloc_timer (void);
static int alloc_timer_id (void);


/************* Private Variable Declaration *********************/
static int indx = 0;
/****************************************************************/

/************* Global Variable Declaration *********************/
struct active_timers  tmrrq;
struct wait_tmr    wait_timers; 
unsigned  int clk[TIMER_WHEEL];
/****************************************************************/

int shut_timer_mgr (void)
{
	return SUCCESS;
}

static inline void update_wheel (tmr_t *p, int wheel)
{
	p->wheel = wheel;
}

int find_tmr_slot_and_place (tmr_t * ptmr)
{
	unsigned int ct = ptmr->ctime;
	unsigned int t = 0;
	int wheel = -1;

	if (GET_HRS(ct, t)) 
		wheel = HRS;
	else if (GET_MINS (ct, t)) 
		wheel = MIN;
	else if (GET_SECS (ct, t)) 
		wheel = SEC;
	else if (GET_TICK (ct, t)) 
		wheel = TICK;
	
	if (wheel >= 0) {
		ptmr->rt = (clk[wheel] + t);
		update_wheel (ptmr, wheel);
		timer_add (ptmr, &tmrrq.root[wheel], QUERY_TIMER_EXPIRY);
	}
	return 0;
}

static void calc_time (tmr_t * ptmr)
{
	unsigned int tick = ptmr->rmt;
	unsigned int secs = tick / tm_get_ticks_per_second ();
	unsigned int mins = secs / 60;
	unsigned int hrs =  mins / 60;

	SET_TICK (ptmr->ctime,  tick);
	SET_SECS (ptmr->ctime,  secs);
	SET_MINS (ptmr->ctime,  mins);
	SET_HRS  (ptmr->ctime,  hrs);
}

int start_timer (unsigned int ticks, void *data, void (*handler) (void *), int flags)
{
	tmr_t  *new = NULL;
	int idx = 0;

	if ( !(idx = alloc_timer_id ())  || 
             !(new = alloc_timer ())) {
		return -1;
	}

	INIT_LIST_HEAD (&new->elist);
	new->idx = idx;
	new->rmt = ticks;
	new->ctime = 0;
	new->rt = 0;
	new->data = data;
	new->time_out_handler = handler;

	if (flags)
		new->flags = flags;
	else
		new->flags = TIMER_ONCE;

	calc_time (new);

	find_tmr_slot_and_place (new);

	INC_TIMER_COUNT ();

	return idx;
}

int setup_timer (int *pindex, void (*handler) (void *), void *data)
{
	tmr_t  *new = NULL;
	int idx = 0;

	if ( !(idx = alloc_timer_id ())  || 
             !(new = alloc_timer ())) {
		return -1;
	}

	INIT_LIST_HEAD (&new->elist);
	new->idx = idx;
	new->rmt = 0;
	new->ctime = 0;
	new->rt = 0;
	new->data = data;
	new->time_out_handler = handler;

	new->flags = TIMER_FOREVER;

	*pindex = idx;

	update_wheel (new, WAIT_TIMERS);

	/*need to find the impact*/
	timer_add (new, &wait_timers.root, QUERY_TIMER_INDEX);

	return 0;
}

int mod_timer (int *pindex, unsigned int ticks)
{
	tmr_t * p = (tmr_t *) timer_tree_walk (&wait_timers.root , *pindex, 
                                                QUERY_TIMER_INDEX);
	if (!p)
		return -1;

	tmr_t * new = alloc_timer ();

	if (!new) {
		return -1;
	}

	INIT_LIST_HEAD (&new->elist);
	new->idx = p->idx;
	new->rmt = ticks * tm_get_ticks_per_second ();
	new->data = p->data;
	new->time_out_handler = p->time_out_handler;

	calc_time (new);

	find_tmr_slot_and_place (new);

	INC_TIMER_COUNT ();

	return 0;
}

int timer_restart  (tmr_t *p)
{
	if (!p)
		return -1;

	INIT_LIST_HEAD (&p->elist);

	calc_time (p);

	find_tmr_slot_and_place (p);

	INC_TIMER_COUNT ();

	return 0;
}


static int alloc_timer_id (void)
{
	if (TIMER_COUNT() > MAX_TIMERS) {
		return 0;
	}
	return ++indx;
}

int stop_timer (int idx)
{
	tmr_t * p = (tmr_t *) query_timer_tree_by_index (idx);

	if (!p)
		return -1;

	timer_del (p, &tmrrq.root[p->wheel]);

	free_timer (p);

	DEC_TIMER_COUNT ();

	return 0;
}

int del_timer (int *idx)
{
	stop_timer (*idx);
}

int restart_timer (int idx)
{
	tmr_t * p = (tmr_t *) query_timer_tree_by_index (idx);

	if (!p)
		return -1;

	timer_del (p, &tmrrq.root[p->wheel]);

        calc_time (p);

        find_tmr_slot_and_place (p);

	return 0;
}

static inline tmr_t * alloc_timer (void)
{
	return tm_calloc (1, sizeof(tmr_t));
}


void free_timer (tmr_t *p) 
{
	tm_free (p, sizeof(*p));
}

static inline int timers_pending_for_service (void)
{
	if (TIMER_COUNT ()) {
		if (rb_first (&tmrrq.root[TICK_TIMERS])) {
			goto service_req;
		}
		if (rb_first (&tmrrq.root[SECS_TIMERS])) {
			if (IS_NXT_SEC_HAPPEND)
				goto service_req;
		}
		if (rb_first (&tmrrq.root[MIN_TIMERS])) {
			if (IS_NXT_MIN_HAPPEND)
				goto service_req;
		}
		if (rb_first (&tmrrq.root[HRS_TIMERS])) {
			if (IS_NXT_HR_HAPPEND)
				goto service_req;
		}
	}
	return 0;
service_req:	
        return 1;
}

void update_times ()
{
	if (!(++clk[TICK] % tm_get_ticks_per_second ())) {

		if (!((++clk[SEC]) % 60) && !((++clk[MIN]) % 60)) {
			++clk[HRS];
		}
	}
	if (timers_pending_for_service ()) {
		service_timers ();
	}
}

unsigned int get_secs (void)
{
	return clk[SEC];
}
unsigned int get_ticks (void)
{
	return clk[TICK];
}

unsigned int get_mins (void)
{
	return clk[MIN];
}
unsigned int get_hrs (void)
{
	return clk[HRS];
}
unsigned int get_timers_count (void)
{
	return TIMER_COUNT();
}

void tm_test_timers_latency ()
{
	dump_task_info ();
	start_timer (1 * tm_get_ticks_per_second (), NULL, tm_test_timers_latency , 0);
}
