/*
 * MODECommand.cc
 *
 * Changes a channel mode
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"
#include	"Constants.h"

const char MODECommand_cc_rcsId[] = "$Id: MODECommand.cc,v 1.10 2001/12/08 17:17:29 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

// LDd P AIAAA :mode #krushnet -o DawgSleep
// mode channel modes
bool MODECommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;
if( st.size() < 3 )
	{
	Usage( theClient ) ;
	return true ;
	}
if(st[1].size() > channel::MaxName)
	{
	bot->Notice(theClient,"Channel can't be more than %d chars",channel::MaxName);
	}
Channel* theChan = Network->findChannel( st[ 1 ] ) ;
if( NULL == theChan )
	{
	bot->Notice( theClient, "Unable to find channel %s\n",
		st[ 1 ].c_str() ) ;
	return true ;
	}

// Define mode to be any mode, such as +abc-def
// Define argument to to be any argument to a particular mode: -o nickArgument

// index into the st object, location of next mode
StringTokenizer::size_type modePos = 2 ;

// index into the st object, location of next argument
StringTokenizer::size_type argPos = 3 ;

// To be used later
iClient* Target = 0 ;
ChannelUser* ChanUser = NULL;
// Store the command to be sent to the network in two strings
// modeString holds the updated modes
string modeString = "" ;

// argString holds the updated arguments, delimited by space (' ')
string argString = "" ;

// Continue while there are more modes to parse
while( modePos < st.size() )
	{

	// modePos could be equal to argPos if a previous iteration of
	// the while loop found one or more modes with arguments
	if( modePos == argPos )
		{
		argPos++ ;
		}

	// This it the size of the increment to make to modePos.
	// This could be greater than 1 if there are more than one mode
	// arguments at st[ modePos ] that require arguments
	string::size_type modePosIncrement = 1 ;

	// Keep track of the polarity of the mode change.
	bool plus = true ;
	bool Op = false;
	// Iterate through the characters of the mode at st[ modePos ]
	for( string::size_type charPos = 0 ; charPos < st[ modePos ].size() ; ++charPos )
		{
		switch( toupper(st[ modePos ][ charPos ]) )
			{
			case 'O':
				Op = true;
			case 'V':

				// Make sure there is an argument for this mode
				if( argPos >= st.size() )
					{
					Usage( theClient ) ;
					return true ;
					}

				// Add the mode to the modeString
				modeString += st[ modePos ][ charPos ] ;

				// arg needs to be a nickname
				Target = Network->findNick( st[ argPos ] ) ;

				// Is the argument a valid nickname?
				if( NULL == Target )
					{
					bot->Notice( theClient, "Unable to find %s\n",
						st[ argPos ].c_str() ) ;
					return true ;
					}

				// Make sure the user is in this particular channel
				if( NULL == (ChanUser = theChan->findUser( Target ) ))
					{
					bot->Notice( theClient, "User %s was not found "
						"on channel %s",
						st[ argPos ].c_str(),
						theChan->getName().c_str() ) ;
					return true ;
					}
				//Dont deop +k				
				if((ChanUser->getMode(iClient::MODE_SERVICES)) && (!plus))
					{	
					Op = false;
					break;
					}
				if(plus)
					if(Op)
						ChanUser->setModeO() ;
					else
						ChanUser->setModeV() ;
				else
				
					if(Op)
						ChanUser->removeModeO() ;
					else
						ChanUser->removeModeV() ;
					
				// Add this nick's numeric (plus a space) to the end
				// of the current argument string
				argString += Target->getCharYYXXX() + ' ' ;

				// Move to next argument
				argPos++ ;

				// Make sure modePos skips over this argument
				modePosIncrement++ ;
				
				//Make sure to turn off op bool for next mode
				Op = false;

				break ;
			case 'B':
				// Each of these modes needs an argument
				if( argPos >= st.size() )
					{
					Usage( theClient ) ;
					return true ;
					}

				// Add this mode to the current modeString
				modeString += st[ modePos ][ charPos ] ;

				// Add this argument to the current argument string
				argString += st[ argPos ] + ' ' ;
				if(!plus)
					theChan->removeBan(st[ argPos ]);
				else
					theChan->setBan(st[ argPos ]);
				// Mode to next argument
				argPos++ ;

				// Make sure modePos skips over this argument
				modePosIncrement++ ;

				break ;

			case 'K':
				// Each of these modes needs an argument
				if( argPos >= st.size() )
					{
					Usage( theClient ) ;
					return true ;
					}

				// Add this mode to the current modeString
				modeString += st[ modePos ][ charPos ] ;

				// Add this argument to the current argument string
				argString += st[ argPos ] + ' ' ;
		
				if(plus)
					{
					theChan->setMode(Channel::MODE_K);
					theChan->setKey(st [ argPos ]);
					}
				else
					{
					theChan->removeMode(Channel::MODE_K);
					theChan->setKey("");
					}

				// Mode to next argument
				argPos++ ;

				// Make sure modePos skips over this argument
				modePosIncrement++ ;
				
				break ;
			case 'L':

				// Mode -l requires no argument
				if( !plus )
					{
					// No args needed
					modeString += st[ modePos ][ charPos ] ;
					theChan->removeMode(Channel::MODE_L);
					break ;
					}
				
				// Else, the user has specified +l, need an
				// argument.

				// Each of these modes needs an argument
				if( argPos >= st.size() )
					{
					Usage( theClient ) ;
					return true ;
					}

				theChan->setMode(Channel::MODE_L);

				// Add this mode to the current modeString
				modeString += st[ modePos ][ charPos ] ;

				// Add this argument to the current argument string
				argString += st[ argPos ] + ' ' ;

				// Mode to next argument
				argPos++ ;

				// Make sure modePos skips over this argument
				modePosIncrement++ ;

				break ;
			case 'I':  //Invite?
				if(!plus)
					theChan->removeMode(Channel::MODE_I);
				else
					theChan->setMode(Channel::MODE_I);
				modeString += st[ modePos ][ charPos ] ;

				break;
			case 'P':  //Private?
				if(!plus)
					theChan->removeMode(Channel::MODE_P);
				else
					theChan->setMode(Channel::MODE_P);
    				modeString += st[ modePos ][ charPos ] ;
				break;
			case 'S':  //Secret?
				if(!plus)
					theChan->removeMode(Channel::MODE_S);
				else
					theChan->setMode(Channel::MODE_S);
			
				modeString += st[ modePos ][ charPos ] ;

				break;
			case 'M':  //Moderated?
				if(!plus)
					theChan->removeMode(Channel::MODE_M);
				else	
					theChan->setMode(Channel::MODE_M);

				modeString += st[ modePos ][ charPos ] ;

				break;
			case 'N':  //No External Messages?
				if(!plus)
					theChan->removeMode(Channel::MODE_N);
				else					
					theChan->setMode(Channel::MODE_N);

				modeString += st[ modePos ][ charPos ] ;

				break;
			case 'T':  //Topic?
				if(!plus)
					theChan->removeMode(Channel::MODE_T);
				else
					theChan->setMode(Channel::MODE_T);
				modeString += st[ modePos ][ charPos ] ;
				break;
			case '+':
				if( plus )
					{
					// Already plus
					break ;
					}
				plus = true ;
				modeString += st[ modePos ][ charPos ] ;
				break ;
			case '-':
				if( !plus )
					{
					// Already minus
					break ;
					}
				plus = false ;
				modeString += st[ modePos ][ charPos ] ;
				break ;
			default:
				// Requires no arguments, just add the mode
				// to the modeString
				modeString += st[ modePos ][ charPos ] ;
				break ;

			} // switch()
		} // for()

	modePos += modePosIncrement ;
	} // while( modePos < st.size() )

bot->ModeAsServer( theChan, modeString + ' ' + argString ) ;

// Update internal tables.
// This is a cheat, but it makes things so much easier :)
// TODO
//theChan->OnModeChange( theClient->getCharYYXXX(), modeString, argString
//) ;

return true ;
}

}
}
