/* sqlChannel.h */

#ifndef __SQLCHANNEL_H
#define __SQLCHANNEL_H "$Id: sqlChannel.h,v 1.35 2001/09/12 21:02:47 gte Exp $"

#include	<string>
#include	<map>
#include	<ctime>
#include	"libpq++.h"
#include	"sqlBan.h"

using std::string ;

namespace gnuworld
{

class sqlChannel
{

public:
	sqlChannel(PgDatabase*) ;
	virtual ~sqlChannel() ;

	typedef unsigned int	flagType ;

	static const flagType	F_NOPURGE;
	static const flagType	F_SPECIAL;
	static const flagType	F_NOREG;
	static const flagType	F_NEVREG;
	static const flagType	F_SUSPEND;
	static const flagType	F_TEMP;
	static const flagType	F_CAUTION;
	static const flagType	F_VACATION;
	static const flagType   F_LOCKED;

	static const flagType	F_ALWAYSOP;
	static const flagType	F_STRICTOP;
	static const flagType	F_NOOP;
	static const flagType	F_AUTOTOPIC;
	static const flagType	F_OPONLY; 	// Deprecated.
	static const flagType	F_AUTOJOIN;

	/*
	 *   Channel 'Event' Flags, used in the channelog table.
	 *   These flags are used to filter channel log records.
	 *   in reports.
	 */

	static const int	EV_MISC;
	static const int	EV_JOIN;
	static const int	EV_PART;
	static const int	EV_OPERJOIN;
	static const int	EV_OPERPART;
	static const int	EV_FORCE;
	static const int	EV_REGISTER;
	static const int	EV_PURGE;

	/* Manually added Comment */
	static const int	EV_COMMENT;
	static const int	EV_REMOVEALL;
	static const int	EV_IDLE;

	/*
	 *  Methods to get data atrributes.
	 */

	inline const unsigned int&	getID() const
		{ return id ; }

	inline const string&		getName() const
		{ return name ; }

	inline const flagType&		getFlags() const
		{ return flags ; }

	inline bool  getFlag( const flagType& whichFlag ) const
		{ return (flags & whichFlag) ; }

	inline const unsigned short int& getMassDeopPro() const
		{ return mass_deop_pro ; }

	inline const unsigned short int& getFloodPro() const
		{ return flood_pro ; }

	inline const string&		getURL() const
		{ return url ; }

	inline const string&		getDescription() const
		{ return description ; }

	inline const string&		getComment() const
		{ return comment ; }

	inline const string&		getKeywords() const
		{ return keywords ; }

	inline const time_t&		getRegisteredTS() const
		{ return registered_ts ; }

	inline const time_t&		getChannelTS() const
		{ return channel_ts ; }

	inline const string&		getChannelMode() const
		{ return channel_mode ; }

	inline const unsigned short int& getUserFlags() const
		{ return userflags ; }

	inline const time_t&		getLastUpdated() const
		{ return last_updated ; }

	inline const bool& 			getInChan() const
		{ return inChan; }

	inline const time_t&		getLastTopic() const
		{ return last_topic ; }

	inline const time_t&		getLastUsed() const
		{ return last_used ; }

	/**
	 * Load channel data from the backend using the channel name as
	 * a key.
	 */
	bool loadData( const string& );

	/**
	 * Load channel data from the backend using the channel_id as a
	 * key.
	 */
	bool loadData( int );

	/*
	 *  Methods to set data atrributes.
	 */

	// 'ID' is a primary key and cannot be altered.

	inline void setID( const unsigned int& _id )
		{ id = _id; }

	inline void setName( const string& _name )
		{ name = _name; }

	inline void setFlag( const flagType& whichFlag )
		{ flags |= whichFlag; }

	inline void removeFlag( const flagType& whichFlag )
		{ flags &= ~whichFlag; }

	inline void setMassDeopPro( const unsigned short int& _mass_deop_pro )
		{ mass_deop_pro = _mass_deop_pro; }

	inline void setFloodPro( const unsigned short int& _flood_pro )
		{ flood_pro = _flood_pro; }

	inline void setURL( const string& _url )
		{ url = _url; }

	inline void setDescription( const string& _description )
		{ description = _description; }

	inline void setComment( const string& _comment )
		{ comment = _comment; }

	inline void setKeywords( const string& _keywords )
		{ keywords = _keywords; }

	inline void setRegisteredTS( const time_t& _registered_ts )
		{ registered_ts = _registered_ts; }

	inline void setChannelTS( const time_t& _channel_ts )
		{ channel_ts = _channel_ts; }

	inline void setChannelMode( const string& _channel_mode )
		{ channel_mode = _channel_mode; }

	inline void setUserFlags( const unsigned short int& _userflags )
		{ userflags = _userflags; }

	inline void setInChan( const bool& _inChan )
		{ inChan = _inChan; }

	inline void setLastTopic( const time_t& _last_topic )
		{ last_topic = _last_topic; }

	inline void setLastUsed( const time_t& _last_used )
		{ last_used = _last_used; }

	/**
	 * Method to perform a SQL 'UPDATE' and commit changes to this
	 * object back to the database.
	 */
	bool commit();
	bool insertRecord();
	void setAllMembers(int);

public:
	/*
	 * Map with key user-id, contents level and username for
	 * easy access
	 * Stores UID's of admin users with forced accessse
	 * on this channel
	 */

	typedef map< unsigned int, pair < unsigned short, string > > forceMapType ;
	forceMapType forceMap;

	typedef vector < sqlBan* > sqlBanVectorType;
	sqlBanVectorType banList;

protected:

	unsigned int	id ;
	string		name ;
	flagType	flags ;
	unsigned short	mass_deop_pro ;
	unsigned short	flood_pro ;
	string		url ;
	string		description ;
	string		comment ;
	string		keywords  ;
	time_t		registered_ts ;
	time_t		channel_ts ;
	string		channel_mode ;
	unsigned short	userflags ;
	time_t		last_updated ;
	time_t		last_topic ;
	bool		inChan;
	time_t		last_used;

	PgDatabase*	SQLDb;

 } ;

}

#endif // __SQLCHANNEL_H
