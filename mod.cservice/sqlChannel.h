/* sqlChannel.h */

#ifndef __SQLCHANNEL_H
#define __SQLCHANNEL_H "$Id: sqlChannel.h,v 1.13 2001/01/21 04:17:41 gte Exp $"

#include	<string>
#include	<ctime>
#include	"libpq++.h"

using std::string ;

namespace gnuworld
{ 
 
class sqlChannel
{

public:
	sqlChannel(PgDatabase*) ;
	virtual ~sqlChannel() ;

	typedef unsigned int	flagType ;

	static const flagType	F_NOPURGE  = 0x00000001 ;
	static const flagType	F_SPECIAL  = 0x00000002 ;
	static const flagType	F_NOREG    = 0x00000004 ;
	static const flagType	F_NEVREG   = 0x00000008 ;
	static const flagType	F_SUSPEND  = 0x00000010 ;
	static const flagType	F_TEMP     = 0x00000020 ;
	static const flagType	F_CAUTION  = 0x00000040 ;
	static const flagType	F_VACATION = 0x00000080 ;

	static const flagType	F_ALWAYSOP  = 0x00010000 ;
	static const flagType	F_STRICTOP  = 0x00020000 ;
	static const flagType	F_NOOP      = 0x00080000 ;
	static const flagType	F_AUTOTOPIC = 0x00100000 ;
	static const flagType	F_OPONLY    = 0x00100000 ;
	static const flagType	F_AUTOJOIN  = 0x00200000 ;

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
	
	inline const string&		getKeywords() const
		{ return keywords ; }
	
	inline const time_t&		getRegisteredTS() const
		{ return registered_ts ; }

	inline const time_t&		getChannelTS() const
		{ return channel_ts ; } 

	inline const string&		getChannelMode() const
		{ return channel_mode ; }
	
	inline const string&		getChannelKey() const
		{ return channel_key ; }
	
	inline const unsigned int&	getChannelLimit() const
		{ return channel_limit ; }
	
	inline const time_t&		getLastUpdated() const
		{ return last_updated ; }

	inline const bool& 			getInChan() const
		{ return inChan; }

	inline const time_t&		getLastTopic() const
		{ return last_topic ; }
 
	// Load channel data from the backend using the channel name as a key.
	bool loadData( const string& );

	// Load channel data from the backend using the channel_id as a key.
	bool loadData( int ); 


	/*
	 *  Methods to set data atrributes.
	 */

	// 'ID' is a primary key and cannot be altered.
	// We aren't going to let people change channel names here either.
 
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

	inline void setKeywords( const string& _keywords )
		{ keywords = _keywords; }

	inline void setRegisteredTS( const time_t& _registered_ts )
		{ registered_ts = _registered_ts; }

	inline void setChannelTS( const time_t& _channel_ts )
		{ channel_ts = _channel_ts; } 

	inline void setChannelMode( const string& _channel_mode )
		{ channel_mode = _channel_mode; }

	inline void setChannelKey ( const string& _channel_key )
		{ channel_key = _channel_key; }

	inline void setChannelLimit( const unsigned int& _channel_limit )
		{ channel_limit = _channel_limit; }

	inline void setInChan( const bool& _inChan )
		{ inChan = _inChan; }

	inline void setLastTopic( const time_t& _last_topic )
		{ last_topic = _last_topic; }

	// 'last_updated' is only altered during commit().
 
	/*
	 * Method to perform a SQL 'UPDATE' and commit changes to this
	 * object back to the database.
	 */

	bool commit();
    void setAllMembers(int);

protected: 

	unsigned int	id ;
	string		name ;
	flagType	flags ;
	unsigned short	mass_deop_pro ;
	unsigned short	flood_pro ;
	string		url ;
	string		description ;
	string		keywords  ;
	time_t		registered_ts ;
	time_t		channel_ts ;
	string		channel_mode ;
	string		channel_key ;
	unsigned int	channel_limit ;
	time_t		last_updated ; 
	time_t		last_topic ;
	bool		inChan;
	PgDatabase*	SQLDb;
} ;

} 
#endif // __SQLCHANNEL_H
