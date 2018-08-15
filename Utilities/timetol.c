/*===========================================================================*/
/* SEED reader     |                timetol                |         utility */
/*===========================================================================*/
/*
	Name:		timetol
	Purpose:	compare two times int struct time format
	Usage:		int timetol ();
				struct time time1;
				struct time time2;
				int result;
				result = timetol (time1, time2);
	Input:		time1 = 1st time in struct time format (see Notes)
				time2 = 2nd time in struct time format (see Notes)
	Output:		result =  0 if time1 == time2
				       = -1 if time1 < time2
				       =  1 if time1 > time2
	Externals:	none
	Warnings:	none
	Errors:		none
	Called by:	anything
	Calls to:	none
	Algorithm:	check each element of the time structure; if an inequality is
				found, return the appropriate value.  If no inequalities are
				found, times are equal.  Checking is done in the order
				fractional seconds, seconds, . . . , year due to decreasing
				likelihood of inequality.
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
	Author:		Dennis O'Neill
	Revisions:	Date      Revised by      Comment
				07/15/88  Dennis O'Neill  Initial preliminary release 0.9
				11/11/88  Dennis O'Neill  added tolerance to fractional second
				                          comparison to allow for roundoff
				11/21/88  Dennis O'Neill  Production release 1.0
*/

#include "rdseed.h"

/* #define isaleap(year) (((year%100 != 0) && (year%4 == 0)) || (year%400 == 0)) */

int timetol (time1, time2, nsamples, samplesPerSec)
struct time time1;
struct time time2;
int nsamples;
float samplesPerSec;

{
	double diff_sec; unsigned int sec1, sec2;

	float secsPerSample;

	if (samplesPerSec == 0)
		return;

	/* check year */

	sec1 = time1.second;
	sec1 += time1.minute*60;   /* how many secs in a minute */
	sec1 += time1.hour*3600;   /* # of secs in an hour */
	sec1 += time1.day*3600*24; /* num = #secs in hour * 24 */


	if isaleap(time1.year) 
		sec1 += ((time1.year-1980)*3600)*24*366;
	else 
		sec1 += ((time1.year-1980)*3600)*24*365;

	sec2 = time2.second;
	sec2 += time2.minute*60;
	sec2 += time2.hour*3600;
	sec2 += time2.day*3600*24;
	
	/* add in the years */
	if isaleap(time2.year) 
		sec2 += ((time2.year-1980)*3600)*24*366;
	else 
		sec2 += ((time2.year-1980)*3600)*24*365;

	diff_sec = ((double) sec1) - ((double)sec2);

	diff_sec += (time1.fracsec - time2.fracsec)/10000.0;

	if (diff_sec < 0)
		diff_sec = -diff_sec;

	secsPerSample = 1/samplesPerSec;

        /* if diff is less than 1/2 the sample period... ok */
        if (diff_sec < ((secsPerSample/2)))
        {       
                return 0;
	}

	/* gap is larger than 1/2 sample period. check against tolerance */
	if (current_channel != NULL)
	{
		if (diff_sec > (nsamples*current_channel->clock_tolerance)) 
		{
			/* make check against user inut fudge factor  */
			if (diff_sec < adj_tolerance)
			{
				return 0;
			}

			return (1);
		}
	}
	else
	{
		/* make check against user inut fudge factor */
		if (diff_sec < adj_tolerance)
			return 0;
	}
	
	/* if I got this far, times are equal */
	return (0);

}
