/* 
 * OPERJOINCommand.cc 
 *
 * 10/02/2001 - David Henriksen <david@itwebnet.dk>
 * Initial Version. Written, and finished.
 *
 * Allows an oper to make cmaster join a registered channel.
 *
 * Caveats: None
 *
 * $Id: OPERJOINCommand.cc,v 1.1 2001/02/10 23:34:02 gte Exp $
 */


#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"
#include	"Network.h"

const char OPERJOINCommand_cc_rcsId[] = "$Id: OPERJOINCommand.cc,v 1.1 2001/02/10 23:34:02 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool OPERJOINCommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
	if( st.size() < 2 )
	{
		Usage(theClient);
		return true;
	}

	/* 
	 *  Check the channel is actually registered.
	 */

	Channel* tmpChan = Network->findChannel(st[1]);
	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if (!theChan) {
		bot->Notice(theClient, "The channel %s doesn't appear to be registered",
			st[1].c_str());
		return false;
	} 

	/*
	 *  Check the user is an oper.
	 */

	if(!theClient->isOper())
		{
		bot->Notice(theClient, "This command is reserved to IRC Operators");
		return true;
		}

 	/* Check the bot isn't in the channel. */ 
	if (theChan->getInChan()) {
		bot->Notice(theClient, "I'm already in that channel!");
		return false;
	}
 
	strstream s;
	
	s	<< server->getCharYY() << " WA :"
		<< theClient->getNickUserHost() << " is asking me to join "
		<< theChan->getName() << ends;
	bot->Write(s);
	delete[] s.str();
 
	theChan->setInChan(true);
	bot->getUplink()->RegisterChannelEvent( theChan->getName(), bot);
	bot->Join(theChan->getName(), theChan->getChannelMode(), theChan->getChannelTS(), false);

	/* Whack this reop on the Q */
	bot->reopQ.push(cservice::reopQType::value_type(bot->currentTime() + 15, theChan->getName()) );
 
	if (tmpChan)
		{
		if(theChan->getFlag(sqlChannel::F_NOOP)) bot->deopAllOnChan(tmpChan);
		if(theChan->getFlag(sqlChannel::F_STRICTOP)) bot->deopAllUnAuthedOnChan(tmpChan);
		}
		
	
	return true;
} 

} // namespace gnuworld.