/*===========================================================================*/
/* SEED reader     |                timeprt                |         utility */
/*===========================================================================*/
/*
	Name:		timeprt
	Purpose:	printf of int struct time format
	Usage:		int timecmp ();
				struct time time1;
	Input:		time1 = 1st time in struct time format (see Notes)
	Output:		time output
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
	Author:		Dennis O'Neill
	Revisions:	Date      Revised by      Comment
*/

#include "rdseed.h"
#define TOLERANCE 1								/* tolerance in .0001 secs */

int timeprt (time)
struct time time;
{
	/* print year */
	printf("%4d,",time.year);

	/* print day */
	printf("%3d,",time.day);

	/* print hour */
	printf("%2d:",time.hour);

	/* print minute */
	printf("%02d:",time.minute);

	/* print second */
	printf("%02d.",time.second);

	/* print fractional second */
	printf("%04d",time.fracsec);

}
