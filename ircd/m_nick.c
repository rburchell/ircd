#include "struct.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "msg.h"
#include "channel.h"
#include "stdio.h"

// used with USER too
extern int register_user(aClient *cptr, aClient *sptr, char *nick);

/*
** 'do_nick_name' ensures that the given parameter (nick) is
** really a proper string for a nickname (note, the 'nick'
** may be modified in the process...)
**
**	RETURNS the length of the final NICKNAME (0, if
**	nickname is illegal)
**
**  Nickname characters are in range
**	'A'..'}', '_', '-', '0'..'9'
**  anything outside the above set will terminate nickname.
**  In addition, the first character cannot be '-'
**  or a Digit.
**
**  Note:
**	'~'-character should be allowed, but
**	a change should be global, some confusion would
**	result if only few servers allowed it...
*/

static	int do_nick_name(nick)
char	*nick;
{
	Reg1 char *ch;

	if (*nick == '-' || isdigit(*nick)) /* first character in [0..9-] */
		return 0;

	for (ch = nick; *ch && (ch - nick) < NICKLEN; ch++)
		if (!isvalid(*ch) || isspace(*ch))
			break;

	*ch = '\0';

	return (ch - nick);
}

/*
** m_nick
**	parv[0] = sender prefix
**	parv[1] = nickname
*/
int m_nick(cptr, sptr, parc, parv)
aClient *cptr, *sptr;
int parc;
char *parv[];
    {
	aClient *acptr;
	char	nick[NICKLEN+2], *s;
	
	if (parc < 2)
	    {
		sendto_one(sptr,":%s %d %s :No nickname given",
			   me.name, ERR_NONICKNAMEGIVEN, parv[0]);
		return 0;
	    }
	if (MyConnect(sptr) && (s = (char *)index(parv[1], '~')))
		*s = '\0';
	strncpyzt(nick, parv[1], NICKLEN+1);
	/*
	 * if do_nick_name() returns a null name OR if the server sent a nick
	 * name and do_nick_name() changed it in some way (due to rules of nick
	 * creation) then reject it. If from a server and we reject it,
	 * KILL it. -avalon 4/4/92
	 */
	if (do_nick_name(nick) == 0 || IsServer(cptr) && strcmp(nick, parv[1]))
	    {
		sendto_one(sptr,":%s %d %s %s :Erroneus Nickname",
			   me.name, ERR_ERRONEUSNICKNAME, parv[0], parv[1]);

		if (IsServer(cptr))
		    {
			sendto_ops("Bad Nick: %s From: %s %s",
				   parv[1], parv[0],
				   get_client_name(cptr, FALSE));
			sendto_one(cptr, ":%s KILL %s :%s (%s <- %s[%s])",
				   me.name, parv[1], me.name, parv[1],
				   nick, cptr->name);
			if (sptr != cptr) /* bad nick change */
			    {
				sendto_serv_butone(cptr,
					":%s KILL %s :%s (%s <- %s!%s@%s)",
					me.name, parv[0], me.name,
					get_client_name(cptr, FALSE),
					parv[0],
					sptr->user?sptr->user->username:"",
					sptr->user?sptr->user->server:
						   cptr->name);
				sptr->flags |= FLAGS_KILLED;
				return exit_client(cptr,sptr,&me,"BadNick");
			    }
		    }
		return 0;
	    }

	/*
	** Check against nick name collisions.[a 'for' loop is used here
	** only to be able to use 'break', these tests and actions
	** would get quite hard to follow, if done with plain if's...]
	*/
	/*
	** Put this 'if' here so that the nesting goes nicely on the screen :)
	** We check against server name list before determining if the nickname
	** is present in the nicklist (due to the way the below for loop is
	** constructed). -avalon
	*/
	if ((acptr = find_server(nick,(aClient *)NULL)) != NULL)
		if (MyConnect(sptr))
		    {
			sendto_one(sptr,
				   ":%s %d %s %s :Nickname is already in use.",
				   me.name, ERR_NICKNAMEINUSE,
				   BadPtr(parv[0]) ? "*" : parv[0], nick);
			return 0; /* NICK message ignored */
		    }
	for (;;)
	    {
		/*
		** acptr already has result from previous find_server()
		*/
		if (acptr)
		    {
			/*
			** We have a nickname trying to use the same name as
			** a server. Send out a nick collision KILL to remove
			** the nickname. As long as only a KILL is sent out,
			** there is no danger of the server being disconnected.
			** Ultimate way to jupiter a nick ? >;-). -avalon
			*/
			sendto_ops("Nick collision on %s(%s <- %s)",
				   sptr->name, acptr->from->name,
				   get_client_name(cptr, FALSE));
			sendto_one(cptr, ":%s KILL %s :%s (%s <- %s)",
				   me.name,
				   sptr->name,
				   me.name,
				   acptr->from->name,
				   /* NOTE: Cannot use get_client_name
				   ** twice here, it returns static
				   ** string pointer--the other info
				   ** would be lost
				   */
				   get_client_name(cptr, FALSE));
			sptr->flags |= FLAGS_KILLED;
			return exit_client(cptr,sptr,&me,
					   "Nick/Server collision");
		    }
		if ((acptr = find_client(nick,(aClient *)NULL)) == NULL)
			break;  /* No collisions, all clear... */
		/*
		** If acptr == sptr, then we have a client doing a nick
		** change between *equivalent* nicknames as far as server
		** is concerned (user is changing the case of his/her
		** nickname or somesuch)
		*/
		if (acptr == sptr)
		    {
			if (strcmp(acptr->name, nick) != 0)
				/*
				** Allows change of case in his/her nick
				*/
				break; /* -- go and process change */
			else
				/*
				** This is just ':old NICK old' type thing.
				** Just forget the whole thing here. There is
				** no point forwarding it to anywhere,
				** especially since servers prior to this
				** version would treat it as nick collision.
				*/
				return 0; /* NICK Message ignored */
		    }
		/*
		** Note: From this point forward it can be assumed that
		** acptr != sptr (point to different client structures).
		*/
		/*
		** If the older one is "non-person", the new entry is just
		** allowed to overwrite it. Just silently drop non-person,
		** and proceed with the nick. This should take care of the
		** "dormant nick" way of generating collisions...
		*/
		if (IsUnknown(acptr) && MyConnect(acptr))
		    {
			exit_client(cptr, acptr, &me, "Overridden");
			break;
		    }
		/*
		** Decide, we really have a nick collision and deal with it
		*/
		if (!IsServer(cptr))
		    {
			/*
			** NICK is coming from local client connection. Just
			** send error reply and ignore the command.
			*/
			sendto_one(sptr,
				   ":%s %d %s %s :Nickname is already in use.",
				   me.name, ERR_NICKNAMEINUSE,
				   parv[0], nick);
			return 0; /* NICK message ignored */
		    }
		/*
		** NICK was coming from a server connection. Means
		** that the same nick is registerd for different
		** users by different servers. This is either a
		** race condition (two users coming online about
		** same time, or net reconnecting) or just two
		** net fragments becoming joined and having same
		** nicks in use. We cannot have TWO users with
		** same nick--purge this NICK from the system
		** with a KILL... >;)
		*/
		/*
		** The client indicated by 'acptr' is dead meat, give
		** at least some indication of the reason why we are just
		** dropping it cold. ERR_NICKNAMEINUSE is not exactly the
		** right, should have something like ERR_NICKNAMECOLLISION..
		*/
		sendto_one(acptr, ":%s %d %s %s :Nickname collision, sorry.",
			   me.name, ERR_NICKNAMEINUSE,
			   acptr->name, acptr->name);
		/*
		** This seemingly obscure test (sptr == cptr) differentiates
		** between "NICK new" (TRUE) and ":old NICK new" (FALSE) forms.
		*/
		if (sptr == cptr)
		    {
			sendto_ops("Nick collision on %s(%s <- %s)",
				   acptr->name, acptr->from->name,
				   get_client_name(cptr, FALSE));
			/*
			** A new NICK being introduced by a neighbouring
			** server (e.g. message type "NICK new" received)
			*/
			sendto_serv_butone((aClient *)NULL, /* all servers */
					   ":%s KILL %s :%s (%s <- %s)",
					   me.name, acptr->name,
					   me.name,
					   acptr->from->name,
					   /* NOTE: Cannot use get_client_name
					   ** twice here, it returns static
					   ** string pointer--the other info
					   ** would be lost
					   */
					   get_client_name(cptr, FALSE));
			acptr->flags |= FLAGS_KILLED;
			return exit_client(cptr,acptr,&me,"Nick collision");
		    }
		/*
		** A NICK change has collided (e.g. message type
		** ":old NICK new". This requires more complex cleanout.
		** Both clients must be purged from this server, the "new"
		** must be killed from the incoming connection, and "old" must
		** be purged from all outgoing connections.
		*/
		sendto_ops("Nick change collision from %s to %s(%s <- %s)",
			   sptr->name, acptr->name, acptr->from->name,
			   get_client_name(cptr, FALSE));
		sendto_serv_butone(NULL, /* KILL old from outgoing servers */
				   ":%s KILL %s :%s (%s(%s) <- %s)",
				   me.name, sptr->name,
				   me.name,
				   acptr->from->name,
				   acptr->name,
				   get_client_name(cptr, FALSE));
		sendto_serv_butone(NULL, /* Kill new from incoming link */
			   ":%s KILL %s :%s (%s <- %s(%s))",
			   me.name, acptr->name,
			   me.name,
			   acptr->from->name,
			   get_client_name(cptr, FALSE),
			   sptr->name);
		acptr->flags |= FLAGS_KILLED;
		exit_client(cptr,acptr,&me,"Nick collision(new)");
		sptr->flags |= FLAGS_KILLED;
		return exit_client(cptr,sptr,&me,"Nick collision(old)");
	    }
	if (IsServer(sptr))
	    {
		/* A server introducing a new client, change source */

		sptr = make_client(cptr);
		add_client_to_list(sptr);
		if (parc > 2)
			sptr->hopcount = atoi(parv[2]);
	    }
	else if (sptr->name[0])
	    {
		/*
		** Client just changing his/her nick. If he/she is
		** on a channel, send note of change to all clients
		** on that channel. Propagate notice to other servers.
		*/
		sendto_common_channels(sptr, ":%s NICK %s", parv[0], nick);
		if (sptr->user)
			add_history(sptr);
		sendto_serv_butone(cptr, ":%s NICK %s", parv[0], nick);
	    }
	else
	    {
		/* Client setting NICK the first time */

		/* This had to be copied here to avoid problems.. */
		strcpy(sptr->name, nick);
		if (sptr->user != NULL)
			/*
			** USER already received, now we have NICK.
			** *NOTE* For servers "NICK" *must* precede the
			** user message (giving USER before NICK is possible
			** only for local client connection!). register_user
			** may reject the client and call exit_client for it
			** --must test this and exit m_nick too!!!
			*/
			if (register_user(cptr, sptr, nick) == FLUSH_BUFFER)
				return FLUSH_BUFFER;
	    }
	/*
	**  Finally set new nick name.
	*/
	if (sptr->name[0])
		del_from_client_hash_table(sptr->name, sptr);
	strcpy(sptr->name, nick);
	add_to_client_hash_table(nick, sptr);
	return 0;
}
