/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *  $Id: $
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
*/

static long int cpus = 0;

struct per_cpu_sched {
	pthread_t cpu_thread;
};


int init_multi_core_arch (void)
{
	cpus = sysconf(_SC_NPROCESSORS_CONF);

	if (cpus == 0x1) { /*Only one CPU*/
		return 0;
	}

	init_per_cpu_scheduling ();

	
	create_per_cpu_os_tasks (i);
}


long int get_cpu_count ()
{
	return cpus;
}

int create_per_cpu_os_tasks (void) 
{
	int  i = 0;

	for (i = 0 ; i < cpus; i++) {

		struct sched_param  param;

		pthread_attr_t  tsk_attr;

		pthread_attr_init (&tsk_attr);

		pthread_attr_setstacksize (&tsk_attr, );

		pthread_attr_setschedpolicy (&tsk_attr, SCHED_RR);

		param.sched_priority = 20;

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
	}
}
