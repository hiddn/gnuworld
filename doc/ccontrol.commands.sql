-- "$Id: ccontrol.commands.sql,v 1.2 2001/07/29 13:33:19 mrbean_ Exp $"

-- Fills the commands table with the default values 
-- Note: DO NOT EDIT THIS FILE BY HAND!
-- 	 THESE ARE JUST DEFAULT VALUES 
--	 USE THE COMMANDS COMMAND TO CHANGE THEM

-- Last Updated : 26/07/01 |MrBean|

Delete from commands;
COPY "commands" FROM stdin;
ACCESS	ACCESS	1	n	n	t	1	n
HELP	HELP	2	n	n	t	1	n
LOGIN	LOGIN	0	n	t	t	1	n
DEAUTH	DEAUTH	4	n	n	t	1	n
NEWPASS	NEWPASS	8	n	n	t	1	n
MODE	MODE	16	n	n	n	1	n
OP	OP	32	n	n	n	1	n
DEOP	DEOP	64	n	n	n	1	n
MODERATE	MODERATE	128	n	n	n	1	n
UNMODERATE	UNMODERATE	256	n	n	n	1	n
INVITE	INVITE	512	n	n	n	1	n
JUPE	JUPE	1024	n	n	n	1	n
GLINE	GLINE	2048	n	n	n	1	n
REMGLINE	REMGLINE	4096	n	n	n	1	n
SCANGLINE	SCANGLINE	8192	n	n	n	1	n
REMOPERCHAN	REMOPERCHAN	16384	n	n	n	1	n
ADDOPERCHAN	ADDOPERCHAN	32768	n	n	n	1	n
LISTOPERCHANS	LISTOPERCHANS	65536	n	n	n	1	n
CHANINFO	CHANINFO	131072	n	n	n	1	n
WHOIS	WHOIS	262144	n	n	n	1	n
ADDUSER	ADDUSER	524288	n	n	n	1	n
REMUSER	REMUSER	1048576	n	n	n	1	n
MODUSER	MODUSER	2097152	n	n	n	1	n
TRANSLATE	TRANSLATE	4194304	n	n	n	1	n
KICK	KICK	8388608	n	n	n	1	n
ADDCOMMAND	ADDCOMMAND	16777216	n	n	n	1	n
REMCOMMAND	REMCOMMAND	33554432	n	n	n	1	n
SUSPEND	SUSPEND	67108864	n	n	n	1	n
UNSUSPEND	UNSUSPEND	134217728	n	n	n	1	n
CLEARCHAN	CLEARCHAN	268435456	n	n	n	1	n
LISTHOSTS	LISTHOSTS	536870912	n	n	n	1	n
ADDSERVER	ADDSERVER	1073741824	n	n	n	1	n
LEARNNET	LEARNNET	1	n	n	n	1	t
REMSERVER	REMSERVER	2	n	n	n	1	t
CHECKNET	CHECKNET	4	n	n	n	1	t
LASTCOM	LASTCOM	8	n	n	t	1	t
FROCEGLINE	FORCEGLINE	16	n	n	n	1	t
EXCEPTIONS	EXCEPTIONS	32	n	n	n	1	t
LISTIGNORES	LISTIGNORES	64	n	n	n	1	t
REMIGNORE	REMIGNORE	128	n	n	n	1	t
LIST	LIST	256	n	n	n	1	t
COMMANDS	COMMANDS	512	n	n	n	1	t
\.
