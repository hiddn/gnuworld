/* config.h
 * $Id: config.h,v 1.6 2001/01/12 23:42:05 dan_karrels Exp $
 */

#ifndef __CONFIG_H
#define __CONFIG_H "$Id: config.h,v 1.6 2001/01/12 23:42:05 dan_karrels Exp $"

/* config.h */
/* GNUWorld main configuration file */
/* blah blah blah goes here, but who cares,
 * Damnit Jim! Im a programmer not an author!
 */

/**
 * EDEBUG
 * This define is pretty obvious...Will probably consume
 * massive quantities of CPU when debugging, recommended
 * only for debugging/testing/optimization purposes
 */
#define EDEBUG

#ifdef EDEBUG

/**
 * DEBUGFILE
 * The file that the debugger will write to
 */
#define DEBUGFILE "debug.log"

#endif // EDEBUG

/**
 * CONFFILE
 * This is the name of the default file from which GNUWorld
 * will read its server characteristics.
 */
#define CONFFILE "gnuworld.conf"

/**
 * LOGFILE
 * Name of the file that gnuworld logs to =P
 */
#define LOGFILE "gnuworld.log"

/**
 * LOG_SOCKET
 * Define this variable to enable logging of incoming raw data.
 * This is very useful for offline debugging.
 */
#define LOG_SOCKET

#ifdef LOG_SOCKET

/**
 * LOG_SOCKET_NAME
 * The name of the default file to which to write raw data
 * from the network.
 */
#define LOG_SOCKET_NAME "socket.log"

#endif /* LOG_SOCKET */

/**
 * CLIENT_DESC
 * Define this variable if you want gnuworld to track the
 * real name (description) of each network client.  This
 * can use quite a bit more memory on large networks.
 */
#undef CLIENT_DESC

/**
 * MAXLOOPCOUNT_DEFAULT
 * This variable represents how many times to attempt a system
 * call before giving up.
 */
#define MAXLOOPCOUNT_DEFAULT 10

#endif /* __CONFIG_H */
