/**
 * msg_M.cc
 */

#include	<new>
#include	<string>
#include	<vector>

#include	"misc.h"
#include	"events.h"

#include	"server.h"
#include	"iClient.h"
#include	"Channel.h"
#include	"ChannelUser.h"
#include	"Network.h"
#include	"ELog.h"
#include	"StringTokenizer.h"

const char msg_M_cc_rcsId[] = "$Id: msg_M.cc,v 1.3 2001/02/06 01:47:40 dan_karrels Exp $" ;

using std::string ;
using std::vector ;
using std::endl ;
using std::ends ;

namespace gnuworld
{

// Mode change
// OAD M ripper_ :+owg
//
// i M #3dx +o eAA
// J[K M DEMET_33 :+i
int xServer::MSG_M( xParameters& Param )
{

if( Param.size() < 3 )
	{
	elog	<< "xServer::MSG_M> Invalid number of arguments\n" ;
	return -1 ;
	}

// This source stuff really isn't used here, but it's here for
// debugging and validation.
iServer* serverSource = 0 ;
iClient* clientSource = 0 ;

// Note that the order of this if/else if/else is important
if( NULL != strchr( Param[ 0 ], '.' ) )
	{
	// Server, by name
	serverSource = Network->findServerName( Param[ 0 ] ) ;
	}
else if( strlen( Param[ 0 ] ) >= 3 )
	{
	// Client numeric
	clientSource = Network->findClient( Param[ 0 ] ) ;
	}
else
	{
	// 1 or 2 char numeric, server
	serverSource = Network->findServer( Param[ 0 ] ) ;
	}

if( (NULL == clientSource) && (NULL == serverSource) )
	{
	elog	<< "xServer::MSG_M> Unable to find source: "
		<< Param[ 0 ] << endl ;
	// return -1
	}

if( '#' != Param[ 1 ][ 0 ] )
	{
	onUserModeChange( Param ) ;
	return 0 ;
	}

// Find the channel in question
Channel* theChan = Network->findChannel( Param[ 1 ] ) ;
if( NULL == theChan )
	{
	elog	<< "xServer::MSG_M> Unable to find channel: "
		<< Param[ 1 ] << endl ;
	return -1 ;
	}

// Find the ChannelUser of the source client
// It is possible that the ChannelUser will be NULL, in the
// case that a server is setting the mode(s)
ChannelUser* theUser = 0 ;
if( clientSource != 0 )
	{
	theUser = theChan->findUser( clientSource ) ;
	if( NULL == theUser )
		{
		elog	<< "xServer::MSG_M> Unable to find channel "
			<< "user" << endl ;
		return -1 ;
		}
	}

bool polarity = true ;
xParameters::size_type argPos = 3 ;
vector< pair< bool, ChannelUser* > > opVector ;
vector< pair< bool, ChannelUser* > > voiceVector ;
vector< pair< bool, string > > banVector ;

for( const char* modePtr = Param[ 2 ] ; *modePtr ; ++modePtr )
	{
	switch( *modePtr )
		{
		case '+':
			polarity = true ;
			break ;
		case '-':
			polarity = false ;
			break ;
		case 't':
			onChannelModeT( theChan,
				polarity, theUser ) ;
			break ;
		case 'n':
			onChannelModeN( theChan,
				polarity, theUser ) ;
			break ;
		case 's':
			onChannelModeS( theChan,
				polarity, theUser ) ;
			break ;
		case 'p':
			onChannelModeP( theChan,
				polarity, theUser ) ;
			break ;
		case 'm':
			onChannelModeM( theChan,
				polarity, theUser ) ;
			break ;
		case 'i':
			onChannelModeI( theChan,
				polarity, theUser ) ;
			break ;
		case 'l':
			onChannelModeL( theChan,
				polarity, theUser,
				polarity ? atoi( Param[ argPos++ ] )
					: 0 ) ;
			break ;
		case 'k':
			onChannelModeK( theChan,
				polarity, theUser,
				polarity ? Param[ argPos++ ] : "" ) ;
			break ;
		case 'o':
			{
			iClient* targetClient = Network->findClient(
				Param[ argPos++ ] ) ;
			if( NULL == targetClient )
				{
				elog	<< "xServer::MSG_M> Unable to "
					<< "find op target client: "
					<< Param[ argPos - 1 ] << endl ;
				break ;
				}
			ChannelUser* targetUser = theChan->findUser(
				targetClient ) ;
			if( NULL == targetUser )
				{
				elog	<< "xServer::MSG_M> Unable to "
					<< "find op target user: "
					<< Param[ argPos - 1 ] << endl ;
				break ;
				}
			opVector.push_back(
				pair< bool, ChannelUser* >(
				polarity, targetUser ) ) ;
			break ;
			}
		case 'v':
			{
			iClient* targetClient = Network->findClient(
				Param[ argPos++ ] ) ;
			if( NULL == targetClient )
				{
				elog	<< "xServer::MSG_M> Unable to "
					<< "find voice target client: "
					<< Param[ argPos - 1 ] << endl ;
				break ;
				}
			ChannelUser* targetUser = theChan->findUser(
				targetClient ) ;
			if( NULL == targetUser )
				{
				elog	<< "xServer::MSG_M> Unable to "
					<< "find voice target user: "
					<< Param[ argPos - 1 ] << endl ;
				break ;
				}
			voiceVector.push_back(
				pair< bool, ChannelUser* >(
				polarity, targetUser ) ) ;
			break ;
			}
		case 'b':
			{
			const char* targetBan = Param[ argPos++ ] ;
			banVector.push_back(
				pair< bool, string >(
				polarity, string( targetBan ) ) ) ;
			break ;
			}

		} // switch()
	} // for()

if( !opVector.empty() )
	{
	onChannelModeO( theChan, theUser, opVector ) ;
	}
if( !voiceVector.empty() )
	{
	onChannelModeV( theChan, theUser, voiceVector ) ;
	}
if( !banVector.empty() )
	{
	onChannelModeB( theChan, theUser, banVector ) ;
	}

return 0 ;
}

void xServer::onUserModeChange( xParameters& Param )
{

// Since users aren't allowed to change modes for anyone other than
// themselves, there is no need to lookup the second user argument
// For some reason, when a user changes his/her/its modes, it still
// specifies the second argument to be nickname instaed of numeric.
iClient* theClient = Network->findNick( Param[ 1 ] ) ;
if( NULL == theClient )
	{
	elog	<< "xServer::MSG_M> Unable to find target client: "
		<< Param[ 1 ] << endl ;
	return ;
	}

// Local channels are not propogated across the network.

// It's important that the mode '+' be default
bool plus = true ;

for( const char* modePtr = Param[ 2 ] ; *modePtr ; ++modePtr )
	{
	switch( *modePtr )
		{
		case '+':
			plus = true ;
			break;
		case '-':
			plus = false ;
			break;
		case 'i':
			if( plus )
				theClient->addMode( iClient::MODE_INVISIBLE ) ;
			else
				theClient->removeMode( iClient::MODE_INVISIBLE ) ;
			break ;
		case 'k':
			if( plus )
				theClient->addMode( iClient::MODE_SERVICES ) ;
			else
				theClient->removeMode( iClient::MODE_SERVICES ) ;
			break ;
		case 'd':
			if( plus )
				theClient->addMode( iClient::MODE_DEAF ) ;
			else
				theClient->removeMode( iClient::MODE_DEAF ) ;
			break ;
		case 'w':
			if( plus )
				theClient->addMode( iClient::MODE_WALLOPS ) ;
			else
				theClient->removeMode( iClient::MODE_WALLOPS ) ;
			break ;
		case 'o':
		case 'O':
			if( plus )
				{
				theClient->addMode( iClient::MODE_OPER ) ;
				PostEvent( EVT_OPER,
					static_cast< void* >( theClient ) ) ;
				}
			else
				{
//				elog	<< "xServer::onUserModeChange> "
//					<< "Caught -o for user: "
//					<< *theClient
//					<< endl ;
				theClient->removeMode( iClient::MODE_OPER ) ;
				// TODO: Post message
				}
			break ;
		default:
			break ;
		} // close switch
	} // close for

}

} // namespace gnuworld