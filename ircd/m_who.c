#include "struct.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "msg.h"
#include "channel.h"
#include "stdio.h"

extern aClient *client;


static void do_who(aClient *sptr, aClient *acptr, aChannel *repchan)
{
  char stat[5];
  int i = 0;

  if (acptr->user->away)
    stat[i++] = 'G';
  else
    stat[i++] = 'H';
  if (IsAnOper(acptr))
    stat[i++] = '*';
  if (repchan && is_chan_op(acptr, repchan))
    stat[i++] = '@';
  stat[i] = '\0';
  sendto_one(sptr,":%s %d %s %s %s %s %s %s %s :%d %s",
	     me.name, RPL_WHOREPLY, sptr->name,
	     (repchan) ? (repchan->chname) : "*",
	     acptr->user->username,
	     acptr->user->host,
	     acptr->user->server, acptr->name,
	     stat, acptr->hopcount, acptr->info);
}


/*
** m_who
**	parv[0] = sender prefix
**	parv[1] = nickname mask list
**	parv[2] = additional selection flag, only 'o' for now.
*/
int m_who(aClient *cptr, aClient *sptr, int parc, char **parv)
{
	Reg1 aClient *acptr;
	Reg2 char *mask = parc > 1 ? parv[1] : NULL;
	Reg3 Link *link;
	aChannel *chptr;
	aChannel *mychannel;
	char *channame = (char *) 0;
	int oper = parc > 2 ? (*parv[2] == 'o' ): 0; /* Show OPERS only */
	int member;

	mychannel = NullChn;
	if (sptr->user)
		if (link = sptr->user->channel)
			mychannel = link->value.chptr;

	/* Allow use of m_who without registering */
	
	/*
	**  Following code is some ugly hacking to preserve the
	**  functions of the old implementation. (Also, people
	**  will complain when they try to use masks like "12tes*"
	**  and get people on channel 12 ;) --msa
	*/
	if (mask == NULL || *mask == '\0')
		mask = NULL;
	else if (mask[1] == '\0' && mask[0] == '*')
	    {
		mask = NULL;
		if (mychannel)
			channame = mychannel->chname;
	    }
	else if (mask[1] == '\0' && mask[0] == '0') /* "WHO 0" for irc.el */
		mask = NULL;
	else
		channame = mask;
	sendto_one(sptr,":%s %d %s Channel User Host Server Nickname S :Name",
		    me.name, RPL_WHOREPLY, parv[0]);
	if (channame && *channame == '#')
	    {
		/*
		 * List all users on a given channel
		 */
		chptr = find_channel(channame, (aChannel *)NULL);
		if (chptr != (aChannel *)NULL)
		    if ((member = IsMember(sptr, chptr)) ||
			  !SecretChannel(chptr))
			for (link = chptr->members; link; link = link->next)
			    {
				if (oper && !IsAnOper(link->value.cptr))
					continue;
				if (IsInvisible(link->value.cptr) && !member)
					continue;
				do_who(sptr, link->value.cptr, chptr);
			    }
	    }
	else for (acptr = client; acptr; acptr = acptr->next)
	    {
		aChannel *ch2ptr = NULL;
		int showperson, isinvis;

		if (!IsPerson(acptr))
			continue;
		if (oper && !IsAnOper(acptr))
			continue;
		showperson = 0;
		/*
		** This is brute force solution, not efficient...? ;( 
		** Show entry, if no mask or any of the fields match
		** the mask. --msa
		*/
		if (mask == NULL ||
		    matches(mask,acptr->name) == 0 ||
		    matches(mask,acptr->user->username) == 0 ||
		    matches(mask,acptr->user->host) == 0 ||
		    matches(mask,acptr->user->server) == 0 ||
		    matches(mask,acptr->info) == 0)
			showperson = 1;
		if (!showperson)
			continue;
		showperson = 0;
		/*
		 * Show user if they are on the same channel, or not
		 * invisible and on a non secret channel (if any).
		 */
		isinvis = IsInvisible(acptr);
		for (link = acptr->user->channel; link; link = link->next)
		    {
			chptr = link->value.chptr;
			member = IsMember(sptr, chptr);
			if (isinvis && !member)
				continue;
			if (member || !isinvis && PubChannel(chptr))
			    {
				ch2ptr = chptr;
				showperson = 1;
				break;
			    }
			if (HiddenChannel(chptr) && !isinvis &&
			    !SecretChannel(chptr))
				showperson = 1;
		    }
		if (!acptr->user->channel && !isinvis)
			showperson = 1;
		if (showperson)
			do_who(sptr, acptr, ch2ptr);
	    }
	sendto_one(sptr, ":%s %d %s %s :* End of /WHO list.", me.name,
		   RPL_ENDOFWHO, parv[0], parv[1] ? parv[1] : "*");
	return 0;
}


