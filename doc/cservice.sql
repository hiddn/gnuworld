------------------------------------------------------------------------------------
-- "$Id: cservice.sql,v 1.12 2001/01/02 02:32:45 gte Exp $"
-- Channel service DB SQL file for PostgreSQL.

-- ChangeLog:
-- 2000-12-30: Gte
--             Added some update notification events for CMaster to listen
--             on and refresh its internal cache.
--
-- 2000-12-22: Gte
--             Fixed invalid UserID reference in userlog table.
--
-- 2000-12-10: Gte
--             Fixed a few typo's, changed TIMESTAMP's to INT4's.
--	       
-- 2000-10-22: Isomer
--             Removed 'ChannelManager' information from channels table
--             Added email/url/public_key to users table
--             Defined the flags
--             added channellog/userlog for combined ilc databases etc.
--             Changed many strings to 'TEXT'.
--	       removed nick_flood_pro.
--	       added defaults for flood_pro's.
--             changed the key type on the bans table.
--	       added supporters table.
--             lotsa misc things
--	       added 'added_by' and 'added_by_ts' to access table
--             users now have language, not channels
--             checked current CS sources in case we missed something.
--
-- 2000-09-21: Bleep
--             Added last_updated timestamps
--
-- Prior: Maintained by moof.           


-- The service supports multiple languages, defined in language
-- files.

CREATE TABLE languages (
	id SERIAL,
	name VARCHAR( 16 ),
	last_update INT4 NOT NULL
--	PRIMARY KEY(id)
);

-- Translations for multi-lingual support.

CREATE TABLE translations (

	language_id INT4 CONSTRAINT translations_language_id_ref REFERENCES languages ( id ),
	response_id INT4 NOT NULL DEFAULT '0',
	text TEXT,
	last_update INT4 NOT NULL,

	PRIMARY KEY (language_id, response_id)
);


-- Create the channel table first since we'll be referring back to it
-- frequently.

CREATE TABLE channels (
	id SERIAL,
	name VARCHAR (200) NOT NULL UNIQUE,
	flags INT4 NOT NULL DEFAULT '0',
-- 0x0000 0001 - No Purge
-- 0x0000 0002 - Special Channel
-- 0x0000 0004 - No Reg -- Don't register for a time period after which is
			-- reviewed by cservice admin.
-- 0x0000 0008 - Never Reg -- Never register
-- 0x0000 0010 - Channel Suspended
			-- everyone (including the 500) is suspended.
-- 0x0000 0020 - Temp Manager
-- 0x0000 0040 - Cautioned
-- 0x0000 0080 - Manager on Vacation

-- 0x0001 0000 - AlwaysOp
-- 0x0002 0000 - StrictOp
-- 0x0004 0000 - NoOp
-- 0x0008 0000 - AutoTopic
-- 0x0010 0000 - OpOnly
-- 0x0020 0000 - AutoJoin
		
	-- Do we want to keep either or both of these?
	-- nb: removed nickflood pro.  not useful.
	mass_deop_pro INT2 NOT NULL DEFAULT 3,
	flood_pro INT2 NOT NULL DEFAULT 7,
	url VARCHAR (128),
	description VARCHAR (128),
	keywords VARCHAR(128),
	registered_ts INT4,
	channel_ts INT4 NOT NULL,
	channel_mode VARCHAR(26),
	-- does anyone actually read these schemas?
	-- Yes :)
	channel_key VARCHAR(24),
	channel_limit INT4,
	last_update INT4 NOT NULL,

	PRIMARY KEY (id)
);

-- A channel is inactive if the manager hasn't logged in for 21 days

CREATE INDEX channel_name_idx ON channels (name);

-- Table for bans; channel_id references the channel entry this ban belongs to.

CREATE TABLE bans (

	id SERIAL,
	banmask VARCHAR (128) NOT NULL,
	set_by VARCHAR (128),			-- nick!user@host
	set_ts INT4,
	level INT2,
	duration INT4,				-- In seconds
	reason VARCHAR (128),
	channel_id INT4 CONSTRAINT bans_channel_id_ref REFERENCES channels (id),
	last_update INT4 NOT NULL,

	PRIMARY KEY (banmask,channel_id)
);

-- Access entries; admin access kept on channel '*'.

CREATE TABLE users (

	id SERIAL,
	user_name VARCHAR (32) NOT NULL,
	password VARCHAR (40) NOT NULL,
	last_seen INT4,
	email VARCHAR (128),
        url  VARCHAR(128),
	language_id INT4 CONSTRAINT language_channel_id_ref REFERENCES languages (id),
	public_key TEXT,
	flags INT2 NOT NULL DEFAULT '0',
-- 0x00 01 -- Suspended globally
	last_update_by VARCHAR (128),		-- nick!user@host
	last_update INT4 NOT NULL,
	PRIMARY KEY ( id )
) ;

CREATE INDEX users_username_idx ON users( user_name );


CREATE TABLE levels (

	channel_id INT4 CONSTRAINT levels_channel_id_ref REFERENCES channels ( id ),
	user_id INT4 CONSTRAINT levels_users_id_ref REFERENCES users ( id ),
	access INT4 NOT NULL DEFAULT '0',
	flags INT2 NOT NULL DEFAULT '0',
-- 0x00 01 -- Logged in
-- 0x00 02 -- AutoOp
-- 0x00 04 -- Protect  (From CS source, unused)
-- 0x00 08 -- Temp forced access. (removed on logout).
	suspend_expires INT4,
	suspend_by VARCHAR( 128 ),
	added INT4,
	added_By VARCHAR( 128 ),
	last_Modif INT4,
	last_Modif_By VARCHAR( 128 ),
	last_Updated INT4 NOT NULL,
	PRIMARY KEY( channel_id, user_id )
);


CREATE INDEX levels_access_idx ON levels( access ) ;

CREATE TABLE channellog (
	ts INT4,
	channelID INT4 CONSTRAINT channel_log_ref REFERENCES channels ( id ),
	message TEXT,
	last_updated INT4 NOT NULL
);

CREATE TABLE userlog (
	ts INT4,
	user_id INT4 CONSTRAINT user_log_ref REFERENCES users ( id ),
	message TEXT,
	last_updated INT4 NOT NULL
);

CREATE TABLE supporters (
	channel_id INT4 CONSTRAINT channel_supporters_ref REFERENCES channels ( id ),
	user_id INT4 CONSTRAINT users_supporters_ref REFERENCES users( id ),
	support CHAR,
-- NULL - not answered yet
-- Y - Supports this channel
-- N - Doesn't support this channel
	reason TEXT,
-- Reason for not supporting it if required.
	last_updated INT4 NOT NULL,
	PRIMARY KEY(channel_id,user_id)
);

CREATE TABLE pending (
	channel_id INT4 CONSTRAINT pending_channel_ref REFERENCES channels (id),
	manager_id INT4 CONSTRAINT pending_manager_ref REFERENCES users (id),
	created_ts INT4 NOT NULL,
	decision_ts INT4,
	decision VARCHAR (80),
	comments TEXT,
	last_updated INT4 NOT NULL,
	PRIMARY KEY(channel_id)
);

CREATE TABLE baddomain (
	id SERIAL,
	domain varchar(1024) NOT NULL UNIQUE,
	PRIMARY KEY(id)
);

-- Update notification rules.
--

CREATE RULE cm1 AS ON UPDATE TO channels DO NOTIFY channels_u;
CREATE RULE cm2 AS ON DELETE TO channels DO NOTIFY channels_d;
CREATE RULE cm3 AS ON UPDATE TO bans DO NOTIFY bans_u;
CREATE RULE cm4 AS ON DELETE TO bans DO NOTIFY bans_d;
CREATE RULE cm5 AS ON UPDATE TO users DO NOTIFY users_u;
CREATE RULE cm6 AS ON DELETE TO users DO NOTIFY users_d;
CREATE RULE cm7 AS ON UPDATE TO levels DO NOTIFY levels_u;
CREATE RULE cm8 AS ON DELETE TO levels DO NOTIFY levels_d;

-----------------------------------------------------------------------------------------

--Notes:
-- * During registration:
--   - Check to see if at least 10 different hostmasks join the channel.
--   - Check to see if the actual listed supporters join the channel
--    (Add a field to supporters that flags if they've been spotted on the channel?)

--  <DrCkTaiL> like - 11 total different clients, 6 of which should be on the
--  supporter list, in 3 days
  
--  <DrCkTaiL> but all three values should be soft so we could change them

-- * Registered channel.
--   If The owner hasn't logged in for 21 days.
--     If noone else has logged in for 21 days:
--	Possible Purge Candidate.
--     Else 
--      Time to organise a new maintainer
