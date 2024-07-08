/**
 * cservice.h
 * Author: Greg Sikorski
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 *
 * $Id: cservice.h,v 1.116 2010/08/31 21:16:46 denspike Exp $
 */

#ifndef __CSERVICE_H
#define __CSERVICE_H "$Id: cservice.h,v 1.116 2010/08/31 21:16:46 denspike Exp $"

#include	<iostream>
#include	<string>
#include	<vector>
#include	<map>

#include	<ctime>

#include	"misc.h"
#include	"client.h"
#include	"iClient.h"
#include	"iServer.h"
#include	"EConfig.h"
#include	"cservice_config.h"
#include	"cserviceCommands.h"
#include	"sqlChannel.h"
#include	"sqlUser.h"
#include	"sqlLevel.h"
#include	"sqlBan.h"
#include	"sqlPendingChannel.h"
#include	"csGline.h"
#include	"dbHandle.h"

namespace gnuworld
{

using std::string ;
using std::vector ;
using std::map ;

/*
 * Class for the configuration variables extracted from the database.
 * An attempted conversion to a number is stored in 'int_value' for
 * convenience.
 */

class ConfigData
{

public:
	string string_value;
	unsigned int int_value;

	inline const string&	asString() const
		{ return string_value ; }

	inline const unsigned int&	asInt() const
		{ return int_value ; }
};

/**
 *  Sublcass the postgres API to create our own accessor
 *  to get at the PID information.
 */
class Command;

class cservice : public xClient
{
protected:

	EConfig* cserviceConfig; /* Configfile */
	typedef map< string, Command*, noCaseCompare > commandMapType ;
	typedef commandMapType::value_type pairType ;
	typedef map< string, unsigned int > ipMapType ;
	ipMapType		ipFloodMap;
	commandMapType          commandMap;
#ifdef USE_COMMAND_LOG
	string			commandlogPath;
	std::ofstream		commandLog;
#endif

public:
	dbHandle* SQLDb; /* PostgreSQL Database */
	string confSqlHost;
	string confSqlPass;
	string confSqlDb;
	string confSqlPort;
	string confSqlUser;
	string confCCChan;
	unsigned int connectRetries;
	unsigned int connectCheckFreq;
	unsigned int connectRetry;
	unsigned int limitCheckPeriod;
	enum {
		AUTH_SUCCEEDED,
		AUTH_FAILED,
		TOO_EARLY_TOLOGIN,
		AUTH_UNKNOWN_USER,
		AUTH_SUSPENDED_USER,
		AUTH_NO_TOKEN,
		AUTH_INVALID_PASS,
		AUTH_ERROR,
		AUTH_INVALID_TOKEN,
		AUTH_FAILED_IPR,
		AUTH_ML_EXCEEDED
	};
	void checkDbConnectionStatus();
	string pendingPageURL;

	cservice(const string& args);
	virtual ~cservice();

	virtual void OnConnect();
	virtual void BurstChannels();
	virtual void OnPrivateMessage( iClient*, const string&,
		bool = false  );
	virtual void OnChannelCTCP( iClient*, Channel*,	const string&, const string& ) ;
	virtual void OnChannelMessage( iClient*, Channel*, const string& );
	virtual void OnChannelNotice( iClient*, Channel*, const string& );
	void handleChannelPart( iClient*, Channel*, const string& );
	virtual void OnAttach() ;
	virtual void OnShutdown(const string&);
	virtual bool isOnChannel( const string& ) const;
	virtual bool RegisterCommand( Command* ) ;
	virtual bool UnRegisterCommand( const string& ) ;
	virtual void OnChannelModeV( Channel*, ChannelUser*,
		const xServer::voiceVectorType& ) ;
	virtual void OnChannelModeO( Channel*, ChannelUser*,
		const xServer::opVectorType& ) ;
	virtual void OnChannelEvent( const channelEventType& whichEvent,
		Channel* theChan,
		void* data1, void* data2, void* data3, void* data4 );
	virtual void OnEvent( const eventType&,
		void*, void*, void*, void*);
	virtual void OnCTCP( iClient* Sender,
                const string& CTCP,
                const string& Message,
                bool Secure = false ) ;
	virtual void OnTimer(const xServer::timerID&, void*);
	virtual void OnJoin( const std::string& ) ;
	virtual bool Notice( const iClient* Target,
		const char* Message, ... ) ;
	virtual bool Notice( const iClient* Target, const string& ) ;
	virtual bool Notice( const std::string& Channel,
		const char* Message, ... ) ;
	virtual bool Notice( const Channel* theChan,
		const char* Message, ... ) ;
	virtual bool Notice( const Channel*, const std::string& ) ;

	virtual bool Topic( Channel*, const std::string& ) ;

	virtual void OnWhois( iClient* sourceClient,
			iClient* targetClient );

	virtual bool Kick( Channel*, iClient*, const std::string&, bool modeAsServer = false ) ;

	virtual bool Kick( Channel*, const std::vector<iClient*>&, const std::string&, bool modeAsServer = false ) ;

	virtual bool Kick( Channel*, const string&,	const std::string&, bool modeAsServer = false ) ;

	/* Log an administrative alert to the relay channel & log. */
	bool logAdminMessage(const char*, ... );

	/* Log a priveleged administrative alert to the priv relay channel */
	bool logPrivAdminMessage(const char*, ... );

	/* Log an debug message to the debug channel */
	bool logDebugMessage(const char*, ... );

	/* Write a channel log */
	void writeChannelLog(sqlChannel*, iClient*, unsigned short, const string&);

	/* get last channel event for specific timestamp */
	const string getLastChannelEvent(sqlChannel* theChannel,
		unsigned short eventType, time_t eventTime);

	typedef commandMapType::const_iterator constCommandIterator ;
	constCommandIterator command_begin() const
                { return commandMap.begin() ; }

	constCommandIterator command_end() const
                { return commandMap.end() ; }

	constCommandIterator findCommand( const string& theComm ) const
                { return commandMap.find( theComm ) ; }
	
	typedef map< string, csGline* >        glineListType ;
        glineListType                   glineList ;

        typedef glineListType::iterator  glineIterator;

	glineIterator gline_begin()
		{ return glineList.begin() ; }

	glineIterator gline_end()
	        { return glineList.end() ; }


	/* returns true is IPR checking is required for this user */
	bool needIPRcheck( sqlUser* );

	/* Returns the access sqlUser has in channel sqlChan. */
	short getAccessLevel( sqlUser*, sqlChannel* );

	/* Returns the access sqlUser has in channel sqlChan taking into account
	 * suspensions, etc.
	 * If "bool" is true, then send sqlUser a notice about why they don't
	 * have a particular access. */
	short getEffectiveAccessLevel( sqlUser*, sqlChannel*, bool );

	/* Check if a client has passed IP restriction checks */
	bool passedIPR( iClient* );

	/* Update last_used data on login */
	bool updateIPRlast_used(sqlUser*, const string& );

	/* Set (or clear) a client's IP restriction timestamp */
	void setIPRts( iClient*, unsigned int );

	/* Checks if a user has an existing ipr for it's username
	 * If it's type = 0 disabled also returns false
	 */
	bool hasIPR( sqlUser* );

	/* Checks a user against IP restrictions */
	bool checkIPR(const string&, sqlUser*, unsigned int& );
	bool checkIPR( iClient*, sqlUser* );

	/* Get failed login counter for client */
	unsigned int getFailedLogins( iClient* );

	/* Set failed login counter for client */
	void setFailedLogins( iClient*, unsigned int );

	/* Returns what admin access a user has. */
	short getAdminAccessLevel( sqlUser*, bool = false);

	/* Returns what access a user has in the coder channel */
	short getCoderAccessLevel( sqlUser* );

	/* Fetch a user record for a user. */
	sqlUser* getUserRecord( const string& );

	/* Fetch a user record for a userId. */
	sqlUser* getUserRecord( int );

	/* Checks if this client is logged in, returns a sqlUser if true.
	 * If "bool" is true, send a notice to the client telling them off. */
	sqlUser* isAuthed(iClient*, bool );

	/* Checks to see if this users is forced on this channel */
	unsigned short isForced(sqlChannel*, sqlUser*);

	/* Fetch a channel record for a channel. */
	sqlChannel* getChannelRecord(const string&, bool historysearch = false);
	sqlChannel* getChannelRecord(int, bool historysearch = false);

	/* Fetch a access level record for a user/channel combo. */
	sqlLevel* getLevelRecord(sqlUser*, sqlChannel*);

	/* Fetch the user status flags. L = in cache, P = got password, U = Is authed. */
	string userStatusFlags( const string& );

	/* Returns the current "Flood Points" this iClient has. */
 	unsigned short getFloodPoints(iClient*);

	/* Sets the flood counter for this iClient. */
 	void setFloodPoints(iClient*, unsigned short);

	/* Determins if a client is in "Flooded" state, and if so Notice them. */
	bool hasFlooded(iClient*, const string&);

	/* Sets the timestamp for when we first recieved a msg from this client.
	 * within the flood period. */
	void setLastRecieved(iClient*, time_t);

	/* Find out when we first heard from this chap. */
	time_t getLastRecieved(iClient*);

	void setOutputTotal(const iClient* theClient, unsigned int count);
	unsigned int getOutputTotal(const iClient* theClient);
	bool hasOutputFlooded(iClient*);

	int authenticateUser(const string& username, const string& password, const string& ip, const string& ident,unsigned int&, sqlUser**);
	int authenticateUser(const string& username, const string& password, iClient*, sqlUser**);

	bool doXQLogin(iServer* , const string&, const string&);

	bool doCommonAuth(iClient* , string username = string() );

	bool doXResponse(iServer* , const string&, const string&, bool kill = false);

	/* Sends OPLIST data XQUERY about a channel to mod.openchanfix */
	bool doXQOplist(const string&);

	/* Handles OPLIST data XREPLY from mod.openchanfix */
	bool doXROplist(iServer*, const string&, const string&);

	// Typedef's for user/channel Hashmaps.
	// User hash, Key is Username.
	typedef map< string, sqlUser*, noCaseCompare > sqlUserHashType ;

	// Channel hash, Key is channelname.
	typedef map< string, sqlChannel*, noCaseCompare > sqlChannelHashType ;
	typedef map< int, sqlChannel* > sqlChannelIDHashType ;

	// Accesslevel cache, key is pair(userid, chanid).
	typedef map < std::pair <int, int>, sqlLevel* > sqlLevelHashType ;

 	/* Silence List */
	typedef map < string, std::pair < time_t, string > > silenceListType;
	silenceListType silenceList;

	bool isIgnored(iClient*);
	void setIgnored(iClient*, bool);

	// Cache of user records.
	sqlUserHashType sqlUserCache;

	// Cache of channel records.
	sqlChannelHashType sqlChannelCache;

	// Cache of channel records indexed by channel ID.
	sqlChannelIDHashType sqlChannelIDCache;

	// Cache of Level records.
	sqlLevelHashType sqlLevelCache;

	// Some counters for statistical purposes.
	unsigned int userHits;
	unsigned int userCacheHits;
	unsigned int channelHits;
	unsigned int channelCacheHits;
	unsigned int levelHits;
	unsigned int levelCacheHits;
	unsigned int banHits;
	unsigned int banCacheHits;
	unsigned int dbErrors;
	unsigned int joinCount;

	/* No of seconds offset our local time is from server time. */
	int dbTimeOffset;

	// To keep track of how many custom data chunks
	// we have allocated.
	unsigned int customDataAlloc;

	// Flood/Notice relay channel - Loaded via config.
	// Added coderChan - Loaded via config.
	string relayChan;
	string privrelayChan;
	string debugChan;
	string coderChan;

	// Loaded via config.
	// Interval at which we pick up updates from the Db.
	int updateInterval;

	// Interval at which we check for expired bans/suspends.
	int expireInterval;

	// Interval at which we attempt to purge the cache(s).
	int cacheInterval;

	/* Interval at which we check for new webrelay messages. */
	int webrelayPeriod;

	/* Message for iauthd-c when login-on-connect params are wrong */
	string locMessage;

	/* Duration in seconds at which an idle user/chan/level/ban
	 * record should be purged from the cache. */
	int idleUserPeriod;
	int idleChannelPeriod;
	int idleLevelPeriod;

	/* Duration in seconds at which a 'pending' channel should
	 * be refreshed/updated */
	int pendingChanPeriod;

	/* Duration in seconds at which a 'pending' channel should
	 * be notified that it is so. */
	int pendingNotifPeriod;

	/* After how much seconds should expire any newly created never logged in user */
	int neverLoggedInUsersExpireTime;

	/* Duration in days, after the system purge unlogged users */
	int UsersExpireDBDays;

	/* Length of a day in seconds - refers to user expiration */
	int daySeconds;

	/*  ***   The Judge   ***  */
	/*    related variables    */
	unsigned int RequiredSupporters;
	unsigned int JudgeDaySeconds;
	unsigned int NoRegDaysOnNOSupport;
	unsigned int RejectAppOnUserFraud;
	//int AcceptOnTrafficPass = 1;
	unsigned int RequireReview;
	unsigned int DecideOnObject;
	unsigned int DecideOnCompleted;
	unsigned int ReviewsExpireTime;
	unsigned int PendingsExpireTime;
	unsigned int MaxDays;
	unsigned int Joins;
	unsigned int UniqueJoins;
	unsigned int MinSupporters;
	unsigned int MinSupportersJoin;
	unsigned int NotifyDays;
	unsigned int SupportDays;
	unsigned int ReviewerId;
	unsigned int LogToAdminConsole;
	string ChanfixServerName;

	string validResponseString;
	void AddToValidResponseString(const string&);
	inline const string& getCurrentValidResponse() const
		{ return validResponseString; }
	bool logTheJudgeMessage(const char*, ... );
	bool isValidUser(const string&);
	bool isValidChannel(const string&);
	bool isDBRegisteredChannel(const string&);
	bool RejectChannel(unsigned int, const string&);
	bool ReviewChannel(unsigned int);
	bool AcceptChannel(unsigned int, const string&);
	bool sqlRegisterChannel(iClient*, sqlUser*, const string&);
	bool wipeChannel(unsigned int);
	void checkValidUsersAndChannelsState();
	void checkNewIncomings();
	void checkTrafficPass();
	void checkObjections();
	void checkAccepts();
	void checkReviews();
	void cleanUpReviews();
	void cleanUpPendings();
	void loadIncompleteChanRegs();

	/* End of The Judge */

	// Input flood rate.
	unsigned int input_flood;
	unsigned int output_flood;
	int flood_duration;
	int topic_duration;

	unsigned int channelsFloodPeriod;
	unsigned int floodproRelaxTime;

	// Timestamp's of when we last checked the database for updates.
	time_t lastChannelRefresh;
	time_t lastUserRefresh;
	time_t lastLevelRefresh;
	time_t lastBanRefresh;

	/* TimerID for checking on the database connection. */
	xServer::timerID dBconnection_timerID;

	/* TimerID we recieve for checking for changes in the Db. */
	xServer::timerID update_timerID;

	/* TimerID we recieve every XX seconds for expiration of bans/suspend. */
	xServer::timerID expire_timerID;

	/* TimerID we recieve every XX hours for expiration of cached entries. */
	xServer::timerID cache_timerID;

	/* TimerID we recieve every XX hours for the notification of pending channels. */
	xServer::timerID pending_timerID;

	/* TimerID we recieve every XX hours for the notification of pending channels. */
	xServer::timerID pendingNotif_timerID;

	/* TimerID we recieve every seconds when we should check if a channel limit needs changing */
	xServer::timerID limit_timerID;

	/* TimerID for checking for web-relay messages */
	xServer::timerID webrelay_timerID;

	/* TimerID for checking channel flood cleanups */
	xServer::timerID channels_flood_timerID;

	// Language definitions table (Loaded from Db).
	typedef map < string, std::pair <int, string> > languageTableType;
	languageTableType languageTable;

	// Language translations table (Loaded from Db).
	typedef map < std::pair <int, int>, string > translationTableType ;
	translationTableType translationTable;

	void loadTranslationTable();

	int rehashMOTD();

	// Method to retrieve a translation string.
	const string getResponse( sqlUser*, int , string = string() );

	/* Help topics (Loaded from Db) */
	typedef map < std::pair <int, string>, string > helpTableType;
	helpTableType helpTable;

	void loadHelpTable();
	const string getHelpMessage(sqlUser*, string);

	/**
	 * Count channel ops.
	 */
	static size_t countChanOps(const Channel*);

	void deVoiceAllOnChan(Channel*);

	// Deop everyone on this channel.
	void deopAllOnChan(Channel*);
	void deopAllUnAuthedOnChan(Channel*);
	void deopSuspendedOnChan(Channel*, sqlUser*);
	void doAllBansOnChan(Channel*);
	void doTheRightThing(Channel*);

	/* Sets a description (url) topic combo. */
	void doAutoTopic(sqlChannel*);

	/* Automatically updates the floating limit for this channel */
	void doFloatingLimit(sqlChannel*, Channel*);

	/* This function is used to ban a mask, to who no matching client is currently existing on the channel */
	bool doSingleBan(sqlChannel* theChan, const string& banMask, unsigned short banLevel, unsigned int banExpire, const string& theReason);

	/* Bans and kick just one client, with level, period, reason */
	bool doSingleBanAndKick(sqlChannel*, iClient*, unsigned short, unsigned int, const string& theReason);

	/* Bans & kicks & *Suspends* a specified user with a specific reason */
	bool doInternalBanAndKick(sqlChannel*, iClient*, const string&);

    /* Bans & kicks all client belonging to the IP addrees, with a specific level, period, reason */
    bool doInternalBanAndKick(sqlChannel*, iClient*, unsigned short, unsigned int, const string& theReason);

    /* Bans & kicks all client belonging to a specified banMask, with a specific level, period, reason */
    bool doInternalBanAndKick(sqlChannel*, const string&, unsigned short, unsigned int, const string&);

    /* Suspends a specified user with a specific level, period, reason */
    bool doInternalSuspend(sqlChannel*, iClient*, unsigned short, unsigned int, const string& theReason);

    /* G-lines a target client to a time-period, with the reason */
    bool doInternalGline(iClient*, const time_t&, const string&);

    /* G-lines a numricIP to a time-period, with the reason */
    bool doInternalGline(const string&, const time_t&, const string&);

    /* Channel, FloodMessage, kickReason */
    bool KickAllWithFloodMessage(Channel*, const string&, const string&, bool clearcount = true);

    /* Channel, FloodMessage, numericIP, banLevel, banDuration, banReason */
    bool KickBanAllWithFloodMessage(Channel*, const string&, unsigned short, unsigned int, const string&);

    /* Channel, FloodMessage, Duration, Reason */
    bool GlineAllWithFloodMessage(sqlChannel*, const string&, const time_t&, const string&);

    /* Support function to check if a user is banned on a channel. */
	sqlBan* isBannedOnChan(sqlChannel*, iClient*);

	/* Matches DB bans, and kicks supplied user if neccessary */
	bool checkBansOnJoin( Channel*, sqlChannel* , iClient* );

	/* Checking for need to raise the floodpro level. Returns the repetition number. */
	unsigned int checkFloodproLevel(sqlChannel*, const string&);

	/* Cleanup the channel flood masks/clients */
	void checkChannelsFlood();

	time_t currentTime() const ;

	struct ValidUserData
	{
		string UserName;
		string Email;
		int type;
	};

	typedef vector<ValidUserData> ValidUserDataListType;

	/* List of channels in 'pending' registration state. */
	typedef map < string, sqlPendingChannel*, noCaseCompare >
		pendingChannelListType;
	pendingChannelListType pendingChannelList;

	struct AppData
	{
		int chanId;
		string chanName;
		int mngrUserId;
	};

	typedef vector<AppData> AppDataListType;

	struct IncomingData
	{
		int chanId;
		string chanName;
		string mngrUserName;
		bool noticed;
	};

	typedef vector<IncomingData> IncomingDataListType;
	typedef map <int, IncomingDataListType> suppIdListType;

	void setSupporterNoticedStatus(int suppId, int chanId, bool noticed);

	/* This is a bug-workaround function, for some reason on cservice::doCommonAuth
	 * doesn't work with the int chanId	*/
	void setSupporterNoticedStatus(int suppId, const string& chanName, bool noticed);

	/* Update the OpList score data for all the pending channels in status 0,1,2, and 8 */
	void UpdatePendingOpLists();

	/* checkIncomings is used to select the supporters of a new pending (status=0) channels
	 * where the supporters had not lodged their support yet */
	void checkIncomings(bool IsFirstNoticing = false);

	void initialiseSupport(const string&, sqlPendingChannel::supporterListType );

	/*
	 *  Load the pendingChannelList from the database.
	 *  This list contains details about channels currently
	 *  'Pending' successful registration.
	 */

	void loadPendingChannelList();

	/*
	 *  Timer Functions.
	 *  These support functions are called at periodic
	 *  intervals to perform maintainence, etc.
	 */

	/*
	 * Expire suspends, ignores and bans respectively.
	 */
	void expireSuspends();
	void expireSilence();
	void expireBans();
	bool expireGlines();
	bool expireWhitelist();

	/*
	 *  Cache expiration functions.
	 *  To expire idle user/level/ban records from the
	 *  cache.
	 *  N.B: We'll never expire out channel records because
	 *  this information may be used to cancel 'unused'
	 *  channels.
	 */

	void cacheExpireUsers();

	/*
	 *  Expire Ban records, only if the channel
	 *  record is 'idle'.
	 */

	void cacheExpireBans();

	/*
	 *  Expire idle Level records.
	 */

	void cacheExpireLevels();

	/*
	 * Process any pending reop requests by the bot.
	 */
	void performReops();


	bool deleteUserFromTable(unsigned int, const string&);
	bool wipeUser(unsigned int, bool);

	void ExpireUsers();

	/*
	 * Process any Postgres notification requests,
	 * reloading cached records if neccessary.
	 */
	void processDBUpdates();

	/*
	 *  Send a generic Error Message, may log/etc at a later date.
	 */
	void dbErrorMessage(iClient*);

	/*
	 *  Misc uncategorisable functions.
	 */

	unsigned int preloadUserDays;

	void preloadBanCache();
	void preloadChannelCache();
	void preloadLevelsCache();
	void preloadUserCache();
	
	bool loadGlines();

	void updateChannels();
	void updateUsers();
	void updateUserLevels(sqlUser* );
	void updateLevels(int channelId = 0);
	vector<sqlUser*> getChannelManager(int channelId);
	void updateBans();

	void updateLimits();

	bool addGline( csGline* );
	bool remGline( csGline* );
	csGline* findGline( const string& );

	void addGlineToUplink(csGline* );

	typedef map < string, int > statsMapType;
	statsMapType statsMap;

	void incStat(const string& name);
	void incStat(const string& name, unsigned int amount);

	void noticeAllAuthedClients(sqlUser* theUser, const char* Message, ... );
	void NoteAllAuthedClients(sqlUser* theUser, const char* Message, ... );
	void NoteChannelManager(const string& theChan, const char* Message, ...);
	void NoteChannelManager(sqlChannel* theChan, const char* Message, ...);

	bool isPasswordRight(sqlUser*, const string&);

	unsigned int totalCommands;

	unsigned int loginDelay;

	unsigned int noteDuration;
	unsigned int noteLimit;

	unsigned int partIdleChan;

	unsigned int MAXnotes;

	void doCoderStats(iClient* theClient);

	typedef map< string, ConfigData*, noCaseCompare > configHashType;
	configHashType configTable;

	/* Method to retrieve a configuration variable */
	ConfigData* getConfigVar(const string&);

	/* A dummy config entry with nothing in it. */
	ConfigData empty_config;

	/*
	 * Method to pre-load the configTable above with everything thats
	 * currently in the database.
	 */
	void loadConfigData();

	inline void clearConfigVariables()
	{
		return cserviceConfig->Clear();
	}

	inline int getConfigVariableSize()
	{
		return (int)cserviceConfig->size();
	}

	inline bool parseConfigFile()
	{
		return cserviceConfig->openConfigFile();
	}
	/*
	 * Load variable values from cservice.conf
	 */
	void loadConfigVariables();

	void outputChannelAccesses(iClient*, sqlUser*, sqlUser*, unsigned int);

	/*
 	 * Global filesystem level admin logging stream.
	 */

	std::ofstream	adminLog ;

	string adminlogPath;

	static string CryptPass( const string& pass ) ;

#ifdef ALLOW_HELLO
	typedef std::map< string, time_t > helloIPListType ;
	helloIPListType	helloIPList ;
	unsigned int helloBlockPeriod ;
#endif // ALLOW_HELLO

#ifdef TOTP_AUTH_ENABLED
	bool totpAuthEnabled;
#endif

/* Welcome messages for newly registered channels */
string welcomeNewChanMessage;
string welcomeNewChanTopic;

bool InsertUserHistory(iClient*, const string&);

} ;


const string escapeSQLChars(const string& theString);
const string searchSQL(const string& theString);

} // namespace gnuworld

#endif // __CSERVICE_H
