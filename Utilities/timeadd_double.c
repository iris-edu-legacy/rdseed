/*===========================================================================*/
/* SEED reader     |           timeadd_double              |         utility */
/*===========================================================================*/
/*
	Name:		timeadd_double
	Purpose:	add some number of seconds to a time of the form yyyyddd
				hh:mm:ss.ffff
	Usage:		struct time timeadd_double ();
				struct time time;
				struct time newtime;
				double increment;
				newtime = timeadd_double (time, increment);
	Input:		time = the original time in struct time form (see Notes)
				increment = number of seconds to add to time
	Output:		newtime = the resultant time in struct time form (see Notes)
	Externals:	none
	Warnings:	none
	Errors:		none
	Called by:	anything
	Calls to:	none
	Algorithm:	convert the input time, exclusive of year and day, to seconds;
				add increment number of seconds to time; convert result to
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
	Problems:	Gives incorrect result if there was a leap second at the year
				boundary
				Gives incorrect result if increment > 1 year
	References:	none
	Language:	C, hopefully ANSI standard
	Author:		Allen Nance
	Revisions:	07/13/92 Adapted from timeadd.c for ver 3.21
*/

#include "rdseed.h"

/*#define isaleap(year) (((year%100 != 0) && (year%4 == 0)) || (year%400 == 0)) */

struct time timeadd_double (time, increment)
struct time time;
double increment;
{
	struct time newtime;						/* result */
	struct time timeinc;						/* time increment */
	int addterm, inc;

	/* initialize year and day */
	newtime.year = time.year;
	newtime.day = time.day;

	/* convert increment to hh:mm:ss.ffff */
	inc = (int)(increment / 10000.0);
	timeinc.fracsec = (int)(increment - ((double)inc * 10000.0));
	timeinc.hour = inc / 3600;
	timeinc.minute = (inc - (timeinc.hour * 3600)) / 60;
	timeinc.second = inc - (timeinc.hour * 3600) - (timeinc.minute * 60);

	/* add the increment to the original time */
	newtime.hour = time.hour + timeinc.hour;
	newtime.minute = time.minute + timeinc.minute;
	newtime.second = time.second + timeinc.second;
	newtime.fracsec = time.fracsec + timeinc.fracsec;

	/* adjust fractional seconds, increment seconds if necessary */
	if (newtime.fracsec >= 10000)
	{
		addterm = newtime.fracsec / 10000;
		newtime.second = newtime.second + addterm;
		newtime.fracsec -= 10000 * addterm;
	}

	/* adjust seconds, increment minutes if necessary */
	if (newtime.second >= 60)
	{
		addterm = newtime.second / 60;
		newtime.minute = newtime.minute + addterm;
		newtime.second -= 60 * addterm;
	}

	/* adjust minutes, increment hours if necessary */
	if (newtime.minute >= 60)
	{
		addterm = newtime.minute / 60;
		newtime.hour = newtime.hour + addterm;
		newtime.minute -= 60 * addterm;
	}

	/* adjust hours, increment days if necessary */
	if (newtime.hour >= 24)
	{
		addterm = newtime.hour / 24;
		newtime.day = newtime.day + addterm;
		newtime.hour -= 24 * addterm;
	}

	/* increment year if necessary, allowing for leap year */
	if ((!isaleap (time.year)) && (newtime.day > 365))
	{
		newtime.year += 1;
		newtime.day -= 365;
	}
	else if ((isaleap (time.year)) && (newtime.day > 366))
	{
		newtime.year += 1;
		newtime.day -=366;
	}

	return (newtime);
}
