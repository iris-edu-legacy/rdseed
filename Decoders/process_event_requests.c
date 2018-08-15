/*===========================================================================*/
/* SEED reader     |       process_event_requests          |                 */
/*===========================================================================*/
/*
	Name:		process_event_requests
	Purpose:	process user look-up requests for data based on event times
	Usage:		void process_event_requests ();

	Input:		none - uses information in globally-available structure
	Output:		none
	Externals:	type71_head - 1st entry in table; defined in structures.h, 
					allocated in globals.h
	Warnings:	none
	Errors:		none
	Called by:	
	Calls to:	none
	Algorithm:	
	Notes:		
	Problems:	none known
	References:	
	Language:	C, hopefully ANSI standard
	Author:		Allen Nance
	Revisions:
*/

#include "rdseed.h"

process_event_requests ()
{
	int i;

	start_time_count = 0;
	end_time_count = 0;

	start_time_point = NULL;
	end_time_point = NULL;


	for (i=0; i<start_phase_count; i++)
	{
		rdseed_strupr(start_phase_point[i].name);
		if (i < end_phase_count) rdseed_strupr(end_phase_point[i].name);
		if (strcmp(start_phase_point[i].name,"ORIGIN") == 0)
		{
			if (process_origin_requests(i)) return(1);
			else return(0);
		}
		else process_phase_requests(i);

	}
}


/*===========================================================================*/
/* SEED reader     |       process_origin_requests         |                 */
/*===========================================================================*/
/*
*/
process_origin_requests(phase_number)
int phase_number;
{
	struct type71 *type71;							/* looping vbl */
	struct time    e_time;
	int i;

	if (type71_head != NULL)
	{
		for (type71 = type71_head; type71 != NULL; type71 = type71->next)
		{
			timecvt(&e_time, type71->origin_time);
			if (event_start_time_count > phase_number)
			{
				if ( timecmp (e_time, event_start_time_point[phase_number]) >= 0)
					if (event_end_time_count > phase_number)
					{
						if ( timecmp (e_time, event_end_time_point[phase_number]) <= 0)
							origin_time_span(phase_number, e_time);
					}
					else origin_time_span(phase_number, e_time);
			}
			else
			{
				if (event_end_time_count > phase_number)
				{
					if ( timecmp (e_time, event_end_time_point[phase_number]) <= 0)
						origin_time_span(phase_number, e_time);
				}
				else origin_time_span(phase_number, e_time);
			}
		}
	}
	else
	{
		fprintf(stderr, "\tERROR - No ORIGIN Times Found\n");
		return(0);
	}
	return(1);
}

/*===========================================================================*/
/* SEED reader     |            origin_time_span            |                 */
/*===========================================================================*/
/*
*/
origin_time_span(phase_number, e_time)
int phase_number;
struct time e_time;
{
	int i;
	
	fprintf(stderr, "Process origin time "); timeprt(e_time); fprintf(stderr, "\n");

	if (start_time_count)
		start_time_point = (struct time *) realloc((char *)start_time_point, sizeof(struct time)*(start_time_count+1));
	else
		start_time_point = (struct time *) malloc(sizeof(struct time));

	start_time_point[start_time_count] = timeaddphase(e_time, start_phase_point[phase_number]);

	start_time_count++;

	if (end_time_count)
		end_time_point = (struct time *) realloc((char *)end_time_point, sizeof(struct time)*(end_time_count+1));
	else
		end_time_point = (struct time *) malloc(sizeof(struct time));

	if (end_phase_count > phase_number)
	{
		if (null_phase_time(end_phase_point[phase_number]))
			end_time_point[end_time_count].year = 9999;
		else	
			end_time_point[end_time_count] = timeaddphase(e_time,end_phase_point[phase_number]);
	}
	else end_time_point[end_time_count].year = 9999;

	end_time_count++;
}

/*===========================================================================*/
/* SEED reader     |            null_phase_time            |                 */
/*===========================================================================*/
/* check for a null (all zeroes) time in a phase structure */
null_phase_time(phase)
struct phase phase;
{
	if (phase.year != 0) return(0);
	if (phase.day != 0) return(0);
	if (phase.hour != 0) return(0);
	if (phase.minute != 0) return(0);
	if (phase.second != 0) return(0);
	if (phase.fracsec != 0) return(0);
	return(1);
}

/*===========================================================================*/
/* SEED reader     |         process_phase_requests        |     time header */
/*===========================================================================*/
/*
*/
/* In the folowing, s_time was computed but not used. This is because I haven't found a way to select which
phase to use when there are multiple of the same type */
process_phase_requests(phase_number)
int phase_number;
{
	struct type72 *type72, *tmp72;							/* looping vbl */
	struct time    p_time, s_time;
	char           cbuf[31];
	int i, count;

	count = 0;
	if (type72_head != NULL)
	{
		for (type72 = type72_head; type72 != NULL; type72 = type72->next)
		{
			strncpy(cbuf, type72->phasename, 30); cbuf[30] = '\0'; rdseed_strupr(cbuf);
			if (strncmp(start_phase_point[phase_number].name, cbuf, 20) != 0) continue;

			timecvt(&p_time, type72->arrival_time);

			s_time.year = s_time.day = s_time.hour = s_time.minute = s_time.second = s_time.fracsec = 0;
			if (phase_number < end_phase_count)
			{
				for (tmp72 = type72_head; tmp72 != NULL; tmp72 = tmp72->next)
				{
					strncpy(cbuf, tmp72->phasename, 30); cbuf[30] = '\0'; rdseed_strupr(cbuf);
					if (strncmp(end_phase_point[phase_number].name, cbuf, 20) == 0) break;
				}
				if (tmp72 != NULL)
					timecvt(&s_time, tmp72->arrival_time);
				else
				{
					fprintf(stderr, "\tERROR - Ending Phase '%s' name not found\n", end_phase_point[phase_number].name);
					continue;
				}
			}
				
					
			if (event_start_time_count > phase_number)
			{
				if ( timecmp (p_time, event_start_time_point[phase_number]) >= 0)
					if (event_end_time_count > phase_number)
					{
						if ( timecmp (p_time, event_end_time_point[phase_number]) <= 0)
						{
							phase_time_span(phase_number, p_time, p_time/*s_time*/);
							count++;
						}
					}
					else
					{
						phase_time_span(phase_number, p_time, p_time/*s_time*/);
						count++;
					}
			}
			else
			{
				if (event_end_time_count > phase_number)
				{
					if ( timecmp (p_time, event_end_time_point[phase_number]) <= 0)
						{
						phase_time_span(phase_number, p_time, p_time/*s_time*/);
						count++;
						}
				}
				else
				{
					phase_time_span(phase_number, p_time, p_time/*s_time*/);
					count++;
				}
			}
		}
	}

	if (type72_head == NULL)
	{
		fprintf(stderr, "\tERROR - No PHASE Arrival Times Found\n");
		return(0);
	}
	else if (!count)
	{
		fprintf(stderr, "\tWARNING - No PHASE Arrival Times Found in Specified Interval\n");
		return(0);
	}

	return(1);
}

/*===========================================================================*/
/* SEED reader     |            phase_time_span            |                 */
/*===========================================================================*/
/*
*/
phase_time_span(phase_number, p_time, s_time)
int phase_number;
struct time p_time, s_time;
{
	int i;
	
	fprintf(stderr, "Process Arrival %s at time ", start_phase_point[phase_number].name); timeprt(p_time);
	fprintf(stderr, "\n");

	if (start_time_count)
		start_time_point = (struct time *) realloc((char *)start_time_point, sizeof(struct time)*(start_time_count+1));
	else
		start_time_point = (struct time *) malloc(sizeof(struct time));

	start_time_point[start_time_count] = timeaddphase(p_time, start_phase_point[phase_number]);

	start_time_count++;

	if (end_time_count)
		end_time_point = (struct time *) realloc((char *)end_time_point, sizeof(struct time)*(end_time_count+1));
	else
		end_time_point = (struct time *) malloc(sizeof(struct time));

	if (end_phase_count > phase_number)
	{
		if (null_phase_time(end_phase_point[phase_number]))
			end_time_point[end_time_count].year = 9999;
		else	
			end_time_point[end_time_count] = timeaddphase(s_time,end_phase_point[phase_number]);
	}
	else end_time_point[end_time_count].year = 9999;

	end_time_count++;

}
