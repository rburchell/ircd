/************************************************************************
 *   IRC - Internet Relay Chat, include/sys.h
 *   Copyright (C) 1990 University of Oulu, Computing Center
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

/*
 * $Id: sys.h,v 6.1 1991/07/04 21:04:38 gruner stable gruner $
 *
 * $Log: sys.h,v $
 * Revision 6.1  1991/07/04  21:04:38  gruner
 * Revision 2.6.1 [released]
 *
 * Revision 6.0  1991/07/04  18:05:07  gruner
 * frozen beta revision 2.6.1
 *
 */

#include <string.h>
#include <time.h>
#include <sys/time.h>

#ifdef NEXT
#define VOIDSIG int	/* whether signal() returns int of void */
#else
#define VOIDSIG void	/* whether signal() returns int of void */
#endif

extern VOIDSIG dummy(), restart();

#ifdef	NO_U_TYPES
typedef	u_char	unsigned char
typedef	u_short	unsigned short
typedef	u_long	unsigned long
typedef	u_int	unsigned int
#endif
