#
# Regular cron jobs for the ergo package
#
0 4	* * *	root	[ -x /usr/bin/ergo_maintenance ] && /usr/bin/ergo_maintenance
