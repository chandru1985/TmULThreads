/*
 *  Author:
 *  Sasikanth.V        <sasikanth@email.com>
 *
 *  $Id: idx.c,v 1.1 2010/10/25 20:02:37 Sasi Exp $
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version
 *  2 of the License, or (at your option) any later version.
*/

#define NBITS_PER_BYTE  8

/********************************
 *   10     |  8    |  12 
 *******************************/
unsigned long create_idx (unsigned int nindex)
{
	int byte = nindex / NBITS_PER_BYTE;
}

unsigned long alloc_idx (unsigned long idx)
{
}

void free_idx (unsigned long idx)
{
}

void destroy_idx (unsigned long idx)
{

}
