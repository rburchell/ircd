#include "struct.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "msg.h"
#include "channel.h"
#include "stdio.h"

extern char *debugmode;
#define HUNTED_NOSUCH	(-1)	/* if the hunted server is not found */
#define	HUNTED_ISME	0	/* if this server should execute the command */
#define HUNTED_PASS	1	/* if message passed onwards successfully */

/*
** m_version
**	parv[0] = sender prefix
**	parv[1] = remote server
*/
int m_version(aClient *cptr, aClient *sptr, int parc, char **parv)
{
	if (check_registered(sptr))
		return 0;

	if (hunt_server(cptr,sptr,":%s VERSION %s",1,parc,parv)==HUNTED_ISME)
		sendto_one(sptr,":%s %d %s %s.%s %s", me.name, RPL_VERSION,
			   parv[0], version, debugmode, me.name);
	return 0;
}
