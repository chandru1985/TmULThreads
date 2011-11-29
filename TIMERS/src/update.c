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

tmr_t * timer_tree_walk (struct rb_root  *root, unsigned int key, char flag);

#define IS_TMR_EXPD(ptmr)               !ptmr->ctime 

static inline void timer_expiry_action (tmr_t * ptmr)
{
	if (IS_TMR_EXPD (ptmr)) {
		DEC_TIMER_COUNT ();
		list_add_tail (&ptmr->elist, &expd_tmrs);
	}
	else
		find_tmr_slot_and_place (ptmr);
}

static inline void clear_wheel_timer (tmr_t *p)
{
	unsigned int wheel_mask [TIMER_WHEEL] = {0xFFFFFC00 , 0xFFFF03FF, 
					         0xFFC0FFFF, 0x3FFFFF};
	p->ctime &= wheel_mask[p->wheel];
}

static inline void process_timers (struct rb_root *root, unsigned int expires)
{
	tmr_t *ptmr = NULL;

	while ((ptmr = timer_tree_walk (root, expires, QUERY_TIMER_EXPIRY))) {
		clear_wheel_timer (ptmr);
		timer_del (ptmr, root);
		timer_expiry_action (ptmr);
	}
}

void tm_process_tick_and_update_timers (void)
{
	process_timers (&tmrrq.root[TICK_TIMERS], clk[TICK_TIMERS]);

	if (IS_NXT_SEC_HAPPEND) {
		process_timers (&tmrrq.root[SECS_TIMERS], clk[SECS_TIMERS]);
		if (IS_NXT_MIN_HAPPEND) {
			process_timers (&tmrrq.root[MIN_TIMERS], clk[MIN_TIMERS]);
			if (IS_NXT_HR_HAPPEND) {
				process_timers (&tmrrq.root[HRS_TIMERS], clk[HRS_TIMERS]);
			}
		}
	}
}

tmr_t * query_timer_tree_by_index (int idx)
{
	tmr_t * p = NULL;

	if ((p = timer_tree_walk (&tmrrq.root[TICK_TIMERS], idx, QUERY_TIMER_INDEX)))
		return p;

	if ((p = timer_tree_walk (&tmrrq.root[SECS_TIMERS], idx, QUERY_TIMER_INDEX)))
		return p;

	if ((p = timer_tree_walk (&tmrrq.root[MIN_TIMERS], idx, QUERY_TIMER_INDEX)))
		return p;

	if ((p = timer_tree_walk (&tmrrq.root[HRS_TIMERS], idx, QUERY_TIMER_INDEX)))
		return p;
}

void timer_add (tmr_t *n, struct rb_root *root, int flag)
{
        unsigned int cmp = 0, dest = 0;
        struct rb_node **link = &root->rb_node;
        struct rb_node *parent = NULL;
        tmr_t  *x = NULL;

	if (flag & QUERY_TIMER_EXPIRY)
		cmp = n->rt;
	if (flag & QUERY_TIMER_INDEX)
		cmp = n->idx;

        while (*link) {

                parent = *link;

                x = rb_entry(parent, tmr_t, rlist);

		if (flag & QUERY_TIMER_EXPIRY)
			dest = x->rt;
		if (flag & QUERY_TIMER_INDEX)
			dest = x->idx;

                if (cmp < dest)
                        link = &(*link)->rb_left;
                else
                        link = &(*link)->rb_right;
        }

        rb_link_node(&n->rlist, parent, link);
        rb_insert_color(&n->rlist, root);
}

void timer_del (tmr_t *n, struct rb_root *root)
{
        rb_erase (&n->rlist, root);
}

tmr_t * timer_tree_walk (struct rb_root  *root, unsigned int key, char flag)
{
        struct rb_node **p = &root->rb_node;
        struct rb_node *parent = NULL;
        tmr_t * timer = NULL;
	unsigned int cmp = 0;

        while (*p) {

                parent = *p;

                timer = rb_entry (parent, tmr_t, rlist);

		if (flag & QUERY_TIMER_EXPIRY)
	                cmp = timer->rt;
		if (flag & QUERY_TIMER_INDEX)
	                cmp = timer->idx;
                if (key < cmp)
                        p = &(*p)->rb_left;
                else if (key > cmp) 
                        p = &(*p)->rb_right;
                else 
                        return timer;
        }
        return NULL;
}
