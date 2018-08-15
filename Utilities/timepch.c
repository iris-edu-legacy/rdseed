/*===========================================================================*/
/* SEED reader     |                timepch                |         utility */
/*===========================================================================*/
/*
	Name:		timepch
	Purpose:	convert string time to integer structure
	Usage:		int timepch (time, epc_time);
				struct time *time;
				unsigned long *epc_time;
				timepch (time, epc_time);
	Input:		time = pointer to time structure
				epc_time = pointer to epoch time result location
	Output:	none
	Externals:	none
	Warnings:	none
	Errors:		none
	Called by:	anything
	Calls to:	none
	Algorithm:
	Notes:		The time structure looks like:
					struct time
					{
						int year;
						int day;
						int hour;
						int minute;
						int second;
						int fracsec;
					};
	Problems:	none known
	References:	none
	Language:	C, hopefully ANSI standard
	Author:		Allen Nance
	Revisions:	Date      Revised by      Comment
				08/09/90  Allen Nance   Initial preliminary release 2.2
*/

#include "rdseed.h"

/* #define isaleap(year) (((year%100 != 0) && (year%4 == 0)) || (year%400 == 0)) */

int timepch (time)
struct time *time;
{
	long time_sec;

	time_sec  = time->second;
	time_sec += time->minute  * 60;
	time_sec += time->hour    * 3600;
	time_sec += (time->day-1) * 3600 * 24;
	if (isaleap(time->year))
		time_sec += (time->year-1970)*366*24*3600;
	else
		time_sec += (time->year-1970)*365*24*3600;

	return (time_sec);
}
