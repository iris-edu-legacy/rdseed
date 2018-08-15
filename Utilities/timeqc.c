/*===========================================================================*/
/* SEED reader     |                timeqc                 |         utility */
/*===========================================================================*/
/*
	Name:		timeqc
	Purpose:	compare two times int struct time format
	Usage:		int timeqc ();
				struct time time1;
				struct time time2;
				int result;
				result = timeqc (time1, time2);
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

int timeqc (time1, time2, nsamples, sample_rate)
struct time time1;
struct time time2;
int nsamples;
float sample_rate;
{
	double diff_sec, calc_sample_rate, tol; int sec1, sec2;

	if (sample_rate == 0.0) return(1);

	sec1 = time1.second;
	sec1 += time1.minute*60;
	sec1 += time1.hour*3600;
	sec1 += time1.day*3600*24;
	if isaleap(time1.year) sec1 += ((time1.year-1980)*3600)*24*366;
	else sec1 += ((time1.year-1980)*3600)*24*365;

	sec2 = time2.second;
	sec2 += time2.minute*60;
	sec2 += time2.hour*3600;
	sec2 += time2.day*3600*24;
	if isaleap(time1.year) sec2 += ((time2.year-1980)*3600)*24*366;
	else sec2 += ((time2.year-1980)*3600)*24*365;

	diff_sec = (double) (sec2-sec1);
/*	diff_sec = diff_sec; */
	diff_sec += ((double)(time2.fracsec - time1.fracsec))/10000.0;

	calc_sample_rate = ((double)nsamples)/diff_sec;

	tol = sample_rate*(current_channel->clock_tolerance == 0?1:current_channel->clock_tolerance)*sample_rate*adj_tolerance;

/*	printf("s1:%d.%d s2:%d.%d n:%d sr:%f csr:%f tol:%f\n", sec1,time1.fracsec, sec2,time2.fracsec, nsamples, sample_rate, calc_sample_rate,tol); */

	if ( ((sample_rate - calc_sample_rate) < -tol) || ((sample_rate - calc_sample_rate) > tol)) return(1);


	/* if I got this far, times are equal */

	sample_rate_accum += calc_sample_rate;  sample_rate_count += 1;

	return (0);

}
