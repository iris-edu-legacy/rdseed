/*===========================================================================*/
/* SEED reader     |             process_data              |    subprocedure */
/*===========================================================================*/
/*
	Name:	process_data
	Purpose:recover seismic data from SEED format
	Usage:	void process_data ();
			process_data ();
	Input:	none (seismic data are taken from globally-available structure)
	Output:	none (a subprocedure writes seismogram files)
	Externals:input.data - the data portion of a SEED logical record
		current_station - pointer to current station info in tables
		current_channel - pointer to current channel info in tables
		data_hdr - SEED fixed-length data header
		last_time - ending time of previous record
		last_sample_rate - sample rate of previous record
		last_nsamples - number of samples in previous record
		seismic_data - the current seismogram
		seismic_data_ptr - a pointer into "seismic_data"
		decode - data decoder key (1st 5 letters of compression name)
		numitems - number of seismograms wanted
		item - list of starting record numbers of desired seismograms
		start_record - starting logical record number of this seismogram
	Warnings:station name not found in tables
			channel name not found in tables
			data decomression type not found in tables
			unknown data compression scheme encountered
	Errors:	none
	Called b:main program
	Calls to:output_data - writes out a seismic data file
			decode_asro - decode ASRO data
			decode_cdsn - decode CDSN data
			decode_dwwssn - decode DWWSSN data
			decode_sro - decode SRO data
			decode_steim - decode Steim-compressed data
			decode_32bit - decode UCSD 32-bit integer data
			timeadd - add two times (yyyy,ddd,hh:mm:ss.ffff)
			timecmp - compare two times (yyyy,ddd,hh:mm:ss.ffff)
			convert_seedhdr - construct minimum data header
	Algorithm:recover the station, channel, and start time from the current
		data block; figure out the ending time and sample rate.
		If the current data block is station-, channel-, and time-
		continuous with the previous data block, append the data
		to the current seismogram and update the minimum data header
		appropriately.  If the current data block is not continuous
		with the previous data block, write out the previous seismogram
		and start a new one using this block's data.  Checks are only
		performed if the current data block is part of a desired 
		seismogram.
	Notes:	none
	Problems:can't derive an uncompressor from the abbreviation dictionary
		description, but they may be faulty anyway (esp. CDSN)
	References:Halbert et al, 1988; see main routine
	Language:C, hopefully ANSI standard
	Author:	Dennis O'Neill
	Revisions:07/15/88  Dennis O'Neill  Initial preliminary release 0.9
		11/11/88  Dennis O'Neill  removed extra argument from a printf
		11/11/88  Dennis O'Neill  added check for possible divide-by-0
		                          condition in time-cntity check
		11/11/88  Dennis O'Neill  corrected neglected "-" sign in
		                          handling sample rate and multiplier
		11/11/88  Dennis O'Neill  installed smart byte-swapping
		11/15/88  Dennis O'Neill  changed handling of long seisgms
		11/16/88  Dennis O'Neill  moved "output", "output_overflow"
		                          vbl to outside of proc body
		11/21/88  Dennis O'Neill  Production release 1.0
		01/24/89  Dennis O'Neill  fixed some fprintfs
		02/06/89  Dennis O'Neill  added decode_32bit call
		02/10/89  Dennis O'Neill  fixed the statement
		data_hdr->number_time_corrections += (long int) swap_4byte;
		data_hdr->number_time_corrections += (long int) temp_4byte;
		05/01/92  Allen Nnace     fixed the statement
		data_hdr->number_time_corrections += (long int) temp_4byte; to
		data_hdr->number_time_corrections = (long int) temp_4byte;
		05/01/92  Allen Nance     display invalid data compression type
		05/05/92  Allen Nnace     change continuity check from
		(timetol (newtime, this_time, data_hdr->nsamples) == 0) to
		(timetol (newtime, this_time, last_nsamples) == 0)
		01/17/95  CL - changed decoding name from 5 bytes MAX to whatever

*/

/* #include <sunmath.h> */

#include "rdseed.h"

#define TIME_PRECISION 10000		/* number of divisions in a */
					/* second in SEED times */

int tspan_flag;		// used to only print errors and warnings once.

struct type50 *get_station_rec();
struct type52 *get_channel_rec();


void do_time_correction();
void dump_seismic_buffer();
char determine_orient_code();
char *code_to_english();
void swap_fsdh();
void swap_fsdh_back();
void blockette_swap_back();

extern struct mini_data_hdr mini_data_hdr;

extern int ignore_net_codes;	/* set in "main" -rdseed.c */
extern int read_summary_flag;	/* set in rdseed.c */
extern int strip_flag;		/* set in rdseed.c */
extern int mini_flag;		/* set in rdseed.c */



/* define these here so they keep their values between calls */
int output;			/* output-wanted flag */
int output_overflow;		/* for long seismograms */

char prev_quality = ' ';
char micro_sec = 0;

struct data_blk_1000 *p;

static char network_code[3];

static int chn_log;

/* ------------------------------------------------------------------------ */
struct data_blk_1000 *scan_for_blk_1000();
struct data_blk_2000 *scan_for_blk_2000();
char scan_for_blk_1001();
/* ------------------------------------------------------------------------ */

void process_data (int fsdh_swapped)

{
	char now_quality;

	int i;									
	char *input_data_ptr;			/* ptr to input data */
	struct input_data_hdr *input_data_hdr;	/* fixed data header */
	struct type52 *old_current_channel, 
				  *new_current_channel, 
				  *p_channel;	/* pointer save locations */

	struct type50 *old_current_station, 
				  *new_current_station, 
				  *p_station;		/* pointer save locations */

	int continuous;		/* stn,chnl,time cts flag */
	double duration;			/* time duration of record */
	struct time newtime;		/* new ending time of s'gm */
	int sample_rate_multiplier;	/* temp used in calc smpl rt */
	short int temp_2byte;		/* temp for byte-swapping */
	int temp_4byte;			/* temp for byte-swapping */
	unsigned short int temp_u2byte; /* temp for byte-swapping */
	struct input_time temptime;	/* temp for byte-swapping */
	int next_index;			/* next index into s'gm array */

	/* "this" => current data blk */
	char this_station[5+1];		/* name of this station */
	char this_location[2+1];	/* location code this station */
	char this_channel[3+1];		/* name of this channel */
	struct time this_time;		/* start time of this block */
	int this_nsamples;		/* number of samples this blk */
	double this_sample_rate;		/* temp used to store smpl rt */

	struct type30 *this_type30;	/* index into SEED tables */

	chn_log = FALSE;

/*                 +=======================================+                 */
/*=================|   read the fixed part of data header  |=================*/
/*                 +=======================================+                 */

	/* point to beginning of data */
	input_data_ptr = lrecord_ptr + 8;

	/* recover the fixed data header from the input record */
	input_data_hdr = (struct input_data_hdr *) input_data_ptr;

	/* check for D, Q, M, R and compare to q_flag */
	if (q_flag != 'E')
		switch (*(lrecord_ptr + 6))
		{
			case 'D' :
				if (q_flag != 'D')
					return;	
				break;
			case 'R' :
				if (q_flag != 'R')
					return;
				break;
			case 'Q' :
				if (q_flag != 'Q')
					return;

				break;
			
			case 'M' :
				if (q_flag != 'M')
					return;
				break;
			
			default:
				fprintf(stderr, "Bad Data Header Indicator byte. Byte=%c\n", *(lrecord_ptr + 7));

		}

	now_quality = *(lrecord_ptr + 6);

/*                 +=======================================+                 */
/*=================| obtain particulars for this data blk  |=================*/
/*                 +=======================================+                 */

	/* recover station name */
	for (i = 0; i < 5; i++) 
		this_station[i] = input_data_hdr->station[i];
	this_station[5] = '\0';

	for (i = 4; this_station[i] == ' '; i--) 
	{
		this_station[i] = '\0';
		if (i == 0)
			break;
	}

	/* recover location code */
	for (i = 0; i < 2; i++) 
		this_location[i] = input_data_hdr->location[i];
	this_location[2] = '\0';

	for (i = 1; this_location[i] == ' '; i--)
	{
		this_location[i] = '\0';
		if (i == 0) 
                        break; 

	}


	/* recover channel name */
	for (i = 0; i < 3; i++) 
		this_channel[i] = input_data_hdr->channel[i];
	this_channel[3] = '\0';

	for (i = 2; this_channel[i] == ' '; i--) 
	{
		this_channel[i] = '\0';
		if (i == 0) 
                        break; 

	}

	/* scan for a LOG/AT/SOH record */
	if (strcasecmp(this_channel, "LOG") == 0)
		chn_log = 1;

	/* recover the network code */
	if (type10.version >= 2.3)
	{
		char *p;

		memcpy(network_code, input_data_hdr->network, 2);
		network_code[3] = 0;
			
	}
	else 
		strcpy(network_code, "");

/*                 +=======================================+                 */
/*=================| find info about this station/channel  |=================*/
/*                 +=======================================+                 */

	old_current_channel = current_channel; /* old current channel pointer */
	old_current_station = current_station; /* old current channel pointer */

	if (chn_log)
	{
		current_station = get_station_rec(this_station, 
						network_code, NULL);

                if (current_station == NULL)
                {
                        fprintf(stderr, "WARNING (process_data):  ");
                        fprintf (stderr, 
				"station %s/%s not found in station for network %2.2s.\n", 
				this_station, 
				type10.version >= 2.3 ? network_code : "N/A");
 
                        fprintf (stderr, "\tData Record Skipped.\n");
 
                        /* toggle = FALSE; */
 
                        current_station = old_current_station;
                        current_channel = old_current_channel;
 
                        return;
 
                }

	}
	else
	{
		get_stn_chn_rec(this_station, 
				this_channel, 
				network_code, 
				this_location,
				NULL);

		if (tspan_flag)
		{

			if (current_station == NULL || current_channel == NULL)
			{
				fprintf(stderr, "WARNING (process_data):  ");

				fprintf (stderr, "station/channel %s/%s not found in station/channel tables for network %2.2s, location code:%2.2s.\n", this_station, this_channel, type10.version >= 2.3 ? network_code : "N/A", this_location);
 
        			fprintf (stderr, "\tData Record Skipped.\n");

                		/* toggle = FALSE; */

                		current_station = old_current_station;
				current_channel = old_current_channel;

				tspan_flag = FALSE;
 
                		return;

			}
		}

	}


   	/* check if byteswapping will be needed to recover data */
	/* hopefully, rdseed will not be around by 3010 */
	/* should have already been done in rdseed.c */

	if (input_data_hdr->time.year < 1950 || 
			input_data_hdr->time.year > 3010)
		{
			fsdh_swapped = TRUE;

			swap_fsdh(&input_data_ptr);
		}

	/* check for sanity */
	if (input_data_hdr->time.year < 1950 || 
		input_data_hdr->time.year > 3010)
	{
		fprintf(stderr, "ERROR - process_data(): Unknown word order for station %s, channel %s, network %s, location %s\n",
 				this_station, 
				this_channel, 	
 				network_code, 
				this_location);

		fprintf(stderr, "Skipping data record.\n");
		return;

	}

	/* recover this block's sample rate */
	this_sample_rate = input_data_hdr->sample_rate;

	sample_rate_multiplier = input_data_hdr->sample_rate_multiplier;

	if (this_sample_rate < 0) 
		this_sample_rate = 1 / (-this_sample_rate);

	if (sample_rate_multiplier > 0) 
		this_sample_rate = this_sample_rate * sample_rate_multiplier;
	else 
	if (sample_rate_multiplier < 0) 
		this_sample_rate = this_sample_rate / (-sample_rate_multiplier);

	parse_type100(input_data_ptr, &this_sample_rate);

	// printBlks(input_data_ptr);

	/* check for sanity */
	if (this_sample_rate < 0)
		return;

	if (this_sample_rate > 5000)
		return;

	this_time.second  = input_data_hdr->time.second;
	/* get start time of this data block */
	this_time.year = input_data_hdr->time.year;

	this_time.day = input_data_hdr->time.day;

	this_time.hour    = input_data_hdr->time.hour;
	this_time.minute  = input_data_hdr->time.minute;
	this_time.second  = input_data_hdr->time.second;

	this_time.fracsec = input_data_hdr->time.fracsec;

/* ***********

printf("\n>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<\n");

printf("input_data=%d,%d:%d:%d.%d\n", input_data_hdr->time.day, input_data_hdr->time.hour, 
	input_data_hdr->time.minute, input_data_hdr->time.second, input_data_hdr->time.fracsec);

********** */

	/* Do time correction  -  not for miniseed output as the fsdh activity flag 
	 * still indicates Not applied upon output
	 */

	if (!mini_flag && (input_data_hdr->activity_flags & 0x02) == 0)
	{
		this_time.fracsec += 
			input_data_hdr->number_time_corrections;

		do_time_correction(&this_time);
	}

// printf("block start time=%d,%d,%d:%d:%d.%d\n", this_time.year, this_time.day, this_time.hour, this_time.minute, this_time.second, this_time.fracsec);

	atc_correct(&this_time, this_station,
                                this_channel,
                                network_code,
                                this_location,
				this_sample_rate);

	current_channel = NULL;

        if (chn_log)
        {
                current_station = get_station_rec(this_station,
                                                network_code, &this_time);

                if (current_station == NULL)
                {
                
			fprintf(stderr, "WARNING (process_data):  ");
                	fprintf(stderr, "station %s response effective time not found in station table.\n", this_station);

                	fprintf(stderr, "\tTrying again ignoring effective times.\n");

                	current_station = get_station_rec(this_station,
                                        		  network_code,
                                        		  NULL);

                	if (current_station == NULL)
                	{
                        	fprintf(stderr, "ERROR - unable to locate any station record for station:%s, network:%s\n\nSkipping data record!\n",
                                                this_station,
                                                type10.version >= 2.3 ?
                                                        network_code:
                                                        "N/A");
 
                        	current_station = old_current_station;
 
                        	return;
                	}	/* current_station == NULL */
		}

        }
        else
        {   

                get_stn_chn_rec(this_station,
                                this_channel,
                                network_code, 
				this_location,
				&this_time);

                if (current_station == NULL || current_channel == NULL)
                {
			if (tspan_flag)
			{
	           	     	fprintf (stderr, "WARNING (process_data):  ");
        	        	fprintf (stderr, "station %s %s response effective time not found in station table.\n", this_station,this_channel);
 
                		fprintf (stderr, "\tTrying again ignoring effective times.\n");
			}
 
                	get_stn_chn_rec(this_station,
					  this_channel,
                               		  network_code,
					  this_location,
                               		  NULL);
 
                	if (current_station == NULL || current_channel == NULL)
                	{
	
				if (tspan_flag)
				{	
                      		  	fprintf(stderr, "ERROR - unable to locate any station/channel record for station:%s, channel: %s, network:%s\n\nSkipping data record!\n", 
					   this_station,
					   this_channel,
        	                           type10.version >= 2.3 ?
                	                   	network_code:
                        	               		"N/A");
 				}

                        	current_station = old_current_station;
				current_channel = old_current_channel;
 
				tspan_flag = FALSE;

                        	return;
 
                	}
			else
			{
				if (tspan_flag)
					fprintf(stderr, "Found stn/chn record.\n");
			}

			tspan_flag = FALSE;

                }
	} 

	if (input_data_hdr->number_blockettes > 0)
        {

		if (scan_for_blk_2000(input_data_ptr +
                                        input_data_hdr->bofb - 8,
					input_data_ptr - 8))
		{

			process_opaque(input_data_ptr, fsdh_swapped);

			return;
		}

	}

	p = 0;
	if (input_data_hdr->number_blockettes > 0)
	{

		p = scan_for_blk_1000(input_data_ptr + 
					/* points to blockettes */
					input_data_hdr->bofb - 8,	
					/* points to lrec base 00000D... */
					input_data_ptr - 8);  
	}

	if (p)
	{
		byteswap = find_wordorder(p);
	}
	else
	{
		byteswap = find_wordorder (NULL);
	}

	/* scan for ATC channels for this stn/chn */

	new_current_station = current_station;	/* new current channel */
	current_station = old_current_station;	/* restore current channel pointer */
	new_current_channel = current_channel;	/* new current channel */
	current_channel = old_current_channel;	/* restore current channel pointer */

	
/*                 +=======================================+                 */
/*=================| Is block within selected time spans   |=================*/
/*                 +=======================================+                 */

	/* if it is a LOG record, check the start time, fake an end time
	 * of infinity
	 */
	if (chn_log)
	{
		struct time fake_time;

		memset((char *)&fake_time, 0, sizeof(fake_time));

		if (!chk_time(this_time, fake_time))
			return;

		if (data_hdr->nsamples > 0)
			dump_seismic_buffer();

		if (Seed_flag)
        	{
                	struct data_hdr d_hdr; 
         
			memset((char *)&d_hdr, 0, sizeof(d_hdr));
 
                	d_hdr.station = this_station; 
                	d_hdr.channel = this_channel; 
			d_hdr.location = this_location;

                	strncpy(d_hdr.network, 
				 network_code, strlen(network_code));
 
                	d_hdr.time = this_time;

			d_hdr.bod = 48;

			if (fsdh_swapped)
			{
				input_data_ptr = lrecord_ptr + 8;
				swap_fsdh_back(&input_data_ptr);
			}
 
                	output_seed_data_file(input_data_ptr); 
 
                	update_type74(&d_hdr);
        
        	}
		else
		dump_LOG_rec(input_data_ptr + (input_data_hdr->bod - 8), 
				LRECL - input_data_hdr->bod,
				&this_time,
				this_station,
				this_channel,
				network_code,
				this_location);

		return;

	}


	this_nsamples = input_data_hdr->nsamples;

	/* if number of samples is zero, do nothing unless we are
	 * outputing miniseed or seed
	 */

	if ((this_nsamples == 0) && ((!mini_flag) && (!Seed_flag)))
		return;

	if ((this_nsamples == 0) && (mini_flag || Seed_flag))
		if (strip_flag)
			return;

	/* figure out current block ending time */
    	if (this_sample_rate != 0)
        	duration = ((double)this_nsamples*TIME_PRECISION)/this_sample_rate;
    	else duration = 0;
 
    	newtime = timeadd_double(this_time, duration);

// printf("block end time =%d,%d,%d:%d:%d.%d\n", newtime.year, newtime.day, newtime.hour, newtime.minute, newtime.second, newtime.fracsec);

    	/* if block time span is not overlapping a selected span,
     	 *  forget this block
     	*/ 

	/* if times needed are from summary file, match up the 
	 * stn, chn with times
	 */

	if (!read_summary_flag)
	{
		/* else use the normal entry (from keyboard) */
    		if (!chk_time (this_time, newtime))
		{
        		return;
		}
	}

#if 0

	else
	{
		// if (!chk_summary(this_time, newtime))
		//	return;
	}
#endif


/*                 +=======================================+                 */
/*=================| accumulate various descriptive info   |=================*/
/*                 +=======================================+                 */

	/* figure out current ending time of seismogram */
	if (last_sample_rate != 0)
		duration = (((double)last_nsamples * TIME_PRECISION) / last_sample_rate);

	else duration = 0;

	newtime = timeadd_double(last_time, duration);



/*                 +=======================================+                 */
/*=================|          determine continuity         |=================*/
/*                 +=======================================+                 */

	/* determine whether this is station-, channel-, and time-continuous */
	/* with previous data block */

	/* if no samples and we are this far, user is asking for mini or
	 * seed file output. Don't clip on zero sample record
	 */

/* **********

printf("checking time %d:%d:%d.%d\n", data_hdr->time.hour, data_hdr->time.minute, data_hdr->time.second, data_hdr->time.fracsec);

printf("last time =%d,%d:%d:%d.%d\n", last_time.day, last_time.hour, last_time.minute, last_time.second, last_time.fracsec);

printf("last time end time =%d,%d:%d:%d.%d\n", newtime.day, newtime.hour, newtime.minute, newtime.second, newtime.fracsec);

printf("this time = %d,%d:%d:%d.%d\n", this_time.day, this_time.hour, this_time.minute, this_time.second, this_time.fracsec);
 ********* */

// printf("  last time = %d:%d:%d.%d\n", newtime.hour, newtime.minute, newtime.second, newtime.fracsec);

	if (this_nsamples == 0)
		continuous = TRUE;
	else
		if ((strcmp (data_hdr->station, this_station) == 0) &&
			(strcmp (data_hdr->channel, this_channel) == 0) &&
			(strcmp (data_hdr->location, this_location) == 0) &&
			// (now_quality == prev_quality) &&
			(timetol (newtime, this_time, last_nsamples, this_sample_rate) == 0))
		{
			continuous = TRUE;
		}
		else 
		{
/* *********

printf("*************\n\tprev_time=%d,%d:%d:%d.%d\n", newtime.day, newtime.hour, newtime.minute, newtime.second, newtime.fracsec);

printf("\tthis time=%d,%d:%d:%d.%d\n***************\n", this_time.day, this_time.hour, this_time.minute, this_time.second, this_time.fracsec);
********** */

			continuous = FALSE;
			output_overflow = FALSE;
		}

	/*
	printf ("%d\n", continuous);
	*/

	/* save information for next time around */
	last_time.year    = (int) this_time.year;
	last_time.day     = (int) this_time.day;
	last_time.hour    = (int) this_time.hour;
	last_time.minute  = (int) this_time.minute;
	last_time.second  = (int) this_time.second;
	last_time.fracsec = (int) this_time.fracsec;
	last_nsamples     = this_nsamples;
	last_sample_rate  = this_sample_rate;

// printf("last_time=%d,%d,%d:%d:%d\n", last_time.year, last_time.day, last_time.hour, last_time.minute, last_time.second);

	/*
	printf ("%f ", last_sample_rate);
	*/


/*                 +=======================================+                 */
/*=================|if continuous, append to current seisgm|=================*/
/*                 +=======================================+                 */

	/* DEBUG */
	
	/*if (continuous)
		fprintf (stderr, "%d: %d + %d = %d < %d\n",
		input.recordnumber, data_hdr->nsamples, this_nsamples,
		data_hdr->nsamples + this_nsamples, MAX_DATA_LENGTH);
*/

	if (continuous)
	{
		/* check size of data; if length of seismogram is too large,
		 * fake a station/component/time discontinuity
		 * and add the current recordnumber to the desired seismogram list,
		 * which will cause (partial) seismogram files to be written */

		if (data_hdr->num_mux_chan != 0)
		{
			if ((data_hdr->nsamples + this_nsamples) > (seis_buffer_length)/data_hdr->num_mux_chan)
			{
				continuous = FALSE;
				output_overflow = TRUE;
				fprintf (stderr, "WARNING (process_data):  ");
				fprintf (stderr, "too much data (burp!).\n");
				fprintf (stderr, 
					"\tMaximum data length exceeded for station %s, channel %s.\n",
						this_station, this_channel);
				fprintf (stderr, "\tDue to indigestion, ");
				fprintf (stderr, "this seismogram will be split into sections.\n");
				fprintf (stderr, "\tTry increasing the Sample Buffer Length\n");
			}
	
			else 
			{
				output_overflow = FALSE;
				next_index = data_hdr->nsamples;

				/* update certain values in data_hdr */
				data_hdr->nsamples           += this_nsamples;
				data_hdr->activity_flags     |= input_data_hdr->activity_flags;
				data_hdr->io_flags           |= input_data_hdr->io_flags;
				data_hdr->data_quality_flags |= input_data_hdr->data_quality_flags;

				data_hdr->number_time_corrections += input_data_hdr->number_time_corrections;

				data_hdr->bod = input_data_hdr->bod;
				data_hdr->bofb = input_data_hdr->bofb;
			}
		}

	}

/*                 +=======================================+                 */
/*=================| if not continuous, write out seisgm   |=================*/
/*                 +=======================================+                 */

	if ((!continuous) && (output) && (data_hdr->time.year != 0))
	{
		dump_seismic_buffer();    
	}

	/* data_hdr->time.year !=0 avoids spurious output at beginning of tape */

	current_channel = new_current_channel;
	current_station = new_current_station;


/*                 +=======================================+                 */
/*=================|  if not continuous, init new seisgm   |=================*/
/*                 +=======================================+                 */

	/* initialize for new seismogram */
	if (!continuous)
	{
		/* initialize for new seismogram */
		data_hdr->nsamples = 0;
		next_index = 0;
		seismic_data_ptr = seismic_data;
		start_record = input.recordnumber;

		/* find out if this seismogram is wanted */
		output = TRUE;

		prev_quality = now_quality;

		if (input_data_hdr->number_blockettes > 0)
							/* points to blockettes */
			micro_sec = scan_for_blk_1001(input_data_ptr + input_data_hdr->bofb - 8,  
							/* points to lrec base 00000D... */
							input_data_ptr - 8); 

/*                 +=======================================+                 */
/*=================| build minimal header from known info  |=================*/
/*                 +=======================================+                 */

		/* build minimal data header from SEED fixed section of data header */
		convert_seedhdr (input_data_hdr);
		data_hdr->sample_rate = this_sample_rate;

 /* Added 4-30-92 (WAN) so that data_hdr has corrected start time */
		data_hdr->time.year    = this_time.year;
		data_hdr->time.day     = this_time.day;
		data_hdr->time.hour    = this_time.hour;
		data_hdr->time.minute  = this_time.minute;
		data_hdr->time.second  = this_time.second;
		data_hdr->time.fracsec = this_time.fracsec;

		memset((char *)&mini_data_hdr, 0, sizeof(struct mini_data_hdr));

		memcpy((char *)&mini_data_hdr.hdr, 
				(char *)(lrecord_ptr + 8), 
					sizeof(mini_data_hdr.hdr));

		mini_data_hdr.hdr.time.year		= data_hdr->time.year;
		mini_data_hdr.hdr.time.day		= data_hdr->time.day;
		mini_data_hdr.hdr.time.hour 	= data_hdr->time.hour;
		mini_data_hdr.hdr.time.minute 	= data_hdr->time.minute;
		mini_data_hdr.hdr.time.second 	= data_hdr->time.second; 
		mini_data_hdr.hdr.time.fracsec  = data_hdr->time.fracsec;

		if (mini_data_hdr.hdr.bofb)	
		{
			if (mini_data_hdr.hdr.bod == 0)
				/* move optional blockettes into buffer for later output */
				memcpy(mini_data_hdr.blockettes, lrecord_ptr + mini_data_hdr.hdr.bofb,
						256 - mini_data_hdr.hdr.bofb);
			else
				if (mini_data_hdr.hdr.bod < 256)
					/* move optional blockettes into buffer for later output */
					memcpy(mini_data_hdr.blockettes, lrecord_ptr + mini_data_hdr.hdr.bofb,
							mini_data_hdr.hdr.bod - mini_data_hdr.hdr.bofb);

		}

		data_hdr->num_mux_chan = 1;
		for (i=0;i<16;i++) data_hdr->mux_chan_name[i] = '1'+ i;


		/* find the current channel within the current station */
		if (current_station != NULL)
		{
			int n = 3;

			for (new_current_channel = current_station->type52_head;
				new_current_channel != NULL;
				new_current_channel = new_current_channel->next)
			{
				i = new_current_channel->subchannel - 1; 
				if ((i<0) || (i>9)) 
					i = 0;
				else /* multiplexed data */
					n = 2;

				if (strncmp(this_channel, new_current_channel->channel, n) == 0)
				{
					data_hdr->mux_chan_name[i] =
						determine_orient_code(new_current_channel);
					i++;
				}
			}
		} 
	}

/*                 +=======================================+                 */
/*=================|       read and decode the data        |=================*/
/*                 +=======================================+                 */

	if (output)
	{
/*                 +=======================================+                 */
/*=================| find compression/decompression format |=================*/
/*                 +=======================================+                 */

		/* find compression format */

		/* check for the presense fo a blockette 1000, 
		 * use the data type if found 
		 */
	
		strcpy(decode, "");
	
		if (data_hdr->bofb)
		{
			struct data_blk_1000 *p;

			/* since the input_data_hdr struct doesn't include the	
			 * 1st 8 bytes, whereas the bofb does, subtract out 
			 */

            		p = scan_for_blk_1000((char *)input_data_hdr + 
					      	 input_data_hdr->bofb - 8, 
							input_data_ptr - 8);


			if (p)
			{
				strcpy(decode, code_to_english(p->encoding_fmt));
			}

		}

		if (strlen(decode) == 0)
		{
			strncpy (decode, "     ", 5);
			for (this_type30 = type30_head; 
				this_type30 != NULL; this_type30 = this_type30->next)
				if (this_type30->code == current_channel->format_code) break;
			if (this_type30 != NULL)
			{
				strcpy (decode, this_type30->name);
			}
		}
		else
		{
			/* must still cycle to the proper type30 for
			 * geoscopian data
			 */
			for (this_type30 = type30_head;
                                this_type30 != NULL; this_type30 = this_type30->next)
                                	if (this_type30->code == current_channel->format_code) 
						break;
		}

		if (this_type30 == NULL)
		{
			fprintf (stderr, "WARNING (process_data):  ");
			fprintf (stderr, "data format description %d ",
			current_channel->format_code);
			fprintf (stderr, "not found in data format table.\n");
			fprintf (stderr, "\tData block Skipped.\n");
			return;
		}
	

		/* decode the data */
		rdseed_strupr(decode);	    /* make all upper case comparisons */

		if (Seed_flag)
		{
			if (fsdh_swapped)
			{
				input_data_ptr = lrecord_ptr + 8;

			 	swap_fsdh_back(&input_data_ptr);

			}
 
			output_seed_data_file(lrecord_ptr + 8);

			return;

		}

		/* point to first sample of this data block */
		input_data_ptr = lrecord_ptr + input_data_hdr->bod;

		if (strcmp(decode, "") == 0)
			return;

		if (strstr(decode, "16-BIT"))
			decode_16bit(input_data_ptr, this_nsamples, next_index);
		else if (strstr(decode, "ASRO"))
			decode_asro(input_data_ptr, this_nsamples, next_index);
		else if (strstr(decode, "CDSN"))
			decode_cdsn(input_data_ptr, this_nsamples, next_index);
		else if (strstr(decode, "GRAEF"))
			decode_graef(input_data_ptr, this_nsamples, next_index);
		else if (strstr(decode, "DWWSS"))
			decode_dwwssn(input_data_ptr, this_nsamples, next_index);
		else if (strstr(decode, "STEIM2")) 
			decode_steim2(input_data_ptr, this_nsamples, next_index);
		else if (strstr(decode, "STEIM")) /* MUST come after STEIM2 */
			decode_steim(input_data_ptr, this_nsamples, next_index);
		else if (strstr(decode, "SRO G"))
			decode_sro(input_data_ptr, this_nsamples, next_index);
		else if (strstr(decode, "ECHY"))
			decode_echery(input_data_ptr, this_nsamples, next_index);
		else if (strstr(decode, "RSTN"))
			decode_rstn(input_data_ptr, this_nsamples, next_index);
		else if (strstr(decode, "32-BI"))
			decode_32bit(input_data_ptr, this_nsamples, next_index);
		else if (strstr(decode, "SUN I"))
			decode_ieeefloat(input_data_ptr, this_nsamples, next_index);
		else if (strstr(decode, "SUN D"))
			decode_ieeedouble(input_data_ptr, this_nsamples, next_index);
		else if (strstr(decode, "GEOSC"))
			decode_geoscope (input_data_ptr, 
					 this_nsamples, 
					 next_index, 
					 this_type30->name);
		else 
		if (strstr(decode, "USNSN"))
			decode_usnsn(input_data_ptr, input_data_hdr);
		else
			decode_ddl(input_data_ptr, this_nsamples, next_index, this_type30);

	}

/*                 +=======================================+                 */
/*=================|              clean up                 |=================*/
/*                 +=======================================+                 */

}


/*=======================================================================*/
/* SEED reader     |             chk_time              |    process_data */
/*=======================================================================*/
int chk_time(blk_start, blk_end)
struct time blk_start;
struct time blk_end;
{
	int i;

	/* if no start times, accept all blocks */
	if ((start_time_count == 0) && (end_time_count == 0)) 
		return(1);

	/* for each user entered time span */
	for (i=0;i<start_time_count;i++)
	{
		/* is start of block within a time span */
		if (timecmp (blk_start, start_time_point[i]) == 0) 
			return(1);

		if (timecmp (blk_start, start_time_point[i]) > 0)
		{
			if (i+1 > end_time_count) 
				return(1);
			else 
			if (timecmp(blk_start, end_time_point[i]) <= 0) 
				return(1);
		}
		/* is end of this block within a time span */
		else
		{
			if (timecmp (blk_end, start_time_point[i]) > 0) 
				return(1);
		}
	}

	/* no overlapping time spans found */

	return(0);
}

/* ------------------------------------------------------------------------ */
#define WITHIN(t, s, e) ((timecmp(t, s) >= 0) && (timecmp(t, e) <= 0))
	
/* ------------------------------------------------------------------------ */
struct type50 *get_station_rec(s, n, rec_time)
char *s;
char *n;
struct time *rec_time;

{
	struct type50 *p, *p2 = NULL;
	
	struct time start, end; 	/* response effective times */

	char stn[10];  			/* if we ever get more than 10 
					 * for station and network sizes,
					 * we need to upgrade 
					 */
	char net[10];

	strncpy(stn, s, 10);
	strncpy(net, n, 10);

	trim(stn);
	trim(net);

	for (p = type50_head; p != NULL;p = p->next)
	{

		if (strcmp(stn, p->station) != 0) 
			continue;

		if ((type10.version >= 2.3) && (!ignore_net_codes))  
			if (strcmp(net, p->network_code) != 0)
				continue;

		timecvt(&start, p->start);
		timecvt(&end, p->end);

		if (p->end == NULL) 
			end.year = 9999;

		/* if no time is given, just use the one with the channel recs attached */
		if (rec_time == NULL)
		{
			if (p->type52_head != NULL)
				p2 = p;
		}
		else	
			/* else, do time check */
			if (WITHIN((*rec_time), start, end))	
			{
				/* make check for channel records there */
				if (p->type52_head != NULL) 
					p2 = p;

			}
	}

	return p2;

}
/* ------------------------------------------------------------------------ */
struct type50 *find_station_rec(s, n, beg_time, end_time)
char *s, *n;
struct time *beg_time;
struct time *end_time;
 
{
    	struct type50 *p, *p2 = NULL;
    
    	struct time start, end;        /* response effective times */

        char stn[10];                   /* if we ever get more than 10
                                         * for station and network sizes,
                                         * we need to upgrade
                                         */
        char net[10];

        strncpy(stn, s, 10);
        strncpy(net, n, 10);

        trim(stn);
        trim(net);
 
	if (end_time->year == 0)
		end_time->year = 9999;		/* I will be retired by then */
 
    	for (p = type50_head; p != NULL;p = p->next)
    	{
		if (strcmp(p->station, stn) != 0)
			continue;

		if (type10.version >= 2.3)
		{
			/* check to make sure there is network code,
			 * as blank network code on disk, turns out as a NULL string
			 */
			if (p->network_code && (!ignore_net_codes))
				if (strcmp( p->network_code, net) != 0)
					continue;
		}

		/* convert station time (YYYY,DDD,....) to struct */
		timecvt(&start, p->start);
		timecvt(&end, p->end);

		if (p->end == 0)
			end.year = 9999;

		/* do time check */
		if ((WITHIN((*beg_time), start, end)) && 
			(WITHIN((*end_time), start, end)))
				p2 = p;
    }
 
    return p2;
 
}
/* ------------------------------------------------------------------------ */
struct type52 *get_channel_rec(s, channel, l, rec_time) 
struct type50 *s; 
char *channel;
char *l;
struct time *rec_time;
 
{ 
	struct time start, end;
	struct type52 *c, *p = NULL;

	char station[10], network[3], loc[3];
	int finished = 0;
	
	int n = 3;

	/*## kludge alert, if multiplexed data, just look at 
	 * the 1st two chars of channel - return after finding 
	 * the 1st matching record - the output*.c routines will
	 * loop through the next two, adding the last char
	 */

	n = strlen(channel);

	strcpy(loc, l);

	/* save original station/network so we know when to
	 * stop scanning station list 
	 */
	strcpy(station, s->station);
	if (type10.version >= 2.3)
		strcpy(network, s->network_code);
	
	do 
	{
		for (c = s->type52_head; c != NULL; c = c->next)
			if ((strncmp (channel, c->channel, n) == 0) && 
 				strcmp(loc, c->location) == 0)
		{
			timecvt(&start, c->start);
			timecvt(&end,   c->end);

			if (c->end == NULL) 
				end.year = 9999;

			if (rec_time != NULL)
			{
				if (WITHIN((*rec_time), start, end))
				{
					p = c;

					if (c->subchannel > 0)
						return(p);
				}
			}
			else
			{
				p = c;
				
				if (c->subchannel > 0) 
                                	return(p);
			}

		}

		/* got here without finding channel record, check
		 * the next station record, could be there
		 */
		s = s->next;

		finished = s == 0;

	} while (!finished && ((strcmp(s->station, station) == 0) &&
		  	(type10.version >= 2.3 ? 
				(strcmp(s->network_code, network) == 0) :
					1)));

	return p;

} 

/* ------------------------------------------------------------------- */
/* actually sets the global variable current_station and current_channel
 * to point to the right station and channel
 */

int get_stn_chn_rec(s, c, n, l, t)
char *s, *c, *l, *n;
struct time *t;            /* station, channel, location, net, time */
 
{

	struct type50 *p, *p2 = NULL;
	struct type52 *c_rec = NULL, *crec2 = NULL;


        struct time start, end;        /* response effective times */

        char stn[10];                   /* if we ever get more than 10
                                         * for station and network sizes,
                                         * we need to upgrade
                                         */
        char net[10];

        strncpy(stn, s, 10);
        strncpy(net, n, 10);

        trim(stn);
        trim(net);


	/* check to see if a time epoch was requested */
	if (t == NULL)
	{
		/* spin until you find a station record */
		for (p = type50_head; p != NULL;p = p->next)
		{
			if (strcmp(stn, p->station) != 0) 
				continue;

			if ((type10.version >= 2.3) && (!ignore_net_codes))  
			{
				if (strcmp(net, p->network_code) != 0)
					continue;
			}

			p2 = p; /* remember station rec */

			/* spin until you find any channel record */
			if (p->type52_head != NULL)
                        {
                                /* check channel */
                                c_rec = get_channel_rec(p, c, l, NULL);
 
                                if (c_rec != NULL)
                                        crec2 = c_rec; /* remember channel */
                        }
			
		}	/* for every station */

		current_station = p2;
		current_channel = crec2;

		return 1;

	}		/* if not time is requested */


	/* else scan based on time */

	for (p = type50_head; p != NULL;p = p->next)
	{
		if (strcmp(stn, p->station) != 0) 
			continue;

		if ((type10.version >= 2.3) && (!ignore_net_codes))  
			if (strcmp(net, p->network_code) != 0)
				continue;

		timecvt(&start, p->start);
		timecvt(&end, p->end);

		if (p->end == NULL) 
			end.year = 9999;

		if (WITHIN((*t), start, end))	
			p2 = p; /* remember station rec */                     

		/* make check for channel records -
		 * it is very possible that the chn records have
		 * been tagged onto a stn rec outside of the 
		 * time bounds
		 */

		if (p->type52_head != NULL) 
		{

			/* check channel */ 
			c_rec = get_channel_rec(p, c, l, t);
 
			if (c_rec != NULL) 
				crec2 = c_rec; /* ditto channel */
		}
	
	}

	current_station = p2;
	current_channel = crec2;

	return 1;

}
/*--------------------------------------------------------------------- */
int get_stn_chn_Lrecl(s, c, n, l, t)
char *s, *c, *n, *l, *t; 		/* station, channel, net, time */

{
	struct time rec_time;
 
	timecvt(&rec_time, t);
	
	get_stn_chn_rec(s, c, n, l, &rec_time);

	if ((current_station == NULL) || (current_channel == NULL)) 
        {

		if (tspan_flag)
		{

	                fprintf (stderr, "WARNING (get_stn_chn_rec_Lrecl()):  ");
        	        fprintf (stderr, "station/channel %s/%s not found in station/channel tables.\n", s, c);
 
	                fprintf (stderr, "\tTrying again ignoring effective times.\n");
		}

		get_stn_chn_rec(s, c, n, l, NULL);

		if ((current_station == NULL) || (current_channel == NULL)) 
		{
			if (tspan_flag)
			{       

				fprintf (stderr, "WARNING (get_stn_chn_Lrecl()):  ");
				fprintf (stderr, "station/channel %s/%s not found in station table.\n", s, c);  

				fprintf(stderr, "Unable to determine the logical record length for station/channel %s/%s for location:\nDefaulting to 4096\n", s, c, l);

			}

			tspan_flag = FALSE;
	
			return 4096;

		}
		else
			fprintf(stderr, "Found stn/chn record\n");

		tspan_flag = FALSE;

	}

	return 2 << (current_channel->log2drecl - 1);	

}

/* ------------------------------------------------------------------ */

int dump_station_effective(s, n)
char *s;        /* station name */
char *n;        /* network */
 
{


    struct type50 *p = NULL;

    for (p = type50_head; p != NULL;p = p->next)
    {
        if ((strcmp(s, p->station) == 0) &&
            (type10.version >= 2.3 ? (strcmp(n, p->network_code) == 0) : 1))
        {
            fprintf(stderr, "\tStation %s, network %s, start/stop times:\n\t\t %s / %s\n",    s, type10.version >= 2.3 ? n : "N/A", p->start, p->end); 
 
 
        }
    }   
 
}

/* ------------------------------------------------------------------------ */

void do_time_correction(t)
struct time *t;

{

 
	while ((t->fracsec >= 10000) || (t->fracsec < 0)) 
	{
		if (t->fracsec >= 10000)
		{
			t->second += t->fracsec/10000;
			t->fracsec = t->fracsec%10000;

			if (t->second >= 60)
            		{

		                t->minute += t->second/60;
		                t->second = t->second%60;

                		if (t->minute >= 60)
		                {
					t->hour += t->minute/60;
					t->minute = t->minute%60;
					
					if (t->hour >= 24)
					{
						t->day += t->hour/24;
						t->hour = t->hour%24;

						if (t->day > (isaleap(t->year) ? 366 : 365))
						{
							t->year += 1;
							t->day = 1;
                        			}
                    			}
				}
			}
		}
		else if (t->fracsec < 0)
        	{
			t->second -= 1;
			t->fracsec += 10000;

			if (t->second < 0)
			{
				t->minute -= 1;
				t->second += 60;

				if (t->minute < 0)
				{
					t->hour -= 1;
					t->minute += 60;
				
					if (t->hour < 0)
					{
						t->day -= 1;
						t->hour += 24;

						if (t->day == 0)
						{
							t->year -= 1;

							t->day = (isaleap(t->year) ? 366 : 365);
						}
					}
				}
            		}
        	}
	}
}


/* ------------------------------------------------------------------------- */

void dump_seismic_buffer()

{

	/* make sure pointers are aligned with data that is in the data rec */

	get_stn_chn_rec(data_hdr->station,
			data_hdr->channel,
			data_hdr->network,
			data_hdr->location,
			&(data_hdr->time));

	if (current_station == NULL || current_channel == NULL)
		get_stn_chn_rec(data_hdr->station,
                        data_hdr->channel, 
                        data_hdr->network, 
			data_hdr->location,
                        NULL);


	if (current_station == NULL || current_channel == NULL) 
	{ 
		fprintf (stderr, "WARNING (process_data):  "); 

		fprintf (stderr, "station/channel %s/%s not found in station/channel tables for location: %s.\n", 
					data_hdr->station, 
					data_hdr->channel, 
					data_hdr->location);

       		fprintf (stderr, "\tSkipping this trace.\n"); 

		return;
	}

	time_span_out();

}

/* ------------------------------------------------------------------------ */
char determine_orient_code(chan)
struct type52 *chan;

{

	if ((ABS(ABS(chan->dip)-90.0) < 2.0) && 
		(ABS(chan->azimuth) < 2.0)) 
		 return 'Z';                             

	if ((ABS(chan->dip) < 2.0) &&
         ((ABS(chan->azimuth-180.0) < 10.0) ||
	  (ABS(chan->azimuth) < 2.0)))
		return 'N'; 

	if ((ABS(chan->dip) < 10.0) &&
		((ABS(chan->azimuth-90.0) < 2.0) ||
	 (ABS(chan->azimuth-270.0) < 2.0)))
                return 'E'; 

	if ((ABS(ABS(chan->dip)-60.0) < 2.0) && 
	   ((ABS(chan->azimuth-0.0) < 2.0) ||
          (ABS(chan->azimuth-180.0) < 2.0)))
		return 'A'; 

	if ((ABS(ABS(chan->dip)-60.0) < 2.0) && 
	    ((ABS(chan->azimuth-120.0) < 2.0) ||
	    (ABS(chan->azimuth-300.0) < 2.0)))
		return 'B';

	if ((ABS(ABS(chan->dip)-60.0) < 2.0) &&
	 ((ABS(chan->azimuth-240.0) < 2.0) ||
	  (ABS(chan->azimuth-60.0) < 2.0)))
		return 'C';

	/* if got here flag warning - use subchannel id */
	fprintf(stderr, "Warning... Azimuth and Dip out of Range on %s,%s\n", 
			current_station->station, chan->channel);

	fprintf(stderr, "Defaulting to subchannel identifier (for multiplexed data only)\n");

	return chan->subchannel + 48;   /* int + '0' */

 
}                  

/* ------------------------------------------------------------------------ */
void swap_fsdh(input_data_ptr)
char **input_data_ptr;
 
{
	struct input_data_hdr *input_data_hdr = (struct input_data_hdr *)*input_data_ptr;

        input_data_hdr->time.year =
		swap_2byte(input_data_hdr->time.year);
 
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
 
	input_data_hdr->bod = swap_2byte(input_data_hdr->bod);
 
	input_data_hdr->bofb =
		swap_2byte(input_data_hdr->bofb);

	if (input_data_hdr->bofb)
		blockette_swap(*input_data_ptr + input_data_hdr->bofb - 8);
}

/* ----------------------------------------------------------------------------- */
void swap_fsdh_back(input_data_ptr)
char **input_data_ptr;
 
{

	struct input_data_hdr *input_data_hdr = (struct input_data_hdr *)*input_data_ptr;

        input_data_hdr->time.year =
		swap_2byte(input_data_hdr->time.year);

	input_data_hdr->time.day = swap_2byte(input_data_hdr->time.day);
 
	input_data_hdr->time.fracsec = swap_2byte(input_data_hdr->time.fracsec);
 
	input_data_hdr->nsamples = swap_2byte(input_data_hdr->nsamples);
 
	input_data_hdr->sample_rate = swap_2byte(input_data_hdr->sample_rate);
 
	input_data_hdr->sample_rate_multiplier = swap_2byte(input_data_hdr->sample_rate_multiplier);
 
	input_data_hdr->number_time_corrections = swap_4byte(input_data_hdr->number_time_corrections);
 
	input_data_hdr->bod = swap_2byte(input_data_hdr->bod);

	/* MUST come before swap of 2 byte bofd as it will be garbage after swap
	 *
	 */
 
	if (input_data_hdr->bofb)
		blockette_swap_back(*input_data_ptr + input_data_hdr->bofb - 8);

	input_data_hdr->bofb = swap_2byte(input_data_hdr->bofb);

}

/* ----------------------------------------------------------------------------- */
#define FSDH_SIZE 48

void blockette_swap_back(struct data_blk_hdr *b_ptr)


{
	short type, next_blk_byte;

	char *base = (char *)b_ptr - FSDH_SIZE;

	while (1 == 1)
	{

		type = b_ptr->type;
		next_blk_byte = b_ptr->next_blk_byte;

		b_ptr->type = swap_2byte(b_ptr->type);

		b_ptr->next_blk_byte = swap_2byte(b_ptr->next_blk_byte);

		switch (type)
		{
                 	case 100 :
                                {
                                char *p;
				float s_rate;

                                /* must switch the sample rate - float */
                                p = (char *)&((struct data_blk_100 *)b_ptr)->sample_rate;

                                *((int *)&s_rate)=swap_4byte(*((int *)p));

                                ((struct data_blk_100 *)b_ptr)->sample_rate =
                               		s_rate;

                                }

                                break;

				
                        case 201:
				{
				char *p;
				float floater;
				int inter;

                                p = (char *)&((struct data_blk_201 *)b_ptr)->signal_amplitude;

                                *((int *)&floater)=swap_4byte(*((int *)p));

                                ((struct data_blk_201 *)b_ptr)->signal_amplitude = floater;

                                p = (char *)&((struct data_blk_201 *)b_ptr)->signal_period;

                                *((int *)&floater)=swap_4byte(*((int *)p));

                                ((struct data_blk_201 *)b_ptr)->signal_period = floater;


                                p = (char *)&((struct data_blk_201 *)b_ptr)->backgr_est;

                                *((int *)&floater)=swap_4byte(*((int *)p));

                                ((struct data_blk_201 *)b_ptr)->backgr_est = floater;

                                /* BTIME structure */

                                // year
				p = (char *)&(((struct data_blk_201 *)b_ptr)->sig_onset.year);

                                 *((int *)&inter) = swap_2byte(*((int *)p));

                                ((struct data_blk_201 *)b_ptr)->sig_onset.year = inter;

                                // day

                                p = (char *)&(((struct data_blk_201 *)b_ptr)->sig_onset.day);

                                *((int *)&inter) = swap_2byte(*((int *)p));

                                ((struct data_blk_201 *)b_ptr)->sig_onset.day = inter;

                                // frac seconds
				p = (char *)&(((struct data_blk_201 *)b_ptr)->sig_onset.fracsec);

                               	 *((int *)&inter) = swap_2byte(*((int *)p));

                                ((struct data_blk_201 *)b_ptr)->sig_onset.fracsec = inter;

                                }

                                break;


                        case 300:
                        case 310:
                        case 200:
                        case 320:
                        case 390:
                        case 395:
                        case 400:
                        case 405:
                        case 500:
                        case 1000:
                        case 1001:
                                break;

			case 2000:
				{
                                struct data_blk_2000 *ptr;

                                ptr = (struct data_blk_2000 *)b_ptr;

                                ptr->blk_length = swap_2byte(ptr->blk_length);
                                ptr->opaque_offset = swap_2byte(ptr->opaque_offset);
                                ptr->rec_num= swap_4byte(ptr->rec_num);
                                }
				break;

                        default : /* oh, oh */
	
        			fprintf(stderr,
                                        "blockette swapper back: Bad blockette scanned\n Blockette = %d\n", b_ptr->type);

                       		return;

		}		/* switch */
	
       		if (next_blk_byte == 0)
                        return;

		b_ptr = (struct data_blk_hdr *)(base + next_blk_byte);

	}   /* while */

}

/* ----------------------------------------------------------------------------- */


int process_opaque(char *lrecord_ptr, int fsdh_swapped)

{
        struct data_blk_hdr *db_hdr;
        struct data_blk_2000 *ptr;

        int rec, rec_length;
        int finished = FALSE;

        struct input_data_hdr *hdr = (struct input_data_hdr *)lrecord_ptr;

	if (hdr->bofb != 0)
       	{
		db_hdr = (struct data_blk_hdr *) (lrecord_ptr + hdr->bofb);
 
		finished = FALSE;
 
		while (!finished)
               	{
			if (db_hdr->type == 2000)
             		{
				ptr = (struct data_blk_2000 *)db_hdr;

				rec_length = ptr->blk_length;

				if (fsdh_swapped)
				{
					blockette_swap_back(db_hdr);
				}

				if (process_blk_2k(hdr, ptr, rec_length) == 0)
					return 0;
 
			}
 
			if (db_hdr->next_blk_byte == 0)
				finished = TRUE;
 
			db_hdr = (struct data_blk_hdr *)
					(lrecord_ptr + 
					  /* subtract the seq num chars */
					  (db_hdr->next_blk_byte - 8));
                        
		}
 
 
 
	}
 
        return 1;
}

/* ------------------------------------------------------------------ */
int printBlks(char *input_data_ptr)

{
        struct input_data_hdr *input_data_hdr;  /* fixed data header */
        struct data_blk_100 *blk_100;           /* blockette 100 pointer */
        int i;                                  /*counter */

        /* point to beginning data header structure */
        input_data_hdr = (struct input_data_hdr *) input_data_ptr;


        if (input_data_hdr->bofb == 0)
                return;

        blk_100 = (struct data_blk_100 *)(input_data_ptr +
                                        (input_data_hdr->bofb - 8));
        do
        {

		printf("type=%d, next=%d\n", blk_100->hdr.type, blk_100->hdr.next_blk_byte);

                if (blk_100->hdr.next_blk_byte == 0)
              		return;

		/* move to next blk */

                blk_100 = (struct data_blk_100 *)(input_data_ptr+
                                               (blk_100->hdr.next_blk_byte-8));

        } while (1);

}

/* ------------------------------------------------------------------ */


char scan_for_blk_1001(b_ptr, base)
struct data_blk_hdr *b_ptr;
char *base;             /* start of the logical rec */

{

        while (1)
        {

                if (b_ptr->type == 1001)
		{
                        /* eureka, we've found it */
                        return ((struct data_blk_1001 *)b_ptr)->usec;
		}

                if (b_ptr->next_blk_byte == 0)
                        return 0;

                /* garbage check */
                switch (b_ptr->type)
                {
                        case 100 :
                        case 201:
                        case 300:
                        case 310:
                        case 200:
                        case 320:
                        case 390:
                        case 395:
                        case 400:
                        case 405:
                        case 1000:
                                break;
                        default : /* oh, oh */

                                fprintf(stderr,
"scan_for_blk_1001(): Bad blockette scanned\n Blockette = %d\n", b_ptr->type);

                                return 0;

                }               /* switch */

                b_ptr = (struct data_blk_hdr *)(base + b_ptr->next_blk_byte);

        }   /* while */

        /* Should never get here */
        return 0;

}

/* ------------------------------------------------------------------ */


