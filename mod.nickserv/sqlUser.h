#ifndef _SQLUSER_H
#define _SQLUSER_H "$Id: sqlUser.h,v 1.4 2002/08/16 21:37:32 jeekay Exp $"

#include <string>

#include "libpq++.h"

#include "sqlManager.h"

namespace gnuworld
{

namespace ns
{

class sqlManager;

class sqlUser
{
  public:

    typedef unsigned short int flagType;
    static const flagType F_SUSPEND;
    static const flagType F_AUTOKILL;

    /** Default constructor makes an empty user */
    sqlUser(sqlManager*);
    
    /** Default destructor deletes user */
    ~sqlUser();
    
    
    /* ACCESSOR METHODS */
    
    /** Accessor for id */
    inline const unsigned int& getID() const
      { return id; }
    
    /** Accessor for name */
    inline const string& getName() const
      { return name; }
      
    /** Check whether a user has a given flag set */
    inline bool getFlag(const flagType& whichFlag) const
      { return (whichFlag == (flags & whichFlag)); }
    
    /** Get the user level */
    inline unsigned int getLevel() const
       { return level; }
    
    /** Get the user lastseen */
    inline unsigned int getLastSeen() const
      { return lastseen; }
    
    
    /* MUTATOR METHODS */
    
    /** Mutator for name */
    inline void setName(const string& _name)
      { name = _name; }
    
    /** Remove a flag from the user record */
    inline void removeFlag(const flagType& whichFlag)
      { flags &= ~whichFlag; }

    /** Set a flag on the user record */
    inline void setFlag(const flagType& whichFlag)
      { flags |= whichFlag; }
    
    /** Set the user level */
    inline void setLevel(const unsigned int _level)
      { level = _level; }
    
    /** Set the lastseen */
    inline void setLastSeen(const unsigned int _lastseen)
      { lastseen = _lastseen; }
    
    /* MISCELLANEOUS METHODS */
    
    /** Commit this user back to the database */
    void commit();
    
    /** Load data for this user from a given DB handle */
    void setAllMembers(PgDatabase*, int);

  protected:
    unsigned int id;
    string name;
    flagType flags;
    unsigned int level;
    unsigned int lastseen;
    
    sqlManager* myManager;
}; // class sqlUser

} // namespace ns

} // namespace gnuworld

#endif
