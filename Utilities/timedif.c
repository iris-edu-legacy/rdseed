/*===========================================================================*/
/* SEED reader     |              timedif                  |         utility */
/*===========================================================================*/
/*
	Name:		timedif
	Purpose:	compute difference between two times.
	Usage:		int timedif ();
				struct time time1;
				struct time time2;
				duration = timedif (time1, time2);
	Input:		time1 = the original time in struct time form (see Notes)
				time2 = the original time in struct time form (see Notes)
	Output:		dif = the time difference in fraction of seconds
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
				Gives incorrect result if difference > 1 year
	References:	none
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/21/88  Dennis O'Neill  Production release 1.0
			07/13/92 Allen Nance returns double for spans gerater than 2.48 days
*/


#include <time.h>

#include "rdseed.h"

#define TIME_PRECISION 10000

time_t compute_time(int yy, int ddd, int h, int m, int s);

double timedif (time1, time2)
struct time time1;
struct time time2;
{
	double time1_sec, time2_sec, dif;

	time_t secs1, secs2;

        secs1 = compute_time(time1.year, time1.day, time1.hour, time1.minute, time1.second);
        secs2 = compute_time(time2.year, time2.day, time2.hour, time2.minute, time2.second);

        time1_sec = (secs1 * TIME_PRECISION) + time1.fracsec;
        time2_sec = (secs2 * TIME_PRECISION) + time2.fracsec;

        return (time2_sec - time1_sec);

}

time_t compute_time(int yy, int ddd, int h, int m, int s)
{
        int y;
        time_t secs;

        secs = 0;

        for (y = 1970; y < yy; y++)
                secs += 86400 * (isaleap(y) ? 366 : 365);

        secs += (ddd - 1) * 86400 + h * 3600 + m * 60 + s;

        return secs;
}



