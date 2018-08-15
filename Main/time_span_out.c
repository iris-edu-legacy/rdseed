/*=========================================================================*/
/* SEED reader     |           time_span_out             |    subprocedure */
/*=========================================================================*/
/*
	Name:		time_span_out
	Purpose:	to select seismic data according to user time span input.
	Usage:		void time_span_out ();
				time_span_out ();
	Input:	none - all data taken from globally available variables.
	Output:		none
	Externals: data_hdr
			start_time_count
			start_time_point
			end_time_count
			end_time_point
	Warnings:	
	Errors:		none
	Called by:	process_data
	Calls to:	none
	Algorithm:
	Problems:	none known
	References:	Halbert, S. E., R. Buland, and C. R. Hutt (1988).  Standard for
	the Exchange of Earthquake Data (SEED), Version V2.0,
	February 25, 1988.  United States Geological Survey,
	Albuquerque Seismological Laboratory, Building 10002,
	Kirtland Air Force Base East, Albuquerque, New Mexico
	87115.  82 pp.
	O'Neill, D. (1987).  IRIS Interim Data Distribution Format
	(SAC ASCII), Version 1.0 (12 November 1987).  Incorporated
	Research Institutions for Seismology, 1616 North Fort Myer
	Drive, Suite 1440, Arlington, Virginia 22209.  11 pp.
	Tull, J. (1987).  SAC User's Manual, Version 10.2, October 7,
	1987.  Lawrence Livermore National Laboratory, L-205,
	Livermore, California 94550.  ??? pp.
	Language:	C, hopefully ANSI standard
	Author:		Allen Nance
	Revisions:
		07/94    CL - took out the breaks so that if the user
				specified time slices they would actually
				get processed.

		01/25/96 CL - cast the offset * TIME_PRECISION to a 
				float to avoid overflow errors.

*/

#include "rdseed.h"								/* SEED tables and structures */

#define TIME_PRECISION 10000

extern int read_summary_flag;   /* defined and set in main() */

struct twindow			/* used if from summary file */
{
        char time_start[23];
        char time_end[23];
	int already_used;
        char chn_list[512];

        struct twindow *next;
};

struct twindow *get_twin_from_summary();
struct twindow *scan_summary_file();

/* ------------------------------------------------------------------------ */
void time_span_out ()
{
	int i;										/* counter */
	struct data_hdr temp_hdr;	/* data header */
	struct time end_time;		/* block ending time */
	unsigned int offset;		/* local computations */
	double duration;

	struct type52 *save_channel_ptr = current_channel;
	struct twindow *twin;

/*  
 * if output is intended for a seed volume, update the blockette info 74
 * only...
 *
 */
	if (Seed_flag)
	{
		update_type74(data_hdr);
	
		data_hdr->nsamples = 0;

		return;
	}
/*                +=======================================+                */
/*================|          Look for Time Spans          |================*/
/*                +=======================================+                */
	if (read_summary_flag)
	{
		char start[23];
		char end[23];
		struct time endtime;
		struct time starttime;

		duration = 
		   ((double)(data_hdr->nsamples)*TIME_PRECISION)/data_hdr->sample_rate;

		endtime = timeadd_double(data_hdr->time, duration);

		
#if 0

		sprintf(start, "%d,%d,%d:%d:%d.%d", 
					data_hdr->time.year,
					data_hdr->time.day,
					data_hdr->time.hour,
					data_hdr->time.minute,
					data_hdr->time.second,
					data_hdr->time.fracsec);

                sprintf(end, "%d,%d,%d:%d:%d.%d", 
                                        endtime.year,
                                        endtime.day, 
                                        endtime.hour, 
                                        endtime.minute,
                                        endtime.second, 
                                        endtime.fracsec); 

		if (input_file_type == DISK_DEVICE)
			/* if from disk drive, already scanned the
			 * block 74s for matching data, just use
			 * what was scanned previously
			 */
			twin = get_twin_from_summary();

		else
			
			/* tape drive seed volume, we must scan the
			 * summary file for "clipping"
			 */
			twin = scan_summary_file(data_hdr->station,
					  	 data_hdr->channel,
					  	 data_hdr->network,
						 data_hdr->location,
						 data_hdr->time,
						 endtime);
					  

		if (twin == NULL)
		{
			fprintf(stderr, "Error - time_span_out(): Unable to locate timespan in the summary file!\nStation: %s;Channel: %s\n for timespan %s to %s \n",
					data_hdr->station,
					data_hdr->channel,
					start, end);

			fprintf(stderr, "Ignoring summary file for this timespan.\n");

			output_data(data_hdr, 0);

			data_hdr->nsamples = 0;

			return;

		}

		timecvt(&starttime, twin->time_start);
		timecvt(&endtime, twin->time_end);
        
		start_time_point = &starttime;
		end_time_point = &endtime;

 
		start_time_count = end_time_count = 1;

#endif

		set_event(&(data_hdr->time), &endtime);
 
	}

	
	/* if user didn't enter any time spans, output all data */
	if (start_time_count == 0)
	{
		output_data(data_hdr, 0);
		data_hdr->nsamples = 0;

		return;
	}
	/* look through time span list as entered by the user */
	for (i=0;i<start_time_count;i++)
	{
		current_channel = save_channel_ptr;

		/* copy current data header */
		temp_hdr = *data_hdr;

		/* is span start equal to start of data? */
		if (timecmp (start_time_point[i],data_hdr->time) == 0)
		{
			/* is there a corresponding end time? */
			if (i < end_time_count)
			{
				/* use current data start time */
				/* calc number of samples */

				duration = timedif(temp_hdr.time,end_time_point[i]);

				/* took this out of timedif */
				if (duration < 0)
					duration = 0;

				temp_hdr.nsamples = (int) (((duration*temp_hdr.sample_rate)/TIME_PRECISION)+.999);

				/* make sure it's not more than available data */
				if (temp_hdr.nsamples > data_hdr->nsamples) temp_hdr.nsamples = data_hdr->nsamples;

				output_data(&temp_hdr, 0);

			}
			else 
			{
				output_data(data_hdr, 0);
			}
		}
		/* is span start less than start of data? */
		else if (timecmp (start_time_point[i],data_hdr->time) < 0)
		{
			/* is there a corresponding end time? */
			if (i < end_time_count)
			{
				if (timecmp (end_time_point[i],data_hdr->time) > 0)
				{
					/* use current data start time */
					/* calc number of samples */

					duration = timedif(temp_hdr.time,end_time_point[i]);

                                	/* took this out of timedif */
                                	if (duration < 0)
                                        	duration = 0;

					temp_hdr.nsamples = (int) (((duration*temp_hdr.sample_rate)/TIME_PRECISION)+.999);

					/* make sure it's not more than available data */
					if (end_time_point[i].year == 9999) temp_hdr.nsamples = data_hdr->nsamples;

					if (temp_hdr.nsamples > data_hdr->nsamples) temp_hdr.nsamples = data_hdr->nsamples;
					output_data(&temp_hdr, 0);
				}
			}
			else
			{
				output_data(data_hdr, 0);
			}
	
		}
		/* span start must be greater than start of data */
		else
		{
			/* figure out data ending time */
			if (data_hdr->sample_rate != 0)
				duration = (((double)data_hdr->nsamples * TIME_PRECISION) / data_hdr->sample_rate);

			else 
				duration = 0;

			end_time = timeadd_double (data_hdr->time, duration);

			/* is span start before end of data? */
			if (timecmp(start_time_point[i],end_time) < 0)
			{
				/* calc new start time and sample offset*/
				duration = timedif(temp_hdr.time,start_time_point[i]);

				/* took this out of timedif */
				if (duration < 0)
					duration = 0;

				offset = (int) ((duration * data_hdr->sample_rate)/TIME_PRECISION);

				/* changed to float to avoid overflow - CL */
				duration = 
					((float) offset * TIME_PRECISION) / 
						data_hdr->sample_rate;

				temp_hdr.time = timeadd_double (data_hdr->time, duration);
				/* calc number of samples */
				if (i < end_time_count)
				{
					if (end_time_point[i].year != 9999)
						duration = timedif(temp_hdr.time,end_time_point[i]);
					else
						duration = timedif(temp_hdr.time,end_time);

				}
				else 
					duration = timedif(temp_hdr.time,end_time);

				/* took this out of timedif */
				if (duration < 0)
					duration = 0;

				temp_hdr.nsamples = (int)(((duration*temp_hdr.sample_rate)/TIME_PRECISION)+.999);

				/* make sure it's not more than available data */
				if ((temp_hdr.nsamples+offset) > data_hdr->nsamples)
					temp_hdr.nsamples = data_hdr->nsamples - offset;

				output_data(&temp_hdr, offset);

			}
		}
	}

	data_hdr->nsamples = 0;

	if (read_summary_flag)
		/* will avoid unnecessary free()s as these weren't malloced */
                start_time_count = end_time_count = 0;
		
	
/*                +=======================================+                */
/*================|                clean-up               |================*/
/*                +=======================================+                */

}
