/*===========================================================================*/
/* SEED reader     |                timecmp                |         utility */
/*===========================================================================*/
/*
	Name:	timecmp
	Purpose:compare two times int struct time format
	Usage:	int timecmp ();
			struct time time1;
			struct time time2;
			int result;
			result = timecmp (time1, time2);
	Input:	time1 = 1st time in struct time format (see Notes)
		time2 = 2nd time in struct time format (see Notes)
	Output:	result =  0 if time1 == time2
				       = -1 if time1 < time2
				       =  1 if time1 > time2
	Externals:none
	Warnings:none
	Errors:	none
	Called by:anything
	Calls to:none
	Algorithm:check each element of the time structure; if an inequality is
		found, return the appropriate value.  If no inequalities are
		found, times are equal.  Checking is done in the order
		fractional seconds, seconds, . . . , year due to decreasing
		likelihood of inequality.
	Notes:	The time structure looks like:
			struct time
			{
				int year;
				int day;
				int hour;
				int minute;
				int second;
				int fracsec;
			};
	Problems:none known
	References:none
	Language:C, hopefully ANSI standard
	Author:	Dennis O'Neill
	Revisions:Date      Revised by      Comment
	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
	11/11/88  Dennis O'Neill  added tolerance to fractional second
	                          comparison to allow for roundoff
	11/21/88  Dennis O'Neill  Production
*/

#include "rdseed.h"
#define TOLERANCE 1								/* tolerance in .0001 secs */

int timecmp (time1, time2)
struct time time1;
struct time time2;
{
	/* check year */
	if (time1.year < time2.year) return (-1);
	if (time1.year > time2.year) return (1);

	/* check day */
	if (time1.day < time2.day)  return (-1);
	if (time1.day > time2.day)  return (1);

	/* check hour */
	if (time1.hour < time2.hour) return (-1);
	if (time1.hour > time2.hour) return (1);

	/* check minute */
	if (time1.minute < time2.minute) return (-1);
	if (time1.minute > time2.minute) return (1);

	/* check second */
	if (time1.second < time2.second) return (-1);
	if (time1.second > time2.second) return (1);

	/* check fractional second */
	if (time1.fracsec < (time2.fracsec - TOLERANCE)) return (-1);
	if (time1.fracsec > (time2.fracsec + TOLERANCE)) return (1);

	/* if I got this far, times are equal */
	return (0);

}
