/*
 * sqlPendingChannel Class Header.
 * Stores information about channels currently being registered.
 * We do a number of things to channels in this state ;)
 */

#ifndef __SQLPENDINGCHANNEL_H
#define __SQLPENDINGCHANNEL_H "$Id: sqlPendingChannel.h,v 1.1 2001/04/15 22:12:36 gte Exp $"

#include	<string> 
 
using std::string ;

namespace gnuworld
{ 
 
class sqlPendingChannel
{
public:
	unsigned int channel_id;
	unsigned int join_count;

};

}
#endif // __SQLPENDINGCHANNEL_H
