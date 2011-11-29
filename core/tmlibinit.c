/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *  $Id: tmlibinit.c,v 1.3 2011/01/26 20:14:19 Sasi Exp $
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/times.h>

int tmlib_init (void)
{
#ifdef SFS_WANTED
	clock_t  init_ticks = times (NULL);

	init_scheduler ();

	create_parent_task (init_ticks);

	create_idle_task ();

	init_soft_irq ();

	init_io ();
#endif
	mem_init ();

	msg_Q_init ();

	init_timer_mgr ();

#ifdef SFS_WANTED
	setup_interrupt ();
#endif

	return 0;
}

int tmlib_deinit (void)
{
#if 0
	destroy_idle_task ();

	deinit_soft_irq ();

	deinit_timer_mgr ();

	deinit_io ();

	no_setup_interrupt ();

	deinit_scheduler ();

#endif
	return 0;
}
