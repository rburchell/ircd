/************************************************************************
 *   IRC - Internet Relay Chat, ircd/s_debug.c
 *   Copyright (C) 1990 Jarkko Oikarinen and
 *                      University of Oulu, Computing Center
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 1, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

char debug_id[] = "debug.c v2.0 (c) 1988 University of Oulu, Computing Center\
 and Jarkko Oikarinen";

#include "struct.h"
#include "common.h"
#include <stdio.h>
#include <sys/file.h>

extern int debuglevel;

#ifdef DEBUGMODE
int debug(level, form, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11)
int level;
char *form, *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9, *p10, *p11;
    {
	if (debuglevel >= 0)
		if (level <= debuglevel)
		    {
			fprintf(stderr, form,
				p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
			fputc('\n', stderr);
		    }
	return 0;
    }
#else
int debug()
    {
	/* do nothing so as not to waste much cpu.
	 * would make gcc very unhappy :) -avalon */
	return 0;
    }
#endif
