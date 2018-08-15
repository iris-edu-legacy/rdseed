/*===========================================================================*/
/* SEED reader     |            timeaddphase               |         utility */
/*===========================================================================*/
/*
	Name:		timeaddphase
	Purpose:	add some number of seconds to a time of the form yyyyddd
				hh:mm:ss.ffff
	Usage:		struct time timeaddphase ();
				struct time time;
				struct time newtime;
				int increment;
				newtime = timeadd (time, phase);
	Input:		time = the original time in struct time form (see Notes)
				phase = phase time structure to add to time
	Output:		newtime = the resultant time in struct time form (see Notes)
	Externals:	none
	Warnings:	none
	Errors:		none
	Called by:	anything
	Calls to:	none
	Algorithm:	add phase structure to time; convert result to
			hours, minutes, seconds, and fractional seconds, allowing for
			change of day and year.
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
			The phase structure looks like:
					struct phase
					{
						char name[30];
						int year;
						int day;
						int hour;
						int minute;
						int second;
						int fracsec;
					};
	Problems:	Gives incorrect result if there was a leap second at the year
				boundary
				Gives incorrect result if increment > 1 year
	References:	none
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
			06/30/92  adapted from above to current by Allen Nance
*/

#include "rdseed.h"

/*#define isaleap(year) (((year%100 != 0) && (year%4 == 0)) || (year%400 == 0)) */

struct time timeaddphase (time, phase)
struct time time;
struct phase phase;
{
	struct time newtime;						/* result */

	int addterm;

	/* initialize year and day */
	newtime.year = time.year;

	/* add the phase to the original time */
	newtime.day = time.day + phase.day;
	newtime.hour = time.hour + phase.hour;
	newtime.minute = time.minute + phase.minute;
	newtime.second = time.second + phase.second;
	newtime.fracsec = time.fracsec + phase.fracsec;

	/* adjust fractional seconds, increment seconds if necessary */
	if (newtime.fracsec >= 10000)
	{
		addterm = newtime.fracsec / 10000;
		newtime.second = newtime.second + addterm;
		newtime.fracsec -= 10000 * addterm;
	}
	else if (newtime.fracsec < 0)
	{
		addterm = (newtime.fracsec / 10000) + 1;
		newtime.second = newtime.second - addterm;
		newtime.fracsec += 10000 * addterm;
	}

	/* adjust seconds, increment minutes if necessary */
	if (newtime.second >= 60)
	{
		addterm = newtime.second / 60;
		newtime.minute = newtime.minute + addterm;
		newtime.second -= 60 * addterm;
	}
	else if (newtime.second < 0)
	{
		addterm = (newtime.second / 60) + 1;
		newtime.minute = newtime.minute - addterm;
		newtime.second += 60 * addterm;
	}

	/* adjust minutes, increment hours if necessary */
	if (newtime.minute >= 60)
	{
		addterm = newtime.minute / 60;
		newtime.hour = newtime.hour + addterm;
		newtime.minute -= 60 * addterm;
	}
	else if (newtime.minute < 0)
	{
		addterm = (newtime.minute / 60) + 1;
		newtime.hour = newtime.hour - addterm;
		newtime.minute += 60 * addterm;
	}

	/* adjust hours, increment days if necessary */
	if (newtime.hour >= 24)
	{
		addterm = newtime.hour / 24;
		newtime.day = newtime.day + addterm;
		newtime.hour -= 24 * addterm;
	}
	else if (newtime.hour < 0)
	{
		addterm = (newtime.hour / 24) + 1;
		newtime.day = newtime.day - addterm;
		newtime.hour += 24 * addterm;
	}

	/* increment year if necessary, allowing for leap year */
	if (newtime.day > (isaleap (time.year) ? 366 : 365))
	{
		newtime.day -= (isaleap (time.year) ? 366 : 365);
		newtime.year += 1;
	}
	else if (newtime.day < 1)
	{
		newtime.year -= 1;
		newtime.day += (isaleap (time.year) ? 366 : 365);
	}

	return (newtime);
}
