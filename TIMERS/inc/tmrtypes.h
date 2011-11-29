/**************************************************************************
 *  *-------------------------------------------------
 *  *  Copyright (C) 2008  TechMinds  (techminds@techie.com)
 *  *  
 *  *  $Id: tmrtypes.h,v 1.13 2011/01/16 14:52:41 Sasi Exp $
 *  *  
 *  *  Author     :   TechMinds (Sasikanth.V)
 *  *
 *  *************************************************************************/

enum {
	TICK = 0,
	SEC,
	MIN,
	HRS
};

enum {
	TICK_TIMERS = 0,
	SECS_TIMERS,
	MIN_TIMERS,
	HRS_TIMERS,
	TIMER_WHEEL,
	WAIT_TIMERS
};

struct active_timers
{
	struct rb_root   root[TIMER_WHEEL]; 
	unsigned int count;
};

struct wait_tmr {
	struct rb_root   root; 
	unsigned int count;
};

typedef struct tm_timer
{
	struct rb_node rlist; 
	struct list_head elist; 
	unsigned int    rt;
	unsigned int    ctime;
	unsigned int    rmt;
	int             wheel;
	int 	        idx;
	int	        flags;
	void           *data;
 	void           (*time_out_handler)(void *);
}tmr_t;


#define TIMER_MGR_RUNNING       2
#define TIMER_MGR_SHUTDOWN      1

#define CALCULATE_REM_TIME(ptmr)     (ptmr->duration - clk[TICK])
#define SET_TIMER_MGR_STATE(state)    tmrmgrstate = state

#define QUERY_TIMER_EXPIRY  0x1
#define QUERY_TIMER_INDEX   0x2

#define AUTO_RESTART  1


/*
 TIME FORMAT  32-BIT
-----------------------------------------
| 10 bits   | 6 bits| 6 bits|  10 bits  |
-----------------------------------------
    HRS        MINS    SECS     TICKS
*/

#define TICK_BITS       0xa
#define SECS_BITS       0x6
#define MINS_BITS       0x6
#define HRS_BITS        0xa

#define SEC_BITS_OFFSET TICK_BITS
#define MIN_BITS_OFFSET (SEC_BITS_OFFSET + SECS_BITS)
#define HRS_BITS_OFFSET (MIN_BITS_OFFSET + MINS_BITS)

#define SET_TICK(ct, t)  ct |= (t % tm_get_ticks_per_second())
#define SET_SECS(ct, t)  ct |= ((t % 60) << SEC_BITS_OFFSET)
#define SET_MINS(ct, t)  ct |= ((t % 60) << MIN_BITS_OFFSET)
#define SET_HRS(ct, t)   ct |= (t << HRS_BITS_OFFSET)

#define GET_TICK(ct, t)  (t = (~(~0U << TICK_BITS) & ct))
#define GET_SECS(ct, t)  (t = ((~(~0U << SECS_BITS)) & (ct >> SEC_BITS_OFFSET)))
#define GET_MINS(ct, t)  (t = ((~(~0U << MINS_BITS)) & (ct >> MIN_BITS_OFFSET)))
#define GET_HRS(ct, t)   (t = ((~(~0U << HRS_BITS))  & (ct >> HRS_BITS_OFFSET)))

#define INC_TIMER_COUNT()   ++tmrrq.count         
#define DEC_TIMER_COUNT()   --tmrrq.count         
#define TIMER_COUNT()      tmrrq.count

#define IS_NXT_SEC_HAPPEND     !(clk[TICK] % tm_get_ticks_per_second ())
#define IS_NXT_MIN_HAPPEND     !(clk[SEC] ? (clk[SEC] % 60) : 1)
#define IS_NXT_HR_HAPPEND      !(clk[MIN] ? (clk[MIN] % 60) : 1)

extern struct active_timers  tmrrq;
extern struct wait_tmr wait_timers;
extern struct list_head expd_tmrs;
extern unsigned int clk[];

void tm_process_tick_and_update_timers (void);
void process_expd_timers (void);
void free_timer_id ();
int find_tmr_slot_and_place (tmr_t * ptmr);
void timer_del (tmr_t *n, struct rb_root *root);
tmr_t * query_timer_tree_by_index (int idx);
void timer_add (tmr_t *n, struct rb_root *root, int);
