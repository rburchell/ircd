#************************************************************************
#*   IRC - Internet Relay Chat, ircd/Makefile
#*   Copyright (C) 1990 Jarkko Oikarinen
#*
#*   This program is free software; you can redistribute it and/or modify
#*   it under the terms of the GNU General Public License as published by
#*   the Free Software Foundation; either version 1, or (at your option)
#*   any later version.
#*
#*   This program is distributed in the hope that it will be useful,
#*   but WITHOUT ANY WARRANTY; without even the implied warranty of
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#*   GNU General Public License for more details.
#*
#*   You should have received a copy of the GNU General Public License
#*   along with this program; if not, write to the Free Software
#*   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#*/

# IRCDMODE given in top level Makefile, but added here to make sure
# compilation works if started in ircd subdirectory

CC=gcc
RM=/bin/rm
SHELL=/bin/sh

IRCDMODE=4711
INCLUDE=../include

# For MIPS, use the following:
# CFLAGS= -O -systype bsd43 -DSYSTYPE_BSD43 -I${INCLUDE}
CFLAGS= -O -I${INCLUDE}

#
#on NeXT other than 2.0:
# IRCDLIBS=-lsys_s
#
# HPUX:
# IRCDLIBS=-lBSD
#
#and otherwise:
#IRCDLIBS=

OBJS=channel.o ircd.o list.o s_bsd.o s_conf.o s_msg.o s_numeric.o \
     whowas.o s_debug.o class.o hash.o note.o s_misc.o \
     bsd.o dbuf.o packet.o send.o match.o parse.o support.o \
     m_nick.o m_who.o m_version.o

SRC=channel.c ircd.c list.c s_bsd.c s_conf.c s_msg.c s_numeric.c \
     whowas.c s_debug.c class.c note.c hash.c s_misc.c \
     bsd.c dbuf.c packet.c parse.c send.c match.c support.c \
     m_nick.c m_who.c m_version.c

RESOBJS= ../res/res_init.o ../res/res_comp.o ../res/res_query.o \
	../res/res_debug.o ../res/res_mkquery.o ../res/res_send.o \
	../res/gethostnamadr.o ../res/sethostent.o

MAKE = make 'CFLAGS=${CFLAGS}' 'CC=${CC}'

all: build

build: ircd

ircd: $(RESOBJS) $(OBJS) ../include/patchlevel.h
	$(SHELL) version.c.SH
	$(CC) $(CFLAGS) -c version.c
	$(CC) $(CFLAGS) $(RESOBJS) $(OBJS) version.o \
		-o ircd $(IRCDLIBS)
	chmod $(IRCDMODE) ircd

auth: $(OBJS) ../include/patchlevel.h
	$(SHELL) version.c.SH
	$(CC) $(CFLAGS) -c version.c
	$(CC) $(CFLAGS) $(OBJS) version.o \
		-o ircd $(IRCDLIBS)
	chmod $(IRCDMODE) ircd


authbind: $(OBJS) $(RESOBJS) ../include/patchlevel.h
	$(SHELL) version.c.SH
	$(CC) $(CFLAGS) -c version.c
	$(CC) $(CFLAgS) $(OBJS) $(RESOBJS) version.o \
		-o ircd $(IRCDLIBS)
	chmod $(IRCDMODE) ircd

saber: $(SRC)
	#load $(SRC) $(COMMONSRC) version.c $(IRCDLIBS)

../common/parse.o: ../common/parse.c ../include/msg.h ../include/config.h\
		   ../include/struct.h ../include/sys.h ../include/numeric.h
	(cd ../common; ${MAKE} build);
../common/bsd.o: ../common/bsd.c ../include/config.h ../include/common.h\
		 ../include/struct.h ../include/sys.h
	(cd ../common; ${MAKE} build);
../common/dbuf.o: ../common/dbuf.c ../include/config.h ../include/common.h\
		  ../include/struct.h ../include/dbuf.h
	(cd ../common; ${MAKE} build);
../common/packet.o: ../common/packet.c ../include/config.h ../include/common.h\
		    ../include/struct.h ../include/msg.h
	(cd ../common; ${MAKE} build);
../common/send.o: ../common/send.c ../include/config.h ../include/common.h\
		  ../include/struct.h ../include/sys.h
	(cd ../common; ${MAKE} build);
../common/match.o: ../common/match.c ../include/config.h ../include/common.h\
		   ../include/sys.h
	(cd ../common; ${MAKE} build);
../common/support.o: ../common/support.c ../include/config.h ../include/sys.h\
		     ../include/common.h
	(cd ../common; ${MAKE} build);

../res/res_init.o: ../res/res_init.c ../include/resolv.h
	(cd ../res; ${MAKE} build);
../res/res_comp.o: ../res/res_comp.c ../include/resolv.h
	(cd ../res; ${MAKE} build);
../res/res_debug.o: ../res/res_debug.c ../include/resolv.h
	(cd ../res; ${MAKE} build);
../res/res_query.o: ../res/res_query.c ../include/resolv.h
	(cd ../res; ${MAKE} build);
../res/res_mkquery.o: ../res/res_mkquery.c ../include/resolv.h
	(cd ../res; ${MAKE} build);
../res/res_send.o: ../res/res_send.c ../include/resolv.h
	(cd ../res; ${MAKE} build);
../res/gethostnamadr.o: ../res/gethostnamadr.c ../include/resolv.h
	(cd ../res; ${MAKE} build);
../res/sethostent.o: ../res/sethostent.c ../include/resolv.h
	(cd ../res; ${MAKE} build);

hash.o: ../include/struct.h ../include/sys.h ../include/hash.h hash.c \
	../include/config.h
	  $(CC) $(CFLAGS) -c hash.c
clean:
	${RM} -f *.o *~ core ircd version.c #* *.bak

depend:
	makedepend -I${INCLUDE} ${SRC}

# DO NOT DELETE THIS LINE -- make depend depends on it.

channel.o: ../include/struct.h ../include/config.h ../include/dbuf.h
channel.o: ../include/numeric.h channel.h
ircd.o: ../include/struct.h ../include/config.h
ircd.o: ../include/dbuf.h ../include/numeric.h
list.o: ../include/struct.h ../include/config.h ../include/dbuf.h
list.o: ../include/sys.h
s_bsd.o: ../include/struct.h ../include/config.h ../include/dbuf.h
s_bsd.o: ../include/sys.h
s_conf.o: ../include/struct.h ../include/config.h ../include/numeric.h
s_conf.o: ../include/dbuf.h ../include/sys.h
note.o: ../include/struct.h ../include/numeric.h ../include/config.h
s_misc.o: ../include/struct.h ../include/config.h ../include/dbuf.h
s_msg.o: ../include/struct.h ../include/config.h
s_msg.o: ../include/dbuf.h ../include/sys.h channel.h
s_msg.o: ../include/msg.h ../include/numeric.h
s_numeric.o: ../include/config.h ../include/sys.h
s_numeric.o: ../include/struct.h
s_numeric.o: ../include/dbuf.h ../include/numeric.h
whowas.o: ../include/struct.h ../include/config.h
whowas.o: ../include/dbuf.h ../include/sys.h
whowas.o: ../include/numeric.h
s_debug.o: ../include/struct.h
class.o: ../include/struct.h ../include/class.h
hash.o: ../include/struct.h ../include/sys.h ../include/hash.h
res_init.o: ../include/resolv.h ../include/nameser.h
res_comp.o: ../include/resolv.h ../include/nameser.h
res_debug.o: ../include/resolv.h ../include/nameser.h
res_query.o: ../include/resolv.h ../include/nameser.h
res_mkquery.o: ../include/resolv.h ../include/nameser.h
res_send.o: ../include/resolv.h ../include/nameser.h
bsd.o: ../include/common.h ../include/sys.h ../include/config.h
dbuf.o: ../include/config.h ../include/common.h ../include/dbuf.h
dbuf.o: ../include/sys.h
packet.o: ../include/struct.h ../include/config.h ../include/class.h
packet.o: ../include/dbuf.h ../include/common.h ../include/msg.h
parse.o: ../include/struct.h ../include/config.h
parse.o: ../include/class.h ../include/dbuf.h ../include/common.h
parse.o: ../include/msg.h ../include/sys.h
parse.o: ../include/numeric.h
send.o: ../include/struct.h ../include/config.h ../include/class.h
send.o: ../include/dbuf.h ../include/common.h
match.o: ../include/config.h ../include/common.h ../include/sys.h
support.o: ../include/config.h ../include/common.h ../include/sys.h

