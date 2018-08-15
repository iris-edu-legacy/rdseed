/* -------------------------------------------------------------------------- 
 * - module: get_date()
 * -
 * - author: Chris Laughbon
 * - date  : Dec 7, 1994
 * - purpose: gets the system date and time, returns date/time in seed time
 * -		  format; YYYY,DDD,HH:MM:SS.FFFF
 * - 
 * - Note:	  the fractional part is always zero!
 * -
 * ---------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <pwd.h>

char *get_date(char *date_buf, int l)

{
	struct timeval tp;
	struct tm *tm;

	gettimeofday(&tp, (void *)NULL);

	tm = (struct tm *)localtime((time_t *)&tp.tv_sec);

	strftime(date_buf, l, "%Y,%j,%H:%M:%S.0000", tm);

	return date_buf;
	
}

/* ------------------------------------------------------------------------ */

