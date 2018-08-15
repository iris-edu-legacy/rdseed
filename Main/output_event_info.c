
/*===========================================================================*/
/* SEED reader     |           output_event_info           |    subprocedure */
/*===========================================================================*/
/*
	Name:	output_event_info
	Purpose:write an event file and a station file from header tables
for selected station channels
	Usage:
		void output_event_info();
	Input:	none (gets its data from globally-available tables and files)
	Output:		none (writes an event/station files;
	Externals:	type_50_head
	Warnings:	unable to open response file for writing
   			failure to properly write the response data
	Errors:		none
	Called by:  Main	
	Calls to:	none
	Algorith:

	Problems:	none known
	Author:		Chris Laughbon, March 28th, 1995	
	Revisions:
*/

#include <sys/param.h>
#include "rdseed.h"		/* SEED tables and structures */


#define EVENT_FILE_NAME "rdseed.events"
#define STATION_FILE_NAME "rdseed.stations"

#define SUBSTR(str,xch,ch)      \
        { char *c; \
          while (c = strchr(str, xch)) \
                *c = ch; \
        }


char *cnvt_time();
char *get_src_name();

void output_event_info()

{
	char buffer[1024];	/* buffer for output string */
	char buff[1024];	/* more room */

	FILE *event_fptr;
	struct type71 *ev_ptr;
	struct type71sub *mag_ptr;

	int ix;

	char orig_dir[MAXPATHLEN];

	getcwd(orig_dir, MAXPATHLEN);

	chdir(output_dir);

	/* open output files */
	if ((event_fptr = fopen(EVENT_FILE_NAME, "w")) == NULL)
	{
		fprintf(stderr, "Error - unable to open event file!\n");
		perror("output_event_info");

		chdir(orig_dir);

		return;
	}

	for (ev_ptr = type71_head; ev_ptr != NULL; ev_ptr = ev_ptr->next)
	{
		strcpy(buff, get_src_name(ev_ptr->source_code));

		SUBSTR(buff, ',', ' ');	/* make sure there are no commas
					 * entered into the source, as it 
					 * will really messup our comma 
					 * delimited scheme
					 */

		printf("Writing event:%s to file:%s\n", ev_ptr->origin_time, EVENT_FILE_NAME);

		sprintf(buffer, "%s, %s, %-+3.4f, %-+3.4f, %-+5.0f, %3d, %4d, ",
				buff,
				cnvt_time(ev_ptr->origin_time),
				ev_ptr->latitude,
				ev_ptr->longitude,
				ev_ptr->depth,
				type10.version >= 2.3 ? 
					ev_ptr->seismic_region : 0,
				type10.version >= 2.3 ? 
					ev_ptr->seismic_location : 0);

		/* now append the magnitudes */

		for (ix = 0; ix < ev_ptr->number_magnitudes; ix++)
		{
			sprintf(buff, "%s, %-2.1f, ", 
					ev_ptr->magnitude[ix].type,
					ev_ptr->magnitude[ix].magnitude);

			strcat(buffer, buff);
	
		}

		/* eliminate the ending comma */
		buffer[strlen(buffer) - 2] = 0;

		if (fprintf(event_fptr, "%s\n", buffer) == -1)
		{
                	fprintf(stderr, "Error - unable to write to the event file!\n");
                	perror("output_event_info");

			fclose(event_fptr);

			chdir(orig_dir);

                	return;

		}

	}		/* events */


/*===========================================================================*/
/*                 +=======================================+                 */
/*=================|          close the output files       |=================*/
/*                 +=======================================+                 */

	fclose(event_fptr);

	chdir(orig_dir);

	return;

}

void output_station_info()

{
	FILE *stn_fptr;

	struct type50 *stn_ptr;
	struct type52 *chn_ptr;

	char buff[1024];	/* */


	char orig_dir[MAXPATHLEN];

	getcwd(orig_dir, MAXPATHLEN);

	chdir(output_dir);

	if ((stn_fptr = fopen(STATION_FILE_NAME, "w")) == NULL)     
    	{
        	fprintf(stderr, "Error - unable to open station info file!\n"); 
        	perror("output_stn_info"); 

		chdir(orig_dir);

        	return;
    	}

	
/*                 +=======================================+                 */
/*=================|      output selected station          |=================*/
/*                 +=======================================+                 */

	for (stn_ptr = type50_head; stn_ptr != NULL; stn_ptr = stn_ptr->next)
	{
		strcpy(buff, "");

		/* out to file */

		fprintf(outputfile, "Writing station info for network/station: %s,%s to file:%s\n", 
					stn_ptr->network_code,
					stn_ptr->station,	
					STATION_FILE_NAME);
		

		fprintf(stn_fptr, "%s %s %3.4f %3.4f %5.0f ", 
				stn_ptr->station,
				type10.version >= 2.3 ? stn_ptr->network_code : "N/A",
				stn_ptr->latitude,
				stn_ptr->longitude,
				stn_ptr->elevation);

		for (chn_ptr = stn_ptr->type52_head; 
		 		chn_ptr !=NULL; chn_ptr = chn_ptr->next)
		{
			/* maybe updated records, multiple channels, skip */
			if (strstr(buff, chn_ptr->channel))
				continue;
				

			strcat(buff, chn_ptr->channel);
			strcat(buff, " ");
		}

		if (fprintf(stn_fptr, "\"%s\" \"%s\" %s %s\n", 
				buff,
				stn_ptr->name,
			stn_ptr->start == NULL ? "1900" : stn_ptr->start,
			stn_ptr->end == NULL ? "2500,365,23:59:59.9999" :
				stn_ptr->end) == -1)
		{
                	fprintf(stderr, "Error - unable to write to the station file!\n");

                	perror("output_station_info");

			fclose(stn_fptr);

			chdir(orig_dir);

                	return;

		}


	}		/* foreach station */

	chdir(orig_dir);

	fclose(stn_fptr);

	return;

}

/* ------------------------------------------------------------------------- */
char *ddd2yyyymmdd();

#define isaleap(year) ((((year)%100 != 0) && ((year)%4 == 0)) || ((year)%400 == 0))

/* -----------------------------------------------------------------------
*/
static int days_in_month[] = {0, 31, 28, 31, 30, 31, 30,
                                31, 31, 30, 31, 30, 31};


char *cnvt_time(seed_time)
char *seed_time;

{
	char *ch_ptr;
	static char YYYYMMDDHHMMSSFFFF[100];
	int year, ddd;

	/* seed_time is YYYY,DDD:etc */
	/* convert to calendar time YYYY/MM/DD:etc */

	/* get the year */
	ch_ptr = strtok(seed_time, ",");

	if (ch_ptr == NULL)
		return ("");

	year = atoi(ch_ptr);

	/* get the day */
        ch_ptr = strtok(NULL, ",");

        if (ch_ptr == NULL) 
		ddd = 1;	/* defaults to jan 1st */
	else
        	ddd = atoi(ch_ptr); 

        /* leave ch_ptr pointing at HHMMSSetc if there */

	strcpy(YYYYMMDDHHMMSSFFFF, ddd2yyyymmdd(ddd, year));	 

	ch_ptr = strtok(NULL, " ");

	if (ch_ptr != NULL)
		/* tack on the time */
		strcat(YYYYMMDDHHMMSSFFFF, ch_ptr);
	
        return YYYYMMDDHHMMSSFFFF;
}

/* ------------------------------------------------------------------------ */
static char yyyymmdd[200];


char *ddd2yyyymmdd(ddd, yyyy)
int ddd;
int yyyy;

{


        int mon;

        /* adjust for leap year */

        if (isaleap(yyyy))
                days_in_month[2] = 29;
	else
		/* always reset as it could be run again on a different year */
		days_in_month[29] = 28;

 
        for (mon = 1; mon < 13; mon++)
        {
                if (ddd <= days_in_month[mon])
                        break;
 
                ddd -= days_in_month[mon];
 
        }
 
        if (mon == 13)
        {
                fprintf(stderr, "Bad day number!\n");
                return;
        }
 
 
        sprintf(yyyymmdd, "%d/%02d/%02d ", yyyy, mon, ddd);
 
        return yyyymmdd;

}

/* ------------------------------------------------------------------------ */
char *get_src_name(s_code)
int s_code;

{

	struct type32 *ptr = type32_head;

	while (ptr)
	{
		if (ptr->code == s_code)
			return ptr->author;

		ptr = ptr->next;
	}

	return "No Source found in blockett 32";

}

/* ------------------------------------------------------------------------ */
