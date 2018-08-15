/*===========================================================================*/
/* SEED reader     |              times_data               |    subprocedure */
/*===========================================================================*/
/*
	Name:		times_data
	Purpose:	determine and print station, channel, start time, number of 
				samples, sample rate, and station/channel/time continutity flag
				for each data record on a SEED volume
	Usage:		void times_data ();
				times_data ();
	Input:		none
	Output:		none (prints information to stdout)
	Externals:	input.data - pointer to beginning of data block
	Warnings:	none
	Errors:		none
	Called by:	snoop
	Calls to:	none
	Algorithm:	determine whether this record is time/station/channel
				continuous with the previous record; print the record number,
				station name, channel name, start time of record, and the
				continuity flag
	Notes:		A continuity flag of 0 (FALSE) means that the record is 
					time/station/channel discontinuous with the previous record,
					while a flag of 1 (TRUE) indicates continuity.  
				A time/station/channel discontinuity means that a new
					seismogram has started.
				For ease of use with "grep", the symbol "<==" is appended to 
					lines indicating beginnings of seismograms.
	Problems:	none known
	References:	Halbert et al, 1988; see main routine
	Language:	C, hopefully ANSI standard
	Author:		Dennis O'Neill
	Revisions:	11/21/88  Dennis O'Neill  Initial version
				11/21/88  Dennis O'Neill  Production release 1.0
*/

#include "rdseed.h"

struct data_blk_1000 *scan_for_blk_1000();

long int total_samples;
/* long int start_record; */

/* added April 5, 1994 - CL  - problem realizing when the previous station/
 * channel had changed, then new time span by definition
 */
char prev_station[14]  = "XXXXX";  /* Must be big enough */
char prev_channel[14] =  "XXXXX";  /* Same here! */
char prev_location[14] = "XXXXX";

double sample_rate;
static struct time newtime, first_time;						/* start time of new seisgm */

void times_data (call_type)
int call_type;
{
int flag = 0;
	int i;										/* counter */
	char *input_data_ptr;		/* pointer to input data */
	struct input_data_hdr *input_data_hdr;		/* input data header */
	struct type50	*p_station;/* ptr to current station */
	struct type52	*p_channel;/* ptr to current channel */

	struct data_blk_1000 *p;

	int continuous;				/* continuity flag */
	int duration;				/* length of seismogram */
	int tempint;				/* temporary integer */
	short int temp_2byte;			/* temp for byte swapping */
	unsigned short int temp_u2byte;		/* temp for byte swapping */
	struct input_time temptime;		/* temp for byte swapping */

	char this_station[5+1];			/* name of station */
	char this_channel[3+1];			/* name of channel */
	char this_network[2+1];			/* name of channel */
	char this_location[3];
	char that_location[3];


	struct time effective_start, effective_end;	/* response effective times */

	struct time this_time;			/* start time of this seisgm */

	continuous = 0;

	/* figure out current ending time of previous seismogram */
	if (last_sample_rate != 0)
	{
		duration = (int)((double)(last_nsamples * 10000) / (double)last_sample_rate);
	}
	else 
	{
		duration = 0;
		last_time.year = 0;
		last_time.day = 0;
		last_time.hour = 0;
		last_time.minute = 0;
		last_time.second = 0;
		last_time.fracsec = 0;
	}

	newtime = timeadd (last_time, duration);

/*		timeprt(last_time);
		printf ("  l_samples %d, l_rate %f, dur %d  ", last_nsamples, (double) last_sample_rate, duration);
		timeprt(newtime); printf("\n"); */

	if (!call_type)
	{

		input_data_ptr = lrecord_ptr+8;

/*                 +=======================================+                 */
/*=================|   read the fixed part of data header  |=================*/
/*                 +=======================================+                 */

		/* recover the fixed data header from the input record */
		input_data_hdr = (struct input_data_hdr *) input_data_ptr;

/*                 +=======================================+                 */
/*=================| obtain particulars for this data blk  |=================*/
/*                 +=======================================+                 */

		/* recover station name */
		for (i = 0; i < 5; i++) this_station[i] = input_data_hdr->station[i];
			this_station[5] = '\0';
		for (i = 4; this_station[i] == ' '; i--) this_station[i] = '\0';

		/* recover channel name */
		for (i = 0; i < 3; i++) this_channel[i] = input_data_hdr->channel[i];
			this_channel[3] = '\0';
		for (i = 2; this_channel[i] == ' '; i--) this_channel[i] = '\0';

		/* recover network name */
		for (i = 0; i < 2; i++) 
			this_network[i] = input_data_hdr->network[i];

		this_network[2] = '\0';

		for (i = 1; this_network[i] == ' '; i--) 
			this_network[i] = '\0';

		/* recover location */
	        for (i = 0; i < 2; i++) 
	                this_location[i] = input_data_hdr->location[i];
 
	        this_location[2] = '\0';  

		if (this_location[1] == ' ')
			this_location[1] = 0;

		if (this_location[0] == ' ')
			this_location[0] = 0;
 
		current_station = NULL;

	        /* find the current station information in the tables */

		current_channel = NULL;

	        for (p_station = type50_head; p_station != NULL; p_station = p_station->next)
	                if (strcmp (this_station, p_station->station) == 0)
			{
				current_station = p_station;
        
				/* find the current channel within the current station */

		                for (p_channel = p_station->type52_head;
	         		       	p_channel != NULL; p_channel = p_channel->next)
				{
					strcpy(that_location, p_channel->location);

					trim(that_location);

	                		if ((strcmp(this_channel, p_channel->channel) == 0) &&
						    (strcmp(this_location, that_location) == 0))
					{
						current_channel = p_channel;

						break;

					}
				}

			}

              
	        if (current_station == NULL) /* station was not found */
	        {
	               	fprintf (stderr, "WARNING (times_data):  ");
	               	fprintf (stderr, "station %s not found in station table.\n", this_station);
	               	fprintf (stderr, "\tData Record Skipped.\n");
			return;
	        }

		if (current_channel == NULL) /* channel was not found */
	        {
	        	fprintf (stderr, "WARNING (times_data):  ");
	               	fprintf (stderr, "channel,loc= %s,%s<< ", this_channel, this_location);
	                fprintf (stderr, "station %s not found in station table.\n", this_station);
	                fprintf (stderr, "\tData Record Skipped.\n");

			return;

		}

   
		/* check if byteswapping will be needed to recover data */
		byteswap = 0;
 
       		/* check if byteswapping will be needed to recover data */
        	/* hopefully, rdseed will not be around by 3010 */
 
        	if (input_data_hdr->time.year < 1950 ||
                        	input_data_hdr->time.year > 3010)
        	{
			byteswap = 1;

                	input_data_hdr->time.year =
                        	swap_2byte(input_data_hdr->time.year);
 
                	/* check for sanity */
                	if (input_data_hdr->time.year < 1950 ||
                        		input_data_hdr->time.year > 3010)
                	{
                        	fprintf(stderr, "ERROR - times_data(): Unknown word order for station %s, channel %s\n",
                                        this_station,
                                        this_channel);
 
                        	fprintf(stderr, "Skipping data record.\n");
                        	return;
 
                	}
 

			input_data_hdr->time.day =
                        	swap_2byte(input_data_hdr->time.day);

                	input_data_hdr->time.fracsec =
                        	swap_2byte(input_data_hdr->time.fracsec);
 
                	input_data_hdr->nsamples =
                        	swap_2byte(input_data_hdr->nsamples);

                	input_data_hdr->sample_rate =
                        	swap_2byte(input_data_hdr->sample_rate);
 
                	input_data_hdr->sample_rate_multiplier =
                        	swap_2byte(input_data_hdr->sample_rate_multiplier);
 
                	input_data_hdr->number_time_corrections =
                        	swap_4byte(input_data_hdr->number_time_corrections);
 
                	input_data_hdr->bod =
                        	swap_2byte(input_data_hdr->bod);

                	input_data_hdr->bofb =
                        	swap_2byte(input_data_hdr->bofb);
 
                	if (input_data_hdr->bofb)
                        	blockette_swap(input_data_ptr +
                                                input_data_hdr->bofb - 8,
                                        		input_data_ptr - 8);
 
        	}
 

        p = 0;
 
        if (input_data_hdr->number_blockettes > 0)
        { 
		p = scan_for_blk_1000(input_data_ptr + input_data_hdr->bofb - 8,
						input_data_ptr - 8);
 
	} 

	/* get start time of this data block */
	temptime.year    = input_data_hdr->time.year;
	temptime.day     = input_data_hdr->time.day;
	temptime.hour    = input_data_hdr->time.hour;
	temptime.minute  = input_data_hdr->time.minute;
	temptime.second  = input_data_hdr->time.second;
	temptime.fracsec = input_data_hdr->time.fracsec;
	this_time.year    = (int) temptime.year;
	this_time.day     = (int) temptime.day;
	this_time.hour    = (int) temptime.hour;
	this_time.minute  = (int) temptime.minute;
	this_time.second  = (int) temptime.second;
	this_time.fracsec = (int) temptime.fracsec;

/* Do time correction */
	if ((input_data_hdr->activity_flags & 0x02) == 0)
	{

		this_time.fracsec += input_data_hdr->number_time_corrections;

		while ((this_time.fracsec >= 10000) || (this_time.fracsec < 0)) {
		if (this_time.fracsec >= 10000)
		{
			this_time.second += this_time.fracsec/10000;
			this_time.fracsec = this_time.fracsec%10000;
			if (this_time.second >= 60)
			{
				this_time.minute += this_time.second/60;
				this_time.second = this_time.second%60;
				if (this_time.minute >= 60)
				{
					this_time.hour += this_time.minute/60;
					this_time.minute = this_time.minute%60;
					if (this_time.hour >= 24)
					{
						this_time.day += this_time.hour/24;
						this_time.hour = this_time.hour%24;
						if (this_time.day > (isaleap(this_time.year) ? 366 : 365))
						{
							this_time.year += 1;
							this_time.day = 1;
						}
					}
				}
			}
		}
		else if (this_time.fracsec < 0)
		{
			this_time.second -= 1;
			this_time.fracsec += 10000;
			if (this_time.second < 0)
			{
				this_time.minute -= 1;
				this_time.second += 60;
				if (this_time.minute < 0)
				{
					this_time.hour -= 1;
					this_time.minute += 60;
					if (this_time.hour < 0)
					{
						this_time.day -= 1;
						this_time.hour += 24;
						if (this_time.day == 0)
						{
							this_time.year -= 1;
							this_time.day = (isaleap(this_time.year) ? 366 : 365);
						}
					}
				}
			}
		}
		}
/*		printf("%02d:%02d:%02d.%05d\n", this_time.hour, this_time.minute, this_time.second, this_time.fracsec);  */
	}


	current_station = NULL;
	current_channel = NULL;
        /* find the current station information in the tables */
        for (p_station = type50_head;
                p_station != NULL;
                p_station = p_station->next)
                if (strcmp (this_station, p_station->station) == 0)
			{
			timecvt(&effective_start, p_station->start);
			timecvt(&effective_end, p_station->end);
			if (p_station->end == NULL) effective_end.year = 9999;
			if ((timecmp(this_time, effective_start) >= 0) &&
				(timecmp(this_time, effective_end) <= 0))
					current_station = p_station;
        /* find the current channel within the current station */
	                for (p_channel = p_station->type52_head;
         		       	p_channel != NULL;
                		p_channel = p_channel->next)

                		if ((strcmp(this_channel, 
					   p_channel->channel) == 0) &&
					(strcmp(this_location, 
						p_channel->location) == 0))
					{
						timecvt(&effective_start, p_channel->start);
						timecvt(&effective_end, p_channel->end);
						if (p_channel->end == NULL) effective_end.year = 9999;
						if ((timecmp(this_time, effective_start) >= 0) &&
							(timecmp(this_time, effective_end) <= 0))
						{
							current_channel = p_channel;
							break;
						}
					}
			}
              
        	if (current_station == NULL) /* station was not found */
        	{
                	fprintf (stderr, "WARNING (process_data):  ");
                	fprintf (stderr, "station %s ",
                        this_station);
 						timeprt(this_time);
                	fprintf (stderr, " - response effective time not found.\n",
                        this_station);
        	       	fprintf (stderr, "\tData Record Skipped.\n");
			return;
        	}

		if (current_channel == NULL) /* channel was not found */
                {
                        fprintf (stderr, "WARNING (process_data):  ");
                        fprintf (stderr, "%s,%s ", this_station, this_channel);
						timeprt(this_time);
                        fprintf (stderr, " - response effective time not found.\n",
                                this_station);
                        fprintf (stderr, "\tData Record Skipped.\n");
			return;
                }

/*                 +=======================================+                 */
/*=================| accumulate various descriptive info   |=================*/
/*                 +=======================================+                 */


/*                 +=======================================+                 */
/*=================|          determine continuity         |=================*/
/*                 +=======================================+                 */

		/* determine whether this is station-, channel-, and time-continuous */
		/* with previous data block */
	/*	if (timeqc (last_time, this_time, last_nsamples, last_sample_rate) == 0) */

		if (last_sample_rate == 0)
			continuous = FALSE;
		else
		{
			if (timetol (newtime, this_time, last_nsamples, last_sample_rate) == 0)
				continuous = TRUE;
			else 
				continuous = FALSE;
		}

		if (!(strcmp(this_station, prev_station) == 0) && (strcmp(prev_station, "XXXXX") != 0))
			continuous = FALSE;

		if (!(strcmp(this_channel, prev_channel) == 0) && (strcmp(prev_channel, "XXXXX") != 0)) 
	        	continuous = FALSE;

		if ((strcmp(this_location, prev_location) != 0) && (strcmp(prev_location, "XXXXX") != 0))
			continuous = FALSE;

		strcpy(prev_station, this_station);
		strcpy(prev_channel, this_channel);
		strcpy(prev_location, this_location);


		/* figure out current display ending time of previous seismogram */
		if (last_sample_rate != 0)
		{
			duration = (int)((double)((last_nsamples-1) * 10000) / (double)last_sample_rate);
		}
		else 
		{
			duration = 0;
			last_time.year = 0;
			last_time.day = 0;
			last_time.hour = 0;
			last_time.minute = 0;
			last_time.second = 0;
			last_time.fracsec = 0;
		}

		newtime = timeadd (last_time, duration);

		/* save information for next time around (continuity check) */
		last_time.year    = (int) this_time.year;
		last_time.day     = (int) this_time.day;
		last_time.hour    = (int) this_time.hour;
		last_time.minute  = (int) this_time.minute;
		last_time.second  = (int) this_time.second;
		last_time.fracsec = (int) this_time.fracsec;
		temp_u2byte        = input_data_hdr->nsamples;

		last_nsamples     = (int) temp_u2byte;

		/* get sample rate */
		temp_2byte = input_data_hdr->sample_rate;
		last_sample_rate = (float) temp_2byte;

		temp_2byte = input_data_hdr->sample_rate_multiplier;
		tempint = (int) temp_2byte;

		if (last_sample_rate < 0) 
			last_sample_rate = 1/(-last_sample_rate);
		if (tempint > 0) 
			last_sample_rate = last_sample_rate * tempint;
		else if (tempint < 0) 
			last_sample_rate = last_sample_rate / (-tempint);

		parse_type100(input_data_ptr, &last_sample_rate);

	/*	printf ("%4d,%03d,%02d:%02d:%02d.%04d  %d\n", 
			this_time.year, this_time.day, this_time.hour, this_time.minute,
			this_time.second, this_time.fracsec, last_nsamples); */
	}
	else
	{
		continuous = FALSE;
		if (last_sample_rate != 0)
		{
			duration = (int)((double)((last_nsamples-1) * 10000) / (double)last_sample_rate);
		}
		else 
		{
			duration = 0;
			last_time.year = 0;
			last_time.day = 0;
			last_time.hour = 0;
			last_time.minute = 0;
			last_time.second = 0;
			last_time.fracsec = 0;
		}
		newtime = timeadd (last_time, duration);
	}

	if (continuous)
	{
		total_samples += (long int) last_nsamples;
	}
	else /* if (!(continuous) && (this_time.year != 0)) */
	{
		strcpy(prev_station, "XXXXX");  /* set for new time span */
		strcpy(prev_channel, "XXXXX");  /* Same here! */
		strcpy(prev_location, "XXXXX");

		if (start_record != 0)
		{

			if (sample_rate_count) sample_rate_accum = sample_rate_accum/sample_rate_count;
			else sample_rate_accum = sample_rate;

			fprintf (outputfile,"%4d,%03d,%02d:%02d:%02d.%04d ", 
				newtime.year, newtime.day, newtime.hour, newtime.minute,
				newtime.second, newtime.fracsec);

			fprintf (outputfile,"%11.7f ", sample_rate);
			fprintf (outputfile,"%6d", total_samples);
			fprintf (outputfile,"\n");

			sample_rate_accum = 0.0;
			sample_rate_count = 0;

		}

/*** Added by Sue to fix (I hope) incorrect display of record numbers ***/

		if (!call_type)
		{
			start_record = input.recordnumber;
			fprintf(outputfile,"%6ld ", start_record);
			fprintf(outputfile,"%-5s ", this_station);
			fprintf(outputfile,"%-3s ", this_channel);
			fprintf(outputfile,"%-2s ", this_network);
			fprintf(outputfile, " %-2.2s ", this_location);
			fprintf(outputfile, " %c ", *((char *)(lrecord_ptr+6)));

			fprintf (outputfile,"%4d,%03d,%02d:%02d:%02d.%04d ", 
				this_time.year, this_time.day, this_time.hour, this_time.minute,
				this_time.second, this_time.fracsec);

			total_samples = last_nsamples;
			start_record = input.recordnumber;
			sample_rate = last_sample_rate;
			first_time = this_time;
		}
	}

}

void dump_stn_chn_table()


{
        struct type50   *p_station;/* ptr to current station */
        struct type52   *p_channel;/* ptr to current channel */

	for (p_station = type50_head; p_station != NULL; p_station = p_station->next)
	{
		if (strcmp(p_station->station, "A11") != 0)
			continue;

		printf("station>>%s\n", p_station->station);

		for (p_channel = p_station->type52_head; p_channel != NULL; p_channel = p_channel->next)
			printf("\t\t chn,loc>>%s, %s<<\n", p_channel->channel, p_channel->location);

	}

	getchar();

}

