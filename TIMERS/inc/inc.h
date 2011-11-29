
/**************************************************************************
 *  *  TechMinds's SLL LIB (C version)
 *  *-------------------------------------------------
 *  *  Copyright (C) 2008  TechMinds  (techminds@techie.com)
 *  *  
 *  *  $Id: inc.h,v 1.9 2011/01/12 16:33:33 Sasi Exp $
 *  *  
 *  *  Author     :   TechMinds
 *  *
 *  *  Description : Implementation of Generic Singly Linked List
 *  *
 *  *  Platform   :   Any
 *  *
 *  *************************************************************************/


#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <fcntl.h>
#include <termio.h>
#include <sys/types.h>     /* standard system types       */
#include <netinet/in.h>    /* Internet address structures */
#include <sys/socket.h>    /* socket interface functions  */
#include <sys/select.h>
#include <semaphore.h>
#include "list.h"
#include "task.h"
#include "rbtree.h"
#include "tmrtypes.h"

#define SUCCESS                 0
#define FAILURE                 1

#define TMR_SERVE_TIMERS 0x1

void  free_timer (tmr_t *p); 
void  btm_hlf (void);

