------------------------------------------------------------------------------------
-- "$Id: webcookies.sql,v 1.1 2001/10/14 05:04:46 nighty Exp $"
-- Channel service DB SQL file for PostgreSQL.
--
-- Tables specific to webcookies (needs to be LOCAL).
-- If your whole DB is local then this table NEEDS to be in a db named "cmaster"
-- If your local DB is named "cmaster" .. then just 'psql cmaster < webcookies.sql'
-- otherwise,
--                createdb cmaster
--                psql cmaster webcookies.sql
--
-- nighty <nighty@undernet.org>

CREATE TABLE webcookies (
        user_id INT4 CONSTRAINT users_supporters_ref REFERENCES users( id ),
        cookie VARCHAR(32) UNIQUE,
        expire INT4
);

CREATE INDEX webcookies_user_id_idx ON webcookies(user_id);
