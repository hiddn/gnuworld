/**
 * snoop.cc
 */

#include	<string>
#include	<sstream>
#include	<iostream>

#include	"server.h"
#include	"client.h"
#include	"snoop.h"
#include	"Channel.h"
#include	"iClient.h"
#include	"Network.h"
#include	"EConfig.h"
#include	"StringTokenizer.h"
#include	"misc.h"

namespace gnuworld
{

using std::endl ;
using std::string ;
using std::stringstream ;

extern "C"
{
xClient* _gnuwinit( const string& args )
	{
	return new snoop( args ) ;
	}
}

snoop::snoop( const string& confFileName )
 : xClient( confFileName )
{
EConfig conf( confFileName ) ;
cmdchar = conf.Require( "cmdchar" )->second ;
adminChanName = conf.Require( "adminchan" )->second ;
relayChanName = conf.Require( "relaychan" )->second ;
defaultQuitMessage = conf.Require( "defaultquitmessage" )->second ;
}

snoop::~snoop()
{}

bool snoop::BurstChannels()
{
getUplink()->JoinChannel( this, adminChanName ) ;
getUplink()->JoinChannel( this, relayChanName ) ;
return xClient::BurstChannels() ;
}

void snoop::OnChannelMessage( iClient* srcClient,
	Channel* theChan,
	const string& Message )
{
if( strcasecmp( theChan->getName(), adminChanName ) )
	{
	// Not the admin chan, ignore
	return ;
	}

if( !srcClient->isOper() )
	{
	return ;
	}

StringTokenizer st( Message ) ;
if( st.empty() || st[ 0 ] != cmdchar )
	{
	return ;
	}

//elog	<< "snoop::OnChannelMessage> Received chan message: "
//	<< st.assemble( 0 )
//	<< endl ;

if( st.size() < 2 )
	{
	return ;
	}

string command( st[ 1 ] ) ;
string_lower( command ) ;

if( command == "spawnclient" )
	{
	handleSpawnClient( srcClient, theChan, st ) ;
	}
else if( command == "spawnjoin" )
	{
	handleSpawnJoin( srcClient, theChan, st ) ;
	}
else if( command == "spawnpart" )
	{
	handleSpawnPart( srcClient, theChan, st ) ;
	}
else if( command == "spawnquit" )
	{
	handleSpawnQuit( srcClient, st ) ;
	}
else if( command == "reload" )
	{
	getUplink()->UnloadClient( this,
		"Something has changed in the matrix..." ) ;
	getUplink()->LoadClient( "libsnoop.la",
		getConfigFileName() ) ;
	}
else if( command == "shutdown" )
	{
	getUplink()->Shutdown() ;
	}
}

void snoop::handleSpawnClient( iClient* srcClient,
	Channel* theChan,
	const StringTokenizer& st )
{
// snoop spawnclient nick!user@host realname
if( st.size() < 4 )
	{
	usage( srcClient, "spawnclient" ) ;
	return ;
	}

const string realname( st.assemble( 3 ) ) ;

//elog	<< "snoop::OnChannelMessage> nickTokens, st[ 2 ]: "
//	<< st[ 2 ]
//	<< endl ;

// st[ 2 ] is nick!user@host:realname
StringTokenizer nickTokens( st[ 2 ], '!' ) ;
if( nickTokens.size() != 2 )
	{
	usage( srcClient, "spawnclient" ) ;
	return ;
	}

StringTokenizer userTokens( nickTokens[ 1 ], '@' ) ;
if( userTokens.size() != 2 )
	{
	usage( srcClient, "spawnclient" ) ;
	return ;
	}

const string nickname( nickTokens[ 0 ] ) ;
const string username( userTokens[ 0 ] ) ;
const string hostname( userTokens[ 1 ] ) ;

if( Network->findNick( nickname ) != 0 )
	{
	Notice( srcClient, "SPAWNCLIENT: Nickname %s already exists",
		nickname.c_str() ) ;
	return ;
	}

if( string::npos == hostname.find( '.' ) )
	{
	Notice( srcClient, "SPAWNCLIENT: Hostname must appear valid" ) ;
	return ;
	}

char newCharYY[ 6 ] ;
newCharYY[ 2 ] = 0 ;
inttobase64( newCharYY, MyUplink->getIntYY(), 2 ) ;

iClient* newClient = new (std::nothrow) iClient(
	MyUplink->getIntYY(), // intYY
	newCharYY, // charYYXXX
        nickname,
	username,
	"AAAAAA", // host base 64
	hostname,
	"realInsecureHost.com", // realInsecureHost
	"+i", // mode
	string(), // account
	realname,
	31337 // connect time
	) ;
assert( newClient != 0 ) ;

if( !getUplink()->AttachClient( newClient, this ) )
	{
	elog    << "snoop::handleSpawnClient> Failed to add new client: "
		<< *newClient
		<< endl ;

	Notice( srcClient, "Failed to create new fake client" ) ;
	delete newClient ; newClient = 0 ;
	}

// Each relay (spawn) client must join the relay chan
if( !getUplink()->JoinChannel( newClient, relayChanName ) )
	{
	Notice( srcClient, "Unable to make \'%s\' join channel "
		"\'%s\', killing off client",
		st[ 2 ].c_str(),
		st[ 3 ].c_str() ) ;

	getUplink()->DetachClient( newClient ) ;
	delete newClient ; newClient = 0 ;
	return ;
	}

Message( theChan, "Successfully created nick %s and joined "
	"relay channel %s",
	nickname.c_str(),
	relayChanName.c_str() ) ;
}

void snoop::handleSpawnJoin( iClient* srcClient,
	Channel* theChan,
	const StringTokenizer& st )
{
// snoopy spawnjoin <nick> <chan>
if( st.size() != 4 )
	{
	usage( srcClient, "spawnjoin" ) ;
	return ;
	}

// Find the client
iClient* fakeClient = Network->findNick( st[ 2 ] ) ;
if( 0 == fakeClient )
	{
	Notice( srcClient, "Nick \'%s\' does not exist",
		st[ 2 ].c_str() ) ;
	return ;
	}

// Verify that it is a fake client, and owned by this module
xClient* ownerClient = Network->findFakeClientOwner( fakeClient ) ;
if( ownerClient != this )
	{
	Notice( srcClient, "I don't own that client!" ) ;
	return ;
	}

// fakeClient is valid, and owned by this module

if( !getUplink()->JoinChannel( fakeClient, st[ 3 ] ) )
	{
	Notice( srcClient, "Unable to make \'%s\' join channel "
		"%s",
		st[ 2 ].c_str(),
		st[ 3 ].c_str() ) ;
	}
else
	{
	Message( theChan, "%s successfully joined %s",
		st[ 2 ].c_str(),
		st[ 3 ].c_str() ) ;
	}
}

void snoop::handleSpawnPart( iClient* srcClient,
	Channel* theChan,
	const StringTokenizer& st )
{
// snoopy spawnpart <nick> <chan>
if( st.size() != 4 )
	{
	usage( srcClient, "spawnpart" ) ;
	return ;
	}

// Find the client
iClient* fakeClient = Network->findNick( st[ 2 ] ) ;
if( 0 == fakeClient )
	{
	Notice( srcClient, "Nick \'%s\' does not exist",
		st[ 2 ].c_str() ) ;
	return ;
	}

// Verify that it is a fake client, and owned by this module
xClient* ownerClient = Network->findFakeClientOwner( fakeClient ) ;
if( ownerClient != this )
	{
	Notice( srcClient, "I don't own that client!" ) ;
	return ;
	}

// fakeClient is valid, and owned by this module

getUplink()->PartChannel( fakeClient, st[ 3 ] ) ;
Message( theChan, "%s successfully parted %s",
	st[ 2 ].c_str(),
	st[ 3 ].c_str() ) ;
}

void snoop::usage( iClient* srcClient,
	const string& command )
{
if( command == "spawnclient" )
	{
	Notice( srcClient, "Usage: spawnclient <nick!user@host <realname>" ) ;
	}
else if( command == "spawnjoin" )
	{
	Notice( srcClient, "Usage: spawnjoin <nick> <chan>" ) ;
	}
else if( command == "spawnpart" )
	{
	Notice( srcClient, "Usage: spawnpart <nick> <chan>" ) ;
	}
else if( command == "spawnquit" )
	{
	Notice( srcClient, "Usage: spawnquit <nick> [quit message]" ) ;
	}
}

void snoop::OnFakeChannelMessage( iClient* srcClient,
	iClient* fakeClient,
	Channel* theChan,
	const string& Message )
{
// A fake client received a channel message
// Ignore if it's the admin channel
if( !strcasecmp( theChan->getName(), adminChanName ) )
	{
	return ;
	}

// For now simply relay to the relay chan without checking for
// state, etc
Channel* relayChan = Network->findChannel( relayChanName ) ;
if( 0 == relayChan )
	{
	elog	<< "snoop::OnFakeChannelMessage> Fake client: "
		<< *fakeClient
		<< " unable to find relay chan: "
		<< relayChanName
		<< endl ;
	return ;
	}

// Don't relay to the relay channel
if( relayChan == theChan )
	{
	return ;
	}

stringstream s ;
s	<< "[RELAY "
	<< theChan->getName()
	<< "] "
	<< srcClient->getNickName()
	<< "!"
	<< srcClient->getUserName()
	<< "@"
	<< srcClient->getInsecureHost()
	<< ": "
	<< Message ;

FakeMessage( relayChan,
	fakeClient,
	s.str().c_str() ) ;
}

void snoop::handleSpawnQuit( iClient* srcClient,
	const StringTokenizer& st )
{
// snoopy spawnquit <nick> [message]
if( st.size() < 3 )
	{
	usage( srcClient, "spawnquit" ) ;
	return ;
	}

string quitMessage( defaultQuitMessage ) ;
if( st.size() >= 4 )
	{
	quitMessage = st.assemble( 3 ) ;
	}

// Find the client
iClient* fakeClient = Network->findNick( st[ 2 ] ) ;
if( 0 == fakeClient )
	{
	Notice( srcClient, "Nick \'%s\' does not exist",
		st[ 2 ].c_str() ) ;
	return ;
	}

// Verify that it is a fake client, and owned by this module
xClient* ownerClient = Network->findFakeClientOwner( fakeClient ) ;
if( ownerClient != this )
	{
	Notice( srcClient, "I don't own that client!" ) ;
	return ;
	}

getUplink()->DetachClient( fakeClient, quitMessage ) ;
Notice( srcClient, "%s detached",
	st[ 2 ].c_str() ) ;

// This module allocated the iClient, and it must therefore deallocate
// it.
delete fakeClient ; fakeClient = 0 ;

}

} // namespace gnuworld
