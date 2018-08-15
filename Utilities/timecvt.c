/*===========================================================================*/
/* SEED reader     |                timecvt                |         utility */
/*===========================================================================*/
/*
	Name:		timecvt
	Purpose:	convert string time to integer structure
	Usage:		int timecvt (time, string);
				struct time *time;
				char *string;
				timecvt (time, string);
	Input:		time = pointer to time structure
				string = pointer to string representation of time
							yyyy,ddd,hh:mm:ss.ffff
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
#include <time.h>

#include "rdseed.h"

/* #define isaleap(year) (((year%100 != 0) && (year%4 == 0)) || (year%400 == 0)) */

static int days_in_month[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

void timecvt (time, string)
struct time *time;
char *string;
{
	char frac_buff[5] = {"0000"};

	char buffer[40], *p;
	int month, mon_day, i;

	time->fracsec = 0;
	time->second = 0;
	time->minute = 0;
	time->hour = 0;
	time->day = 0;
	time->year = 0;

	if (string == NULL) return;

	strncpy(buffer,string,40);
	buffer[39] = '\0';

	if (NULL == strchr(buffer,'/')) mon_day = FALSE;
	else mon_day = TRUE;

	/* extract year */
	p = strtok(buffer,":,./");
	if (p != NULL)
		{
		time->year = atoi(p);
		if (time->year < 100) time->year += 1900;
		}
	else return;
	time->day = 1;	/* Default day if year found */

	/* extract day */
	p = strtok(NULL,":,./");
	if (p != NULL)
		{
		if (mon_day)
			{
			month = atoi(p);
			if (month==0) month = 1;
			/* parse day number */
			p = strtok(NULL,":,./ ");
			if (p != NULL)
				{
				time->day = atoi(p);
				for (i=0;i<month-1;i++) 
					time->day += days_in_month[i];

				if (isaleap(time->year) && month>2) 
					time->day++;
				}
			else
				{
				for (i=0;i<month-1;i++) 
					time->day += days_in_month[i];
				if (isaleap(time->year) && month>2) 
					time->day++;
				return;
				}
			}
		else time->day = atoi(p);
		}
	else return;
	
	/* extract hour */
	p = strtok(NULL,":,./");
	if (p != NULL) time->hour = atoi(p);
	else return;

	/* extract minute */
	p = strtok(NULL,":,./");
	if (p != NULL) time->minute = atoi(p);
	else return;

	/* extract second */
	p = strtok(NULL,":,./");
	if (p != NULL) time->second = atoi(p);
	else return;

	/* extract fractional seconds */
	p = strtok(NULL,":,./");
	if (p != NULL)
	{ 
		strncpy(frac_buff, p, strlen(p));

		time->fracsec = atoi(frac_buff);
	}

	else return;

	return;
}

// converts a SEED time string into a calendar style
// yyyyDDDstr must look like;  YYYY,DDD,hh:mm:ss.ffff
// 

void cvttime(char *yyyyDDMMstr, struct time *t)
{
	int i;

	int now_d;

	char mon_num[20];
	char day_num[20];
	char num[20];

	sprintf(yyyyDDMMstr, "%d", t->year);

	now_d = t->day;

	if (isaleap(t->year)) 
		days_in_month[1] = 29;
	else
		days_in_month[1] = 28;

	for (i = 0; i < 12; i++)
	{
		if (now_d <= days_in_month[i])
			break;

		now_d -= days_in_month[i];
	}

	// since months started at 0, increment for right month
	i++;

	// if is leap year and after feb, add a day

	sprintf(mon_num, "%02d", i);
	sprintf(day_num, "%02d", now_d);

	strcat(yyyyDDMMstr, "-");
	 
	strcat(yyyyDDMMstr, mon_num);
	 
	strcat(yyyyDDMMstr, "-");
	 
	strcat(yyyyDDMMstr, day_num);
	 
	strcat(yyyyDDMMstr, "T");

	sprintf(num, "%02d", t->hour);

	strcat(yyyyDDMMstr, num);

	strcat(yyyyDDMMstr, ":");

	sprintf(num, "%02d", t->minute);

	strcat(yyyyDDMMstr, num);

	strcat(yyyyDDMMstr, ":");

	sprintf(num, "%02d", t->second);

	strcat(yyyyDDMMstr, num);

}

void phasecvt (phase, string)
struct phase *phase;
char *string;
{
	char buffer[100], *p;
	int sign, i;

	phase->name[0] = '\0';			/* Null out return parms */
	phase->fracsec = 0;
	phase->second = 0;
	phase->minute = 0;
	phase->hour = 0;
	phase->day = 0;
	phase->year = 0;

	strncpy(buffer,string,100);
	buffer[99] = '\0';

/* extract phase */
	p = strtok(buffer,":,");
	if (p != NULL)
	{
		if (isdigit(*p)) return;
		else strcpy(phase->name, p);
	}
	else return;

/* extract day */
	p = strtok(NULL,":,");
	if (p != NULL)
		{
		phase->day = atoi(p);
		while (*p == ' ') p++;
		if (*p == '-') sign = -1;
		else sign = 1;
		}
	else return;
	
/* extract hour */
	p = strtok(NULL,":,");
	if (p != NULL) phase->hour = atoi(p);
	else return;
	phase->hour = sign*phase->hour;

/* extract minute */
	p = strtok(NULL,":,");
	if (p != NULL) phase->minute = atoi(p);
	else return;
	phase->minute = sign*phase->minute;

/* extract second */
	p = strtok(NULL,":,");
	if (p != NULL) phase->second = atoi(p);
	else return;
	phase->second = sign*phase->second;

/* extract fractional seconds */
	p = strtok(NULL,":,");
	if (p != NULL) phase->fracsec = atoi(p);
	else return;
	phase->fracsec = sign*phase->fracsec;

	return;
}
