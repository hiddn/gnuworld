/**
 * msg_J.cc
 */

#include	<new>
#include	<string>
#include	<iostream>

#include	<cassert>

#include	"server.h"
#include	"iClient.h"
#include	"Channel.h"
#include	"ChannelUser.h"
#include	"events.h"
#include	"Network.h"
#include	"ELog.h"
#include	"StringTokenizer.h"

const char msg_J_cc_rcsId[] = "$Id: msg_J.cc,v 1.8 2001/03/24 01:31:42 dan_karrels Exp $" ;

namespace gnuworld
{

using std::string ;
using std::endl ;

/**
 * Someone has just joined a non-empty channel.
 *
 * 0AT J #coder-com
 * OAT J #coder-com,#blah
 */
int xServer::MSG_J( xParameters& Param )
{

// Verify that sufficient arguments have been provided
// client_numeric #channel[,#channel2,...]
if( Param.size() < 2 )
	{
	// Insufficient arguments provided, log the error
	elog	<< "xServer::MSG_J> Invalid number of arguments"
		<< endl ;

	// Return error
	return -1 ;
	}

// Find the client in question.
iClient* Target = Network->findClient( Param[ 0 ] ) ;

// Did we find the client?
if( NULL == Target )
	{
	// Nope, log the error
	elog	<< "xServer::MSG_J> ("
		<< Param[ 1 ] << ") Unable to find user: "
		<< Param[ 0 ]
		<< endl ;

	// Return error
	return -1 ;
	}

// Tokenize by ',', as the client may join more than one
// channel at once.
StringTokenizer st( Param[ 1 ], ',' ) ;

for( StringTokenizer::size_type i = 0 ; i < st.size() ; i++ )
	{

	// Is it a modeless channel?
	if( '+' == st[ i ][ 0 ] )
		{
		// Don't care about modeless channels
		continue ;
		}

	// Is the user parting all channels?
	if( '0' == st[ i ][ 0 ] )
		{
		// Yup, call userPartAllChannels which will update
		// the user's information and notify listening
		// services clients of the parts
		userPartAllChannels( Target ) ;

		// continue to next channel
		continue ;
		}

	// Attempt to allocate a ChannelUser structure for this
	// user<->channel association
	ChannelUser* theUser =
		new (nothrow) ChannelUser( Target ) ;
	assert( theUser != 0 ) ;

	// This variable represents which event actually occurs
	channelEventType whichEvent = EVT_JOIN ;

	// On a JOIN command, the channel should already exist.
	Channel* theChan = Network->findChannel( st[ i ] ) ;

	// Does the channel already exist?
	if( NULL == theChan )
		{
		// Nope, this transmutes to a CREATE
		// Create a new Channel to represent this
		// network channel
		theChan = new (nothrow) Channel( st[ i ], ::time( 0 ) ) ;
		assert( theChan != 0 ) ;

		// Add the channel to the network tables
		if( !Network->addChannel( theChan ) )
			{
			// Addition to network tables failed
			// Log the error
			elog	<< "xServer::MSG_J> Unable to add channel: "
				<< theChan->getName()
				<< endl ;

			// Prevent memory leaks by deallocating the
			// Channel and ChannelUser objects
			delete theChan ; theChan = 0 ;
			delete theUser ; theUser = 0 ;

			// Continue to next channel
			continue ;
			}

		// Since this is equivalent to a CREATE, set the user
		// as operator.
		theUser->setMode( ChannelUser::MODE_O ) ;

		// Update the event type
		whichEvent = EVT_CREATE ;

		} // if( NULL == theChan )

	// Otherwise, the channel was found just fine :)

	// Add a new ChannelUser representing this client to this
	// channel's user structure.
	if( !theChan->addUser( theUser ) )
		{
		// Addition of this ChannelUser to the Channel failed
		// Log the error
		elog	<< "xServer::MSG_J> Unable to add user "
			<< theUser->getNickName()
			<< " to channel: "
			<< theChan->getName()
			<< endl ;

		// Prevent memory leaks by deallocating the unused
		// ChannelUser object
		delete theUser ; theUser = 0 ;

		// Continue to next channel
		continue ;
		}

	// Add this channel to this client's channel structure.
	Target->addChannel( theChan ) ;

	// Post the event to the clients listening for events on this
	// channel, if any.
	PostChannelEvent( whichEvent, theChan,
		static_cast< void* >( Target ),
		static_cast< void* >( theUser ) ) ;

	// TODO: Update event posting so that CREATE is also
	// passed the client who created the channel

	} // for()

return 0 ;

}

void xServer::userPartAllChannels( iClient* theClient )
{
// Artifact, user is parting all channels
for( iClient::channelIterator ptr = theClient->channels_begin(),
	endPtr = theClient->channels_end() ; ptr != endPtr ; ++ptr )
	{

	// Remove this ChannelUser from the Channel's internal
	// structure.
	// Deallocate the ChannelUser
	ChannelUser* theChanUser =  (*ptr)->removeUser( theClient ) ;
	if( NULL == theChanUser )
		{
		elog	<< "xServer::userPartAllChannels> Unable to "
			<< "remove iClient "
			<< *theClient
			<< " from channel "
			<< *(*ptr)
			<< endl ;
		}
	delete theChanUser ; theChanUser = 0 ;

	// BUG: This iClient has inconsistent state because
	// no channels have been removed from its internal
	// structure until the end of this method.

	// Post this event to all listeners
	PostChannelEvent( EVT_PART,
		*ptr,
		static_cast< void* >( theClient ) ) ; // iClient*

	// Is the channel empty of all network and services
	// clients?
	if( (*ptr)->empty() )
		{
		// TODO: Post event

		// Yup, remove the channel from the network channel
		// table
		delete Network->removeChannel( (*ptr)->getName() ) ;
		}
	}

// Just to be sure
theClient->clearChannels() ;

} // userPartAllChannels()

} // namespace gnuworld
