/* ccGline.h */

#ifndef __CCGLINE_H
#define __CCGLINE_H "$Id: ccGline.h,v 1.3 2001/03/29 21:54:32 dan_karrels Exp $"

#include	<string>

#include	<ctime>

#include	"libpq++.h"

namespace gnuworld
{ 

using std::string ;

class ccGline
{
public:
	ccGline(PgDatabase*);
	virtual ~ccGline();
	//Mehods for getting info
	
	inline const string& 	get_Id() const
		{ return Id; }

	inline const string& 	get_Host() const
		{ return Host; }
	
	inline const string& 	get_AddedBy() const
		{ return AddedBy; }
    
	inline const time_t&	get_AddedOn() const
		{ return AddedOn; }
	    
	inline const time_t&	get_Expires() const
		{ return Expires; }

	inline const string&	get_Reason() const
		{ return Reason; }

	
	//Methods for setting info
	
	inline void 		set_Id( const string& _Id ) 
		{ Id = _Id; }

	inline void 		set_Host( const string& _Host ) 
		{ Host = _Host; }
	
	inline void 		set_AddedBy( const string& _AddedBy ) 
		{ AddedBy = _AddedBy; }
    
	inline void		set_AddedOn( const time_t& _AddedOn ) 
		{ AddedOn = _AddedOn; }
	    
	inline void		set_Expires( const time_t& _Expires ) 
		{ Expires = _Expires; }

	inline void 		set_Reason( const string& _Reason ) 
		{ Reason = _Reason; }
	    
	//Methods for updating 

	bool Insert();
	
	bool Update();
	
	bool loadData( int );

protected:
	string Id;
	string Host;
	string AddedBy;
	time_t AddedOn;
	time_t Expires;
	string Reason;
	PgDatabase* SQLDb;

}; // class ccGline

} // namespace gnuworld

#endif // __CCGLINE_H